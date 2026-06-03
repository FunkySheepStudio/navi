import 'dart:async';
import 'dart:convert';
import 'dart:ffi' as ffi;
import 'dart:io' show File, Directory, Platform;
import 'dart:ui' as ui;
import 'package:flutter/services.dart';
import 'package:ffi/ffi.dart';

typedef _cm93_init_c = ffi.Int32 Function(ffi.Pointer<ffi.Int8>);
typedef _cm93_render_c = ffi.Pointer<ffi.Uint8> Function(ffi.Int32, ffi.Int32, ffi.Double, ffi.Double, ffi.Int32);
typedef _cm93_free_c = ffi.Void Function(ffi.Pointer<ffi.Uint8>);

class Cm93Ffi {
  late final ffi.DynamicLibrary _lib;
  late final int Function(ffi.Pointer<ffi.Int8>) _cm93_init;
  late final ffi.Pointer<ffi.Uint8> Function(int, int, double, double, int) _cm93_render;
  late final void Function(ffi.Pointer<ffi.Uint8>) _cm93_free;

  Cm93Ffi._(this._lib) {
    _cm93_init = _lib.lookupFunction<_cm93_init_c, int Function(ffi.Pointer<ffi.Int8>)>('cm93_init');
    _cm93_render = _lib.lookupFunction<_cm93_render_c, ffi.Pointer<ffi.Uint8> Function(int, int, double, double, int)>('cm93_render');
    _cm93_free = _lib.lookupFunction<_cm93_free_c, void Function(ffi.Pointer<ffi.Uint8>)>('cm93_free_buffer');
  }

  static Cm93Ffi? _instance;

  factory Cm93Ffi.load([String? path]) {
    if (_instance != null) return _instance!;
    final lib = _openLibrary(path);
    _instance = Cm93Ffi._(lib);
    return _instance!;
  }

  static ffi.DynamicLibrary _openLibrary(String? path) {
    if (path != null && path.isNotEmpty) return ffi.DynamicLibrary.open(path);
    if (Platform.isWindows) return ffi.DynamicLibrary.open('cm93_wrapper.dll');
    if (Platform.isMacOS) return ffi.DynamicLibrary.open('libcm93_wrapper.dylib');
    return ffi.DynamicLibrary.open('libcm93_wrapper.so');
  }

  int init(String cm93Dir) {
    final ptrUtf8 = cm93Dir.toNativeUtf8();
    final res = _cm93_init(ptrUtf8.cast<ffi.Int8>());
    malloc.free(ptrUtf8);
    return res;
  }

  Future<Directory> extractAssetDirectory(String assetPrefix) async {
    final normalizedPrefix = assetPrefix.replaceAll('\\', '/');
    String? manifestJson;
    
    // Try to load AssetManifest - try .json first, then .bin
    try {
      manifestJson = await rootBundle.loadString('AssetManifest.json');
    } catch (e) {
      try {
        // Flutter newer versions use binary format - load list of assets differently
        // Use the assets directory listing API if available
        manifestJson = null;
      } catch (e2) {
        print('Cm93: Failed to load manifest: $e2');
        manifestJson = null;
      }
    }

    final target = await Directory.systemTemp.createTemp('cm93_assets_');
    
    if (manifestJson != null) {
      // Parse JSON manifest if available
      final manifest = json.decode(manifestJson) as Map<String, dynamic>;
      final entries = manifest.keys.where((key) => key.startsWith(normalizedPrefix)).toList();
      
      for (final assetPath in entries) {
        try {
          final bytes = await rootBundle.load(assetPath);
          final relativePath = assetPath.substring(normalizedPrefix.length);
          final outFile = File('${target.path}${Platform.pathSeparator}${relativePath.replaceAll('/', Platform.pathSeparator)}');
          await outFile.parent.create(recursive: true);
          await outFile.writeAsBytes(bytes.buffer.asUint8List());
        } catch (e) {
          // Skip assets that can't be loaded
        }
      }
    } else {
      // Fallback: try common asset paths directly
      // Try assets/cm93/CM93 (the nested structure)
      try {
        await _extractAssetTree('map', target);
      } catch (e) {
        print('Cm93: Fallback extraction failed: $e');
      }
    }

    return target;
  }

  Future<void> _extractAssetTree(String assetPath, Directory targetDir) async {
    // Attempt to load assets from common CM93 subdirectories
    final subdirs = [
      '00300000', '00300060', '00300120', '00300180', '00300240', '00300300', '00300360', '00300420',
      '00300480', '00300540', '00300600', '00300660', '00300720', '00300780', '00300840', '00300900',
      '00900000', '00900060', '01500000', '02100000', '03900000', '04500000'
    ];
    
    for (final subdir in subdirs) {
      final dirPath = '$assetPath/$subdir';
      for (final fileType in ['A', 'B', 'C', 'D', 'E', 'F', 'G']) {
        try {
          final assetFile = '$dirPath/$fileType';
          final bytes = await rootBundle.load(assetFile);
          final outFile = File('${targetDir.path}${Platform.pathSeparator}CM93${Platform.pathSeparator}$subdir${Platform.pathSeparator}$fileType');
          await outFile.parent.create(recursive: true);
          await outFile.writeAsBytes(bytes.buffer.asUint8List());
        } catch (e) {
          // Asset doesn't exist, continue
        }
      }
    }
  }

  Future<int> initFromAssets(String assetPrefix) async {
    final extractedDir = await extractAssetDirectory(assetPrefix);
    return init(extractedDir.path);
  }

  Future<ui.Image?> renderAsImage(int width, int height, double lat, double lon, int zoom) async {
    final ptr = _cm93_render(width, height, lat, lon, zoom);
    if (ptr == ffi.nullptr) return null;
    final length = width * height * 4;
    final bytes = ptr.asTypedList(length);
    final completer = Completer<ui.Image>();
    ui.decodeImageFromPixels(bytes, width, height, ui.PixelFormat.rgba8888, (ui.Image img) {
      completer.complete(img);
      _cm93_free(ptr);
    });
    return completer.future;
  }
}
