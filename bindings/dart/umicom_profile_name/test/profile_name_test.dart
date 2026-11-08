/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: bindings/dart/umicom_profile_name/test/profile_name_test.dart
 *
 * PURPOSE:
 *   Exercise actual Dart-to-C normalization, byte conversion and independent call lifetimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

@TestOn('vm')
library;
import 'dart:isolate';
import 'package:test/test.dart';
import 'package:umicom_profile_name/umicom_profile_name.dart';
void main() {
  test('mixed case is canonicalized by C', () {
    final result = normalizeProfileName('SaM_9-');
    expect(result.accepted, isTrue);
    expect(result.status, 0);
    expect(result.canonicalName, 'sam_9-');
  });
  test('minimum accepted length', () => expect(normalizeProfileName('AbC').canonicalName, 'abc'));
  test('maximum accepted length', () => expect(normalizeProfileName('A' * 48).canonicalName, 'a' * 48));
  for (final entry in <String, String>{
    'empty': '', 'short': 'Ab', 'long': 'A' * 49,
    'digit first': '1abc', 'punctuation': 'ab/c', 'whitespace': ' Abc',
    'embedded NUL': 'Ab\u0000c', 'non ASCII': 'Abé',
    'supplementary character': 'Ab😀', 'unpaired surrogate': 'Ab\uD800',
  }.entries) {
    test('reject ${entry.key}', () {
      final result = normalizeProfileName(entry.value);
      expect(result.status, 1); expect(result.accepted, isFalse);
      expect(result.canonicalName, isNull);
    });
  }
  test('result survives later calls and their buffer disposal', () {
    final first = normalizeProfileName('FiRsT');
    expect(normalizeProfileName('!bad').accepted, isFalse);
    expect(normalizeProfileName('SECOND').canonicalName, 'second');
    expect(first.canonicalName, 'first');
  });
  test('repeated calls have independent results', () {
    for (var i = 0; i < 1000; ++i) {
      expect(normalizeProfileName('USER_$i').canonicalName, 'user_$i');
    }
  });
  test('native binding is usable from another isolate', () async {
    final canonical = await Isolate.run(() => normalizeProfileName('WORKER').canonicalName);
    expect(canonical, 'worker');
  });
}
