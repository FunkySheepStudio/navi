// Copyright Jody M Sankey 2026
// This software may be modified and distributed under the terms
// of the MIT license. See the LICENCE.md file for details.

part of 'common.dart';

class FecParser extends SentenceParser {
  @override
  List<BoundValue> parse(List<String> fields) {
    _validateFieldCount(fields, 3);
    final heading = _parseSingleValue(fields[0], Property.heading);
    final pitch = _parseSingleValue(fields[1], Property.pitch);
    final roll = _parseSingleValue(fields[2], Property.roll);
    return [
      ?heading,
      ?pitch,
      ?roll,
    ];
  }
}