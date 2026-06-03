// Copyright Jody M Sankey 2026
// This software may be modified and distributed under the terms
// of the MIT license. See the LICENCE.md file for details.

part of 'common.dart';

class AttParser extends SentenceParser {
  @override
  List<BoundValue> parse(List<String> fields) {
    _validateFieldCount(fields, 4);
    final heading = double.parse(fields[1]);
    final pitch = double.parse(fields[2]);
    final roll = double.parse(fields[3]);
    return [
      _boundSingleValue(heading, Property.heading),
      _boundSingleValue(pitch, Property.pitch),
      _boundSingleValue(roll, Property.roll),
    ];
  }
}