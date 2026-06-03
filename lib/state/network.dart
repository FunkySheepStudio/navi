// Copyright Jody M Sankey 2022
// This software may be modified and distributed under the terms
// of the MIT license. See the LICENCE.md file for details.

import 'dart:async';
import 'dart:io';

import 'package:async/async.dart';
import 'package:flutter/services.dart';
import 'package:logging/logging.dart';
import 'package:nmea_dashboard/parsing/common.dart';
import 'package:nmea_dashboard/state/settings.dart';
import 'package:nmea_dashboard/state/values.dart';
import 'package:udp/udp.dart';

const _networkErrorRetry = Duration(seconds: 15);

final _log = Logger('Network');

/// Returns an infinite stream of valid bound values read from a port specified
/// in the supplied network settings, logging any errors. Guaranteed to return
/// (potentially null) values at least every _timeout seconds even if no network
/// traffic is present to enable cancelling.
Stream<BoundValue?> valuesFromNetwork(NetworkSettings settings) {
  NmeaParser parser = NmeaParser(settings.requireChecksum);
  switch (settings.mode) {
    case NetworkMode.tcpConnect:
      return _valuesFromTcpConnect(settings.ipAddress, settings.port, parser);
    case NetworkMode.udpListen:
      return _valuesFromUdpListen(settings.port, parser);
    case NetworkMode.simulate:
      return _valuesFromSimulation(parser);
  }
}

/// Returns an infinite stream of valid bound values read from the supplied TCP
/// address and port, logging any errors, guaranteed to return (potentially
/// null) values at least every _timeout seconds even if no network traffic is
/// present to enable cancelling.
Stream<BoundValue?> _valuesFromTcpConnect(
  InternetAddress ipAddress,
  int portNum,
  NmeaParser parser,
) async* {
  _log.info('Starting TCP stream on $ipAddress:$portNum');
  try {
    while (true) {
      try {
        var socket = await Socket.connect(ipAddress, portNum);
        await for (final value in _valuesFromPackets(socket, parser)) {
          yield value;
        }
        socket.close();
      } on SocketException catch (e) {
        _log.warning(
          'Exception opening TCP stream on $ipAddress:$portNum. '
          'Please check your network settings. ($e)',
        );
        yield null;
        await Future.delayed(_networkErrorRetry);
      }
    }
  } finally {
    _log.info('Closing TCP stream to $ipAddress:$portNum');
  }
}

/// Returns an infinite stream of valid bound values read from the supplied
/// network port, logging any errors, guaranteed to return (potentially null)
/// values at least every _timeout seconds even if no network traffic is present
/// to enable cancelling.
Stream<BoundValue?> _valuesFromUdpListen(int portNum, NmeaParser parser) async* {
  _log.info('Starting UDP listen stream on $portNum');
  try {
    while (true) {
      try {
        var receiver = await UDP.bind(Endpoint.any(port: Port(portNum)));
        await for (final value in _valuesFromPackets(
          receiver.asStream().map((d) => d?.data ?? _emptyPacket),
          parser,
        )) {
          yield value;
        }
      } on SocketException catch (e) {
        _log.warning(
          'Exception opening UDP listen on $portNum. '
          'Please check your network settings. ($e)',
        );
        yield null;
        await Future.delayed(_networkErrorRetry);
      }
    }
  } finally {
    _log.info('Closing UDP listen to $portNum');
  }
}

final Uint8List _emptyPacket = Uint8List(0);

/// Returns an empty Uint8List periodically.
Stream<Uint8List> _periodicEmptyPackets() {
  return Stream.periodic(const Duration(seconds: 3), (_) => _emptyPacket);
}

/// Returns an stream of valid values read from the supplied packet stream,
/// logging any errors, guaranteed to return (potentially null) values at
/// least every _timeout seconds even if no network traffic is present to
/// enable cancelling.
Stream<BoundValue?> _valuesFromPackets(Stream<Uint8List> packetStream, NmeaParser parser) async* {
  String remaining = '';
  await for (final packet in StreamGroup.merge([packetStream, _periodicEmptyPackets()])) {
    parser.logAndClearIfNeeded();
    if (packet.isEmpty) {
      // Empty packets are included in the stream even if no traffic is
      // present so we can return empty values that let a subscriber cancel
      // the stream.
      yield null;
    } else {
      // Process whatever we left over plus the new packet.
      remaining += String.fromCharCodes(packet);

      // Keep going while the string contains terminators or a .
      var nextSplit = _findSplit(remaining);
      while (nextSplit >= 0) {
        final potentialMessage = remaining.substring(0, nextSplit).trim();
        remaining = remaining.substring(nextSplit).trim();
        nextSplit = _findSplit(remaining);

        if (potentialMessage.isNotEmpty) {
          try {
            for (final value in parser.parseString(potentialMessage)) {
              yield value;
            }
          } on FormatException catch (e) {
            _log.warning('Error parsing $potentialMessage ${e.message}');
          }
        }
      }
    }
  }
}

/// Returns the best location to split remaing data based on the first CR LF,
/// or message start indicator, or -1 if there is none. This is needed because
/// annoyingly some networks don't CRLF terminate all messages correctly.
int _findSplit(String remainingData) {
  if (remainingData.length < 2) {
    return -1;
  }
  final nextEnd = remainingData.indexOf(RegExp(r'[\n\r]'));
  final nextStart = remainingData.indexOf(RegExp(r'[\$!]'), 1);
  if (nextEnd >= 0 && (nextStart <= 0 || nextEnd < nextStart)) {
    return nextEnd;
  } else if (nextStart > 0) {
    return nextStart;
  }
  return -1;
}

Stream<BoundValue?> _valuesFromSimulation(NmeaParser parser) async* {
  _log.info('Starting simulated NMEA stream');
  await for (final tick in Stream.periodic(const Duration(seconds: 1), (count) => count)) {
    for (final sentence in _generateSimulatedSentences(tick)) {
      try {
        for (final value in parser.parseString(sentence)) {
          yield value;
        }
      } on FormatException catch (e) {
        _log.warning('Simulation parse error for $sentence ${e.message}');
      }
    }
  }
}

List<String> _generateSimulatedSentences(int tick) {
  final timestamp = DateTime.now().toUtc();
  final timeString = _twoDigits(timestamp.hour) + _twoDigits(timestamp.minute) + _twoDigits(timestamp.second);
  // Port Camargues (Le Grau-du-Roi) approx: 43.52N, 4.03E
  final baseLat = 43.528633;
  final baseLon = 4.1249061;
  // small oscillation so simulated vessel moves slowly around the port
  final lat = baseLat; //+ ((tick % 40) * 0.0005) - 0.01;
  final lon = baseLon; //+ ((tick % 40) * 0.0005) - 0.01;
  final latString = _formatLatitude(lat);
  final southNorth = lat >= 0 ? 'N' : 'S';
  final lonString = _formatLongitude(lon);
  final eastWest = lon >= 0 ? 'E' : 'W';
  final heading = (tick * 7) % 360;
  final speedKnots = 5.0 + ((tick % 10) * 0.2);
  final speedKmh = speedKnots * 1.852;
  final depthMeters = 2.0 + ((tick % 20) * 0.1);

  final gga = _wrapNmea('GPGGA', [
    timeString,
    latString,
    southNorth,
    lonString,
    eastWest,
    '1',
    '08',
    '0.9',
    depthMeters.toStringAsFixed(1),
    'M',
    '0.0',
    'M',
    '',
    '',
  ]);
  final vtg = _wrapNmea('GPVTG', [
    heading.toStringAsFixed(1),
    'T',
    '',
    'M',
    speedKnots.toStringAsFixed(1),
    'N',
    speedKmh.toStringAsFixed(1),
    'K',
  ]);
  final vhw = _wrapNmea('GPVHW', [
    heading.toStringAsFixed(1),
    'T',
    heading.toStringAsFixed(1),
    'M',
    speedKnots.toStringAsFixed(1),
    'N',
    speedKmh.toStringAsFixed(1),
    'K',
  ]);
  final dbt = _wrapNmea('GPDBT', [
    depthMeters.toStringAsFixed(1),
    'f',
    (depthMeters * 3.28084).toStringAsFixed(1),
    'M',
    depthMeters.toStringAsFixed(1),
    'F',
  ]);
  return [gga, vtg, vhw, dbt];
}

String _twoDigits(int value) {
  return value.toString().padLeft(2, '0');
}

String _formatLatitude(double latitude) {
  final absLatitude = latitude.abs();
  final degrees = absLatitude.floor();
  final minutes = (absLatitude - degrees) * 60.0;
  return '${degrees.toString().padLeft(2, '0')}${minutes.toStringAsFixed(2).padLeft(5, '0')}';
}

String _formatLongitude(double longitude) {
  final absLongitude = longitude.abs();
  final degrees = absLongitude.floor();
  final minutes = (absLongitude - degrees) * 60.0;
  return '${degrees.toString().padLeft(3, '0')}${minutes.toStringAsFixed(2).padLeft(5, '0')}';
}

String _wrapNmea(String sentenceType, List<String> fields) {
  final body = '$sentenceType,${fields.join(',')}';
  final checksum = _calculateChecksum(body);
  return '\$$body*$checksum';
}

String _calculateChecksum(String payload) {
  var checksum = 0;
  for (final codeUnit in payload.codeUnits) {
    checksum ^= codeUnit;
  }
  return checksum.toRadixString(16).toUpperCase().padLeft(2, '0');
}
