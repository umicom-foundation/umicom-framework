![Umicom](../../resources/brand/umicom-logo.svg)

# Use the C UI SDK from a C++ application

This example checks a C++ client's ability to consume the same C23 UI services used by native Umicom applications. The implementation remains in Framework. A C++ frontend can use the component registry, enterprise cell values and workspace state without reimplementing those services.

The example shares qualification sources with the Framework repository. Keep it in this checkout. Its include paths and libraries come from an installed SDK; it does not compile Framework implementation files.

## Prepare a matching SDK

1. Install Framework with its matching headers, libraries, CMake exports and canonical brand assets.
2. Select compatible C and C++ compilers, architecture and runtime settings. A C linkage declaration does not make unrelated binary formats compatible.
3. Make required runtime libraries available before executing consumers. The CMake prefix selects a package; it does not set the operating system's DLL search path.
4. On Windows, provide a resource compiler. The three workflow executables use the exported branding helper and Umicom ICO. Runtime deployment remains a separate step.

## Qualify the toolkit-neutral contracts

From the Framework source root, replace **C:/sdk/umicom** with your installed SDK directory. Run each command separately and stop on an error.

~~~powershell
cmake -S examples/ui_cpp_consumer -B build/ui-cpp-sdk -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/sdk/umicom -DBUILD_TESTING=ON
cmake --build build/ui-cpp-sdk --target umicom-ui-sdk-cpp-checks
ctest --test-dir build/ui-cpp-sdk -R "^framework[.]sdk[.]cpp_.*[.]ui[.]" --output-on-failure
~~~

This configuration registers 75 isolated header consumers and three memory-only workflows. The header consumers retain a link reference to a representative C function. They do not invoke arbitrary product APIs. The workflows exercise component copies, typed cells, by-value selection ranges, workspace lock and clone behavior, and the shared brand palette.

## Include optional GTK4 adapter contracts

Use an SDK that exports **Umicom::ui_gtk4** and has its GTK dependencies available. Choose a separate build directory to keep the two configurations' evidence distinct.

~~~powershell
cmake -S examples/ui_cpp_consumer -B build/ui-cpp-sdk-gtk -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/sdk/umicom -DBUILD_TESTING=ON -DUMICOM_UI_CPP_WITH_GTK=ON
cmake --build build/ui-cpp-sdk-gtk --target umicom-ui-sdk-cpp-checks
ctest --test-dir build/ui-cpp-sdk-gtk -R "^framework[.]sdk[.]cpp_.*[.]ui[.]" --output-on-failure
~~~

This adds 28 adapter header consumers, for 106 checks in total. These checks establish header compilation, representative linkage and the selected memory workflows only. No GTK window is created; input, focus, accessibility, rendering, fonts and DPI still need native UI qualification.

## Compare source-tree and installed checks

For an in-tree Framework build, enable **BUILD_TESTING=ON** and **UMICOM_BUILD_CPP_UI_CONSUMER_TESTS=ON**, build **umicom-ui-cpp-checks**, then use the same CTest name pattern. **UMICOM_BUILD_GTK4_ADAPTER** controls the additional adapter contracts in that mode. Ordinary C23 builds keep the C++ option off.

An unresolved C++-encoded symbol usually calls for checking which header the compiler selected and its linkage declaration. An unresolved C symbol may indicate mismatched SDK files or a missing link dependency. A load failure after linking calls for runtime-library and architecture inspection. Record those stages separately.

See [the public UI language boundary](../../docs/development/ui-cpp-consumers.md). Successful compilation or a passing memory check is not evidence of a working Qt or GTK application.
