import 'dart:async';

import 'package:gldraw/gldraw.dart';
import 'package:flutter/material.dart';

void main() {
  runApp(App());
}

class App extends StatefulWidget {
  @override
  _AppState createState() => _AppState();
}

class _AppState extends State<App> {
  late Future<GLDraw> _texFuture;

  @override
  void initState() {
    super.initState();
    _texFuture = _loadTexture();
  }

  Future<GLDraw> _loadTexture() async {
    // load texture and continually trigger a new frame to be drawn every 500ms
    final tex = await GLDraw.open();
    Timer.periodic(Duration(milliseconds: 500), (_) => tex.draw());
    return tex;
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      theme: ThemeData(primarySwatch: Colors.blue),
      home: Scaffold(
        body: Column(
          children: [
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: Text(
                'Expecting Icon()s on left and Texture() on right\n' +
                    'Resize window to see viewport corruption of all non-Texture viewports',
                style: TextStyle(fontSize: 24),
                textAlign: TextAlign.center,
              ),
            ),
            Expanded(
              child: Row(
                children: [
                  Expanded(
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      children: [
                        Icon(Icons.account_circle_rounded, size: 200),
                        Icon(Icons.analytics_rounded, size: 200),
                      ],
                    ),
                  ),
                  Expanded(
                    child: FutureBuilder<GLDraw>(
                      future: _texFuture,
                      builder: (context, snapshot) {
                        if (snapshot.connectionState ==
                            ConnectionState.waiting) {
                          return CircularProgressIndicator.adaptive();
                        }
                        return Texture(textureId: snapshot.data!.textureId);
                      },
                    ),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
