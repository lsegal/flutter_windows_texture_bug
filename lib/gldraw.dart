import 'dart:async';

import 'package:flutter/services.dart';

const MethodChannel _ch = const MethodChannel('gldraw');

class GLDraw {
  final int textureId;

  GLDraw._({required this.textureId});

  static Future<GLDraw> open() async {
    final id = await _ch.invokeMethod<int>('open');
    return GLDraw._(textureId: id!);
  }

  Future<void> draw() {
    return _ch.invokeMethod('draw', textureId);
  }

  void dispose() {
    _ch.invokeMethod('dispose', textureId);
  }
}
