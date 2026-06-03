// Copyright Jody M Sankey 2026
// This software may be modified and distributed under the terms
// of the MIT license. See the LICENCE.md file for details.

import 'dart:convert';
import 'dart:math' as math;
import 'dart:io' show File, Directory, Platform;

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'dart:ui' as ui;
import 'package:nmea_dashboard/native/cm93_ffi.dart';
import 'package:provider/provider.dart';
import 'package:nmea_dashboard/state/common.dart';
import 'package:nmea_dashboard/state/data_element.dart';
import 'package:nmea_dashboard/state/data_set.dart';
import 'package:nmea_dashboard/state/values.dart';

class AisMapPage extends StatelessWidget {
  final String title;

  const AisMapPage({super.key, required this.title});

  Future<List<String>> _loadMapAssetPaths() async {
    final manifestJson = await rootBundle.loadString('assets/map_manifest.json');
    final manifest = json.decode(manifestJson) as List<dynamic>;
    return manifest.cast<String>().map((path) => path.replaceAll('\\', '/')).toList();
  }

  int _coordToTileKey(double degrees) {
    final absDegrees = degrees.abs();
    final deg = absDegrees.floor();
    final minutes = ((absDegrees - deg) * 60).floor();
    final seconds = (((absDegrees - deg) * 60 - minutes) * 60).round();
    return deg * 1000000 + minutes * 10000 + seconds * 100;
  }

  List<String> _findNearestMaps(List<String> files, double latitude) {
    final dirValues = files
        .where((path) => path.startsWith('map/') && path.length >= 12)
        .map((path) {
          final match = RegExp(r'^map/(\d{8})/').firstMatch(path);
          if (match == null) return null;
          return int.tryParse(match.group(1)!);
        })
        .whereType<int>()
        .toSet()
        .toList();
    if (dirValues.isEmpty) {
      return files.where((path) => path.startsWith('map/')).take(5).toList();
    }

    final target = _coordToTileKey(latitude);
    dirValues.sort((a, b) => (a - target).abs().compareTo((b - target).abs()));
    final nearestDirs = dirValues.take(5).toSet();
    final result = <String>[];

    for (final path in files) {
      final match = RegExp(r'^map/(\d{8})/').firstMatch(path);
      if (match != null) {
        final value = int.tryParse(match.group(1)!);
        if (value != null && nearestDirs.contains(value)) {
          result.add(path);
        }
      }
    }

    return result.take(6).toList();
  }

  @override
  Widget build(BuildContext context) {
    return FutureBuilder<List<String>>(
      future: _loadMapAssetPaths(),
      builder: (context, manifestSnapshot) {
        return Consumer<DataSet>(
          builder: (context, dataSet, child) {
            final gpsElement = dataSet
                .find(Source.network, Property.gpsPosition.name)
                as DataElement<DoubleValue<double>, DoubleValue<double>>?;
            final ships = dataSet.aisShips.values.toList()
              ..sort((a, b) => a.mmsi.compareTo(b.mmsi));
            final mapPaths = manifestSnapshot.data ?? <String>[];
            final selectedMapPaths = (gpsElement != null && gpsElement.value != null)
              ? _findNearestMaps(mapPaths, gpsElement.value!.first)
              : <String>[];
            final ownPosition = gpsElement?.value;

            return AnimatedBuilder(
              animation: gpsElement ?? dataSet,
              builder: (context, child) {
                return Scaffold(
                  appBar: AppBar(title: Text(title)),
                  body: Column(
                    children: [
                      Expanded(
                        child: Padding(
                          padding: const EdgeInsets.all(8.0),
                          child: Container(
                            decoration: BoxDecoration(
                              color: Theme.of(context).colorScheme.surfaceContainerHighest,
                              borderRadius: BorderRadius.circular(16.0),
                            ),
                            child: ownPosition == null
                                ? Center(
                                    child: Text(
                                      'Waiting for own GPS position...',
                                      style: Theme.of(context).textTheme.titleMedium,
                                    ),
                                  )
                                : _AisMapWidget(
                                    ownLatitude: ownPosition.first,
                                    ownLongitude: ownPosition.second,
                                    ships: ships,
                                    theme: Theme.of(context),
                                  ),
                          ),
                        ),
                      ),
                      Container(
                        padding:
                            const EdgeInsets.symmetric(horizontal: 12.0, vertical: 8.0),
                        color: Theme.of(context).colorScheme.surface,
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.stretch,
                          children: [
                            Text(
                              'Own position: ${ownPosition != null ? '${ownPosition.first.toStringAsFixed(5)}, ${ownPosition.second.toStringAsFixed(5)}' : 'unknown'}',
                              style: Theme.of(context).textTheme.bodyMedium,
                            ),
                            const SizedBox(height: 8.0),
                            Text(
                              'AIS targets: ${ships.length}',
                              style: Theme.of(context).textTheme.bodyMedium,
                            ),
                            const SizedBox(height: 8.0),
                            Text(
                              'Map files near position:',
                              style: Theme.of(context).textTheme.bodyMedium,
                            ),
                            if (selectedMapPaths.isEmpty)
                              Padding(
                                padding: const EdgeInsets.only(top: 6.0),
                                child: Text(
                                  ownPosition == null
                                      ? 'Waiting for GPS before choosing maps.'
                                      : 'No numeric map directories found in assets.',
                                  style: Theme.of(context).textTheme.bodySmall,
                                ),
                              )
                            else ...[
                              const SizedBox(height: 6.0),
                              SizedBox(
                                height: 100,
                                child: ListView.builder(
                                  itemCount: selectedMapPaths.length,
                                  itemBuilder: (context, index) {
                                    final path = selectedMapPaths[index];
                                    return Padding(
                                      padding:
                                          const EdgeInsets.symmetric(vertical: 2.0),
                                      child: Text(
                                        path,
                                        style:
                                            Theme.of(context).textTheme.bodySmall,
                                      ),
                                    );
                                  },
                                ),
                              ),
                            ],
                            if (ships.isNotEmpty) ...[
                              const SizedBox(height: 8.0),
                              SizedBox(
                                height: 120,
                                child: ListView.builder(
                                  itemCount: ships.length,
                                  itemBuilder: (context, index) {
                                    final ship = ships[index];
                                    return Padding(
                                      padding: const EdgeInsets.symmetric(vertical: 4.0),
                                      child: Text(
                                        'MMSI ${ship.mmsi}: ${ship.lat.toStringAsFixed(4)}, ${ship.lon.toStringAsFixed(4)}  '
                                        'HDG ${ship.heading.toStringAsFixed(0)}° SOG ${ship.speed.toStringAsFixed(1)} kn COG ${ship.course.toStringAsFixed(1)}°',
                                        style:
                                            Theme.of(context).textTheme.bodySmall,
                                      ),
                                    );
                                  },
                                ),
                              ),
                            ],
                          ],
                        ),
                      ),
                    ],
                  ),
                );
              },
            );
          },
        );
      },
    );
  }
}

class _AisMapPainter extends CustomPainter {
  final double ownLatitude;
  final double ownLongitude;
  final List<AisShip> ships;
  final ThemeData theme;

  _AisMapPainter({
    required this.ownLatitude,
    required this.ownLongitude,
    required this.ships,
    required this.theme,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final rect = Offset.zero & size;
    final gradient = LinearGradient(
      begin: Alignment.topCenter,
      end: Alignment.bottomCenter,
      colors: [
        theme.colorScheme.primary.withOpacity(0.12),
        theme.colorScheme.primary.withOpacity(0.06),
      ],
    );
    final background = Paint()..shader = gradient.createShader(rect);
    canvas.drawRect(rect, background);

    // subtle radial highlight around own ship to indicate center
    final center = Offset(size.width / 2, size.height / 2);
    final radial = RadialGradient(colors: [theme.colorScheme.primary.withOpacity(0.08), Colors.transparent]);
    final radialPaint = Paint()..shader = radial.createShader(rect);
    canvas.drawCircle(center, math.min(size.width, size.height) * 0.6, radialPaint);

    final borderPaint = Paint()
      ..color = theme.colorScheme.onSurface.withAlpha((0.55 * 255).round())
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2.0;
    canvas.drawRect(Offset.zero & size, borderPaint);

    final gridPaint = Paint()
      ..color = theme.colorScheme.onSurface.withAlpha((0.12 * 255).round())
      ..strokeWidth = 1.0;

    for (int i = 1; i < 5; i++) {
      final dx = size.width * i / 5;
      final dy = size.height * i / 5;
      canvas.drawLine(Offset(dx, 0), Offset(dx, size.height), gridPaint);
      canvas.drawLine(Offset(0, dy), Offset(size.width, dy), gridPaint);
    }

    // draw faint latitude/longitude tick labels around edges
    final tickStyle = TextStyle(color: theme.colorScheme.onSurface.withAlpha((0.2 * 255).round()), fontSize: 10);
    _drawText(canvas, '${ownLatitude.toStringAsFixed(3)}N', Offset(6, 6), tickStyle);
    _drawText(canvas, '${ownLongitude.toStringAsFixed(3)}E', Offset(size.width - 80, 6), tickStyle);

    final labelStyle = TextStyle(
      color: theme.colorScheme.onSurface.withAlpha((0.7 * 255).round()),
      fontSize: 12,
    );
    _drawText(canvas, 'Own ship', center + const Offset(8, -18), labelStyle);

    final latDeltas = ships.map((ship) => (ship.lat - ownLatitude).abs()).toList();
    final lonDeltas = ships.map((ship) => (ship.lon - ownLongitude).abs()).toList();
    final maxLatDelta = latDeltas.isEmpty ? 0.01 : latDeltas.reduce(math.max);
    final maxLonDelta = lonDeltas.isEmpty ? 0.01 : lonDeltas.reduce(math.max);
    final visibleLat = math.max(maxLatDelta * 1.25, 0.01);
    final visibleLon = math.max(maxLonDelta * 1.25, 0.01);
    final scale = math.min(size.width / 2.2 / visibleLon, size.height / 2.2 / visibleLat);

    final ownPaint = Paint()..color = theme.colorScheme.primary;
    canvas.drawCircle(center, 8.0, ownPaint);
    canvas.drawCircle(center,
        20.0,
        ownPaint
          ..style = PaintingStyle.stroke
          ..strokeWidth = 2.0);

    final shipPaint = Paint()..color = theme.colorScheme.secondary;
    final labelTextStyle = TextStyle(color: theme.colorScheme.onSurface, fontSize: 11);
    for (final ship in ships) {
      final dx = (ship.lon - ownLongitude) * scale;
      final dy = -(ship.lat - ownLatitude) * scale;
      final position = center.translate(dx, dy);
      canvas.drawCircle(position, 6.0, shipPaint);
      _drawText(canvas, ship.mmsi.toString(), position + const Offset(8, -8), labelTextStyle);
    }

    final outlinePaint = Paint()
      ..color = theme.colorScheme.onSurface.withAlpha((0.35 * 255).round())
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.5;
    canvas.drawCircle(center, math.min(size.width, size.height) * 0.45, outlinePaint);
  }

  void _drawText(Canvas canvas, String text, Offset position, TextStyle style) {
    final textSpan = TextSpan(text: text, style: style);
    final textPainter = TextPainter(text: textSpan, textDirection: TextDirection.ltr);
    textPainter.layout();
    textPainter.paint(canvas, position);
  }

  @override
  bool shouldRepaint(covariant _AisMapPainter oldDelegate) {
    return oldDelegate.ownLatitude != ownLatitude ||
        oldDelegate.ownLongitude != ownLongitude ||
        oldDelegate.ships.length != ships.length ||
        !listEquals(oldDelegate.ships, ships);
  }
}

class _AisMapWidget extends StatefulWidget {
  final double ownLatitude;
  final double ownLongitude;
  final List<AisShip> ships;
  final ThemeData theme;

  const _AisMapWidget({
    required this.ownLatitude,
    required this.ownLongitude,
    required this.ships,
    required this.theme,
    Key? key,
  }) : super(key: key);

  @override
  State<_AisMapWidget> createState() => _AisMapWidgetState();
}

class _AisMapWidgetState extends State<_AisMapWidget> {
  ui.Image? _chartImage;
  bool _loading = false;

  @override
  void didUpdateWidget(covariant _AisMapWidget oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (oldWidget.ownLatitude != widget.ownLatitude || oldWidget.ownLongitude != widget.ownLongitude) {
      _requestRender();
    }
  }

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) => _requestRender());
  }

  Future<void> _requestRender() async {
    if (_loading) return;
    setState(() { _loading = true; });
    try {
      final cm = Cm93Ffi.load();
      var initResult = -1;
      try {
        initResult = await cm.initFromAssets('assets/map/');
        debugPrint('Cm93 initFromAssets => $initResult');
      } catch (e) {
        debugPrint('Cm93 initFromAssets error: $e');
      }
      if (initResult != 0) {
        final candidatePaths = <String>[Directory.current.path];
        try {
          final exeDir = File(Platform.resolvedExecutable).parent;
          candidatePaths.add(exeDir.path);
          var parent = exeDir;
          for (var i = 0; i < 4; i++) {
            if (!parent.parent.existsSync()) break;
            parent = parent.parent;
            if (!candidatePaths.contains(parent.path)) {
              candidatePaths.add(parent.path);
            }
          }
        } catch (_) {}

        for (final path in candidatePaths) {
          if (initResult == 0) break;
          initResult = cm.init(path);
          debugPrint('Cm93 init($path) => $initResult');
        }
      }
      if (initResult != 0) {
        debugPrint('Cm93 initialization failed; using fallback renderer');
      }

      final img = await cm.renderAsImage(1024, 1024, widget.ownLatitude, widget.ownLongitude, 10);
      if (!mounted) return;
      setState(() {
        _chartImage = img;
      });
    } catch (e) {
      debugPrint('Cm93 render error: $e');
    } finally {
      if (mounted) setState(() { _loading = false; });
    }
  }

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(builder: (context, constraints) {
      return Stack(children: [
        if (_chartImage != null)
          Positioned.fill(
            child: RawImage(image: _chartImage, fit: BoxFit.cover),
          ),
        CustomPaint(
          painter: _AisMapPainter(
            ownLatitude: widget.ownLatitude,
            ownLongitude: widget.ownLongitude,
            ships: widget.ships,
            theme: widget.theme,
          ),
          size: Size.infinite,
        ),
        if (_loading)
          const Positioned(top: 8, right: 8, child: SizedBox(width: 16, height: 16, child: CircularProgressIndicator(strokeWidth: 2.0))),
      ]);
    });
  }
}
