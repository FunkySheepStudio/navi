// Copyright Jody M Sankey 2026
// This software may be modified and distributed under the terms
// of the MIT license. See the LICENCE.md file for details.

part of 'common.dart';

class AisParser extends SentenceParser {
  @override
  List<BoundValue> parse(List<String> fields) {
    if (fields.length < 5) return [];
    final data = fields[4];
    final bits = _decodeAisData(data);
    if (bits.length < 168) return [];
    final messageType = _getInt(bits, 0, 6);
    if (messageType != 1 && messageType != 2 && messageType != 3) return [];
    final lat = _getSignedInt(bits, 89, 27) / 600000.0;
    final lon = _getSignedInt(bits, 61, 28) / 600000.0;
    final mmsi = _getInt(bits, 8, 30);
    final cog = _getInt(bits, 116, 12) / 10.0;
    final sog = _getInt(bits, 50, 10) / 10.0;
    final hdg = _getInt(bits, 128, 9);
    final ship = AisShip(mmsi, lat, lon, hdg.toDouble(), sog, cog);
    return <BoundValue<AisShipValue>>[
      BoundValue<AisShipValue>(Source.network, Property.aisShip, AisShipValue(ship)),
    ];
  }
}

List<int> _decodeAisData(String data) {
  final bits = <int>[];
  for (final char in data.codeUnits) {
    int val = char - 48;
    if (val > 40) val -= 8;
    for (int i = 5; i >= 0; i--) {
      bits.add((val >> i) & 1);
    }
  }
  return bits;
}

int _getInt(List<int> bits, int start, int length) {
  int val = 0;
  for (int i = 0; i < length; i++) {
    val = (val << 1) | bits[start + i];
  }
  return val;
}

int _getSignedInt(List<int> bits, int start, int length) {
  final val = _getInt(bits, start, length);
  final sign = (val >> (length - 1)) & 1;
  if (sign == 1) {
    return val - (1 << length);
  }
  return val;
}