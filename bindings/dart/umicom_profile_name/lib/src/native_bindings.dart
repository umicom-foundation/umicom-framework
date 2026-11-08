/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: bindings/dart/umicom_profile_name/lib/src/native_bindings.dart
 *
 * PURPOSE:
 *   Declare exact native ABI signatures for the shared profile-name adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

import 'dart:ffi';
const _asset = 'package:umicom_profile_name/src/native_bindings.dart';
@Native<Uint32 Function()>(assetId: _asset, symbol: 'UmiInteropProfileNameVersion')
external int profileNameVersion();
@Native<Uint32 Function()>(assetId: _asset, symbol: 'UmiInteropProfileNameCapacity')
external int profileNameCapacity();
// Size is the target size_t type. Int32 matches the bridge's fixed-width result,
// avoiding any assumption about a C enum's binary representation.
@Native<Int32 Function(Pointer<Uint8>, Size, Pointer<Uint8>, Size)>(
  assetId: _asset, symbol: 'UmiInteropProfileNameNormalize')
external int profileNameNormalize(Pointer<Uint8> name, int length,
    Pointer<Uint8> output, int capacity);
