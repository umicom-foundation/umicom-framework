<!--
Umicom Framework
File: framework/docs/guides/private-native-runtime.md

PURPOSE:
  Stage coherent Windows DLL dependencies for native console helpers.

EDUCATIONAL OVERVIEW:
  An executable name and its public protocol remain stable while deployment
  supplies the runtime libraries needed before its entry point can execute.

AUTHOR AND ORGANISATION:
  Sammy Hegab
  Umicom Foundation

LICENCE:
  MIT for Umicom contributions. Existing third-party terms remain applicable.
-->

# Private runtime for native helpers

`cmake/UmicomNativeRuntime.cmake` provides an opt-in
`umicom_prepare_native_runtime` composition function. Call it in the source
directory that owns the executable, after its `install(TARGETS ...)` rule:

```cmake
include("${UMICOM_FRAMEWORK_SOURCE_DIR}/cmake/UmicomNativeRuntime.cmake")
umicom_prepare_native_runtime(
    TARGET umicom-ai-bridge
    RUNTIME_DIR "C:/msys64/ucrt64/bin"
    DESTINATION tools/native)
```

The implementation reuses Framework deployment filename, PE architecture and
copy checks. It uses CMake runtime-dependency scanning with the selected toolchain's
objdump, not a Python interpreter or arbitrary PATH tools. Normal Windows hosts
stage after linking, refresh on the ALL build, and stage at the selected install
destination. Missing libraries can therefore be repaired without renaming or
removing the executable.

The selected runtime must be the coherent installation used to link the target.
System libraries remain owned by Windows. Unknown/conflicting dependencies,
architecture mismatches and existing differing private DLLs are refused before
copying. The scanner process excludes inherited Conda library paths; account PATH
settings are unchanged. A copy failure can leave newly created dependencies but
never replaces a conflicting existing file.

This helper is for console targets and does not collect GTK schemas or plug-in
resources. Existing GUI deployment remains separate and unchanged. Additional
libraries loaded dynamically by name are not discovered by an import scan.
Redistribution also requires all applicable licence notices.

Cross-compilation-only builds must disable the consuming application's runtime
staging option. Native Windows runtime deployment and loader qualification require
a Windows host; parsing PE bytes on another host is not the same verification.

References:

- https://cmake.org/cmake/help/latest/command/file.html#get-runtime-dependencies
- https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
