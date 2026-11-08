<!--
Umicom Framework
File: bindings/dart/umicom_profile_name/README.md

PURPOSE:
Explain native asset build ownership, ABI mapping and qualification of the local Dart package.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Framework profile names from Dart

This local package calls the canonical Framework C naming policy. It accepts 3–48 ASCII letters, digits, hyphen and underscore, starting with a letter, and returns lowercase text. It does not create an account, authenticate a user or save data.

The package is intended to remain at bindings/dart/umicom_profile_name inside a complete Framework checkout. It is not published to pub.dev and is not a standalone copy of the C implementation. The build hook resolves source paths relative to the package root.

## Prepare and inspect

1. Install Dart 3.10 or later, or a Flutter SDK containing a compatible Dart SDK. Install the native compiler for your chosen target.
2. Read hook/build.dart and pubspec.yaml before resolving dependencies. The hook compiles local C code during run/test/build. It does not download a Framework binary or invoke a shell script.
3. Run the following commands from this package directory, stopping on errors. Dependency resolution downloads packages and creates a local lockfile. Retain that lockfile with your qualification evidence.

```powershell
dart --version
dart pub get
dart analyze
dart test test/profile_name_test.dart
```

These are commands to execute on your own toolchain; authored tests are not passing results. The hook selects C23 for Clang/GCC and the latest C mode for the Windows MSVC toolchain. An older compiler may reject that mode. Check the compiler actually selected rather than silently substituting a different ABI or a desktop archive for a mobile target.

## Call the service

```dart
import 'package:umicom_profile_name/umicom_profile_name.dart';

final result = normalizeProfileName('SaM_9-');
if (result.accepted) {
  print(result.canonicalName); // Expected: sam_9-
} else {
  print('Name refused: ${result.status}');
}
```

Invalid names return status 1 with no canonical text. Native loading, allocation and incompatible ABI errors stay visible. The result owns a Dart string; native pointers never escape the call. An arena releases both temporary buffers after the string has been copied, including on exceptions. Embedded NUL and non-ASCII input reach the shared byte policy and are refused. Long Dart inputs are rejected before allocating native buffers using the ABI capacity bound.

## ABI and build identity

The C bridge uses uint32_t version/capacity, int32_t status and size_t byte counts. Dart binds these as Uint32, Int32 and Size. Native enum layout and C heap ownership do not cross the boundary. The asset ID is package:umicom_profile_name/src/native_bindings.dart and remains the same for every architecture and Apple SDK. The native library basename also remains stable.

CMake consumers can link Umicom::profile_name_ffi. Its 21 C cases check capacity, overlap, exact spans, padding and unchanged output after refusal. Dart cases additionally exercise string conversion, native asset resolution, immutable results and isolate use. A repeated-call test does not by itself prove absence of memory leaks.

## Integrate and qualify targets

Add this package as a path dependency to a native Dart or Flutter application. Preserve the complete Framework checkout at that path. Browser dart:ffi is not supported by this package; use the separately designed web adapter rather than pretending a desktop pass covers the browser.

A host Dart test does not qualify Android packaging or an iPhone build. Run target-specific integration tests through the corresponding SDK; Apple targets require the appropriate Apple development environment. This package supplies no product screen or new layout.

## Recovery

For a missing asset or symbol, check the complete Framework tree, selected package path, asset ID and exported bridge symbols. For an ABI mismatch, use matching C and Dart source. For dependency-resolution or compiler failure, retain the exact versions and diagnostic; do not report a native call as tested.

The implementation follows the current [Flutter native binding guide](https://docs.flutter.dev/interop/bind-native-code), [Dart hooks guide](https://dart.dev/tools/hooks), [native toolchain API](https://pub.dev/documentation/native_toolchain_c/latest/native_toolchain_c/CBuilder-class.html) and [Dart C interop types](https://dart.dev/interop/c-interop). Native-toolchain APIs can change; the package pins the reviewed C builder version while owner qualification checks the resolved dependency set.
