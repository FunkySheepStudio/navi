// Copyright Jody M Sankey 2026
// This software may be modified and distributed under the terms
// of the MIT license. See the LICENCE.md file for details.

import 'dart:convert';

import 'package:flutter/material.dart';

class MapFilesPage extends StatelessWidget {
  final String title;

  const MapFilesPage({super.key, required this.title});

  Future<List<String>> _loadMapFiles(BuildContext context) async {
    final bundle = DefaultAssetBundle.of(context);
    final manifestJson = await bundle.loadString('assets/map_manifest.json');
    final manifest = json.decode(manifestJson) as List<dynamic>;
    final files = manifest.cast<String>()..sort();
    return files;
  }

  String _extension(String path) {
    final index = path.lastIndexOf('.');
    if (index < 0 || index < path.lastIndexOf('/')) {
      return '<no extension>';
    }
    return path.substring(index).toLowerCase();
  }

  Map<String, int> _countByExtension(List<String> files) {
    final counts = <String, int>{};
    for (final path in files) {
      final ext = _extension(path);
      counts[ext] = (counts[ext] ?? 0) + 1;
    }
    return counts;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text(title)),
      body: FutureBuilder<List<String>>(
        future: _loadMapFiles(context),
        builder: (context, snapshot) {
          if (snapshot.connectionState != ConnectionState.done) {
            return const Center(child: CircularProgressIndicator());
          }
          if (snapshot.hasError) {
            return Center(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Text(
                  'Could not load map files. Make sure the map folder is listed in pubspec assets and rebuild the app.\n\nDetails: ${snapshot.error}',
                  style: Theme.of(context).textTheme.bodyMedium,
                ),
              ),
            );
          }
          final files = snapshot.data ?? [];
          final typeCounts = _countByExtension(files);
          return Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              Container(
                color: Theme.of(context).colorScheme.surface,
                padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 12.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text('Map folder overview', style: Theme.of(context).textTheme.titleMedium),
                    const SizedBox(height: 8.0),
                    Text('Total map assets: ${files.length}', style: Theme.of(context).textTheme.bodyMedium),
                    const SizedBox(height: 8.0),
                    Wrap(
                      spacing: 8.0,
                      runSpacing: 6.0,
                      children: typeCounts.entries
                          .map(
                            (entry) => Chip(
                              label: Text('${entry.key}: ${entry.value}'),
                            ),
                          )
                          .toList(),
                    ),
                  ],
                ),
              ),
              Expanded(
                child: files.isEmpty
                    ? Center(
                        child: Text(
                          'No map assets were found in the bundled map folder.',
                          style: Theme.of(context).textTheme.bodyMedium,
                          textAlign: TextAlign.center,
                        ),
                      )
                    : ListView.separated(
                        itemCount: files.length,
                        separatorBuilder: (context, index) => const Divider(height: 1),
                        itemBuilder: (context, index) {
                          final path = files[index];
                          return ListTile(
                            dense: true,
                            title: Text(path, style: Theme.of(context).textTheme.bodyLarge),
                            subtitle: Text('Type: ${_extension(path)}', style: Theme.of(context).textTheme.bodySmall),
                          );
                        },
                      ),
              ),
            ],
          );
        },
      ),
    );
  }
}
