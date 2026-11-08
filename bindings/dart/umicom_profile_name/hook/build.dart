/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: bindings/dart/umicom_profile_name/hook/build.dart
 *
 * PURPOSE:
 *   Build the canonical C policy and exported adapter for the requested native target without copying implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

import 'package:code_assets/code_assets.dart';
import 'package:hooks/hooks.dart';
import 'package:native_toolchain_c/native_toolchain_c.dart';

Future<void> main(List<String> args) async {
  await build(args, (input, output) async {
    if (!input.config.buildCodeAssets) return;
    // This local package remains inside the Framework source tree. Resolve all
    // inputs from packageRoot, never from the caller's current directory.
    // One stable asset ID/name is used for every architecture and Apple SDK.
    final builder = CBuilder.library(
      name: 'umicom_profile_name',
      assetName: 'src/native_bindings.dart',
      sources: [
        '../../../src/security/profile_name.c',
        '../../../adapters/ffi/profile_name.c',
        '../../../include/umicom/security/profile_name.h',
        '../../../include/umicom/interop/profile_name.h',
        '../../../include/umicom/base/status.h',
      ],
      includes: ['../../../include'],
      defines: {'UMICOM_PROFILE_FFI_BUILD': '1'},
      // The hook's Windows toolchain is MSVC. Other supported native targets
      // use a C23-capable Clang/GCC. Unsupported compilers fail visibly.
      std: input.config.code.targetOS == OS.windows ? 'clatest' : 'c23',
    );
    await builder.run(input: input, output: output);
  });
}
