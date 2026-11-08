/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: bindings/dart/umicom_profile_name/lib/umicom_profile_name.dart
 *
 * PURPOSE:
 *   Own temporary native buffers while delegating naming rules to the shared Framework C implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

import 'dart:convert';
import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'src/native_bindings.dart' as native;

/// An immutable observation of name validation; it creates no account or file.
final class ProfileNameResult {
  const ProfileNameResult._(this.status, this.canonicalName);
  final int status;
  final String? canonicalName;
  bool get accepted => status == 0;
}

/// Validate through Framework. Invalid names return status 1 and no text.
/// Native asset/loading or ABI mismatch errors remain visible to the caller.
ProfileNameResult normalizeProfileName(String input) {
  final version = native.profileNameVersion();
  final capacity = native.profileNameCapacity();
  if (version != 1 || capacity != 49) {
    throw StateError('Unsupported Framework profile ABI: $version/$capacity');
  }
  // Bound allocation using the ABI's capacity. Any accepted ASCII name uses
  // one UTF-16 unit per byte, so a longer Dart input cannot fit the C contract.
  // All character and leading-letter rules still belong to Framework.
  if (input.length >= capacity) return const ProfileNameResult._(1, null);
  final bytes = utf8.encode(input);
  return using((arena) {
    final name = arena<Uint8>(bytes.isEmpty ? 1 : bytes.length);
    final output = arena<Uint8>(capacity);
    name.asTypedList(bytes.length).setAll(0, bytes);
    final status = native.profileNameNormalize(name, bytes.length, output, capacity);
    if (status != 0) return ProfileNameResult._(status, null);
    // Decode only this owned bounded buffer before the arena releases it.
    final view = output.asTypedList(capacity);
    final end = view.indexOf(0);
    if (end < 0) throw StateError('Framework returned an unterminated profile name');
    final canonical = ascii.decode(view.sublist(0, end));
    return ProfileNameResult._(0, canonical);
  });
}
