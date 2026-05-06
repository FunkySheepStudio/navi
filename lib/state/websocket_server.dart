// Copyright Jody M Sankey 2026
// This software may be modified and distributed under the terms
// of the MIT license. See the LICENCE.md file for details.

import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:logging/logging.dart';
import 'package:nmea_dashboard/state/values.dart';

final _webSocketLog = Logger('NmeaWebsocketServer');

/// A simple WebSocket server that broadcasts parsed NMEA values to connected
/// clients.
class NmeaWebsocketServer {
  static const int defaultPort = 9090;

  final InternetAddress address;
  final int port;

  HttpServer? _server;
  final Set<WebSocket> _clients = {};

  NmeaWebsocketServer({InternetAddress? address, this.port = defaultPort})
      : address = address ?? InternetAddress.anyIPv4;

  /// Creates and starts a new WebSocket server on the default address and
  /// port.
  static Future<NmeaWebsocketServer> createAndStart({InternetAddress? address, int port = defaultPort}) async {
    final server = NmeaWebsocketServer(address: address, port: port);
    try {
      await server.start();
    } catch (error, stack) {
      _webSocketLog.warning('Could not start WebSocket server on $address:$port: $error', error, stack);
    }
    return server;
  }

  /// Indicates whether the server is currently listening for client
  /// connections.
  bool get isRunning => _server != null;

  /// Starts the server and begins accepting WebSocket connections.
  Future<void> start() async {
    if (isRunning) {
      return;
    }
    _webSocketLog.info('Starting WebSocket server on $address:$port');
    _server = await HttpServer.bind(address, port);
    _server!.listen(_handleHttpRequest, onError: (error, stack) {
      _webSocketLog.warning('WebSocket server HTTP error: $error', error, stack);
    });
  }

  Future<void> _handleHttpRequest(HttpRequest request) async {
    if (WebSocketTransformer.isUpgradeRequest(request)) {
      try {
        final socket = await WebSocketTransformer.upgrade(request);
        _addClient(socket);
      } catch (error, stack) {
        _webSocketLog.warning('Failed to upgrade HTTP request to WebSocket: $error', error, stack);
        request.response.statusCode = HttpStatus.internalServerError;
        await request.response.close();
      }
    } else {
      request.response.statusCode = HttpStatus.badRequest;
      request.response.headers.contentType = ContentType.text;
      request.response.write('This endpoint only accepts WebSocket connections.');
      await request.response.close();
    }
  }

  void _addClient(WebSocket socket) {
    _webSocketLog.info('WebSocket client connected: ${socket.hashCode}');
    _clients.add(socket);

    socket.done.then((_) {
      _clients.remove(socket);
      _webSocketLog.info('WebSocket client disconnected: ${socket.hashCode}');
    }).catchError((error, stack) {
      _clients.remove(socket);
      _webSocketLog.warning('WebSocket client error: $error', error, stack);
    });
  }

  /// Broadcasts a parsed NMEA value to every connected WebSocket client.
  void broadcast(BoundValue value) {
    if (!isRunning || _clients.isEmpty) {
      return;
    }
    final message = _messageToJson(value);
    for (final socket in List<WebSocket>.from(_clients)) {
      try {
        socket.add(message);
      } catch (error, stack) {
        _webSocketLog.warning('Error sending WebSocket message: $error', error, stack);
        _clients.remove(socket);
      }
    }
  }

  String _messageToJson(BoundValue value) {
    return jsonEncode({
      'source': value.source.name,
      'property': value.property.name,
      'dimension': value.property.dimension.name,
      'tier': value.tier,
      'valueType': value.value.runtimeType.toString(),
      'value': _serializeValue(value.value),
      'receivedAt': DateTime.now().toUtc().toIso8601String(),
    });
  }

  dynamic _serializeValue(Value value) {
    if (value is SingleValue<double>) {
      return value.data;
    }
    if (value is SingleValue<DateTime>) {
      return value.data.toUtc().toIso8601String();
    }
    if (value is DoubleValue<double>) {
      return {
        'first': value.first,
        'second': value.second,
      };
    }
    if (value is AugmentedBearing) {
      return {
        'bearing': value.bearing,
        'variation': value.variation,
      };
    }
    return value.serialize();
  }

  /// Stops the server and disconnects all clients.
  Future<void> stop() async {
    for (final client in List<WebSocket>.from(_clients)) {
      await client.close(WebSocketStatus.normalClosure, 'Server is stopping');
    }
    _clients.clear();
    if (_server != null) {
      await _server!.close(force: true);
      _server = null;
    }
    _webSocketLog.info('WebSocket server stopped');
  }
}
