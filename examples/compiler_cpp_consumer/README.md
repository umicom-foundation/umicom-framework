![Umicom](../../resources/brand/umicom-logo.svg)

# Consume the C compiler SDK from C++

This example qualifies the public language boundary. Framework's implementation stays in C23. C++ is used here because a C++ application, including a Qt frontend, must be able to include the public compiler headers and resolve the library's C symbols without a private wrapper.

The example reuses a qualification matrix from the repository. Its source files include installed public headers and link **Umicom::compiler**; they do not compile Framework implementation files. Keep this example within the source checkout so its shared check sources are available.

## Prepare the installation

Use a complete Framework SDK with the corrected compiler headers, its matching libraries and exported branding helper. Choose C and C++ compilers that match the SDK's architecture, calling conventions and runtime/toolchain requirements. C linkage does not make incompatible binary formats, architectures or runtimes interchangeable.

On Windows, prepare the resource compiler for the canonical Umicom icon. The workflow executable uses **umicom_apply_application_branding** with **NO_RUNTIME_DEPLOYMENT**. Shared-library runtime deployment is a separate prerequisite; a CMake prefix is not a DLL search path.

## Configure and run your qualification

From the Framework source root, adjust **C:/sdk/umicom** to your installed SDK. Run each PowerShell command separately and stop on an error.

~~~powershell
cmake -S examples/compiler_cpp_consumer -B build/compiler-cpp-sdk -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/sdk/umicom -DBUILD_TESTING=ON
cmake --build build/compiler-cpp-sdk --target umicom-compiler-sdk-cpp-checks
ctest --test-dir build/compiler-cpp-sdk -R "^framework[.]compiler[.]cpp_" --output-on-failure
~~~

The matrix creates 32 independent header-consumer executables and one planning-workflow executable. Each header is the first Framework include in its own translation unit. A volatile function pointer retains a reference to one representative exported symbol from that header. A C++ standard header is included afterwards to help detect a linkage block left open.

The workflow consumes the umbrella header, initializes a C request, plans arguments for a synthetic GCC provider, checks pointer ownership and projects a manifest's language mask. The named driver is never executed. These are expected checks; the example has no implied build, link or runtime result.

## Understand what the checks establish

- Header compilation checks the selected header and its include dependencies for the chosen C++ compiler.
- Linking checks representative symbol linkage against the selected library.
- The workflow checks a small public-API path from C++.
- None of these checks proves every ABI combination, compiler backend, real driver, Qt UI or installed application workflow.

The ordinary C23 Framework build does not require these optional C++ checks. For an in-tree owner qualification, enable **UMICOM_BUILD_CPP_COMPILER_CONSUMER_TESTS=ON** with **BUILD_TESTING=ON**, build **umicom-compiler-cpp-checks**, and select the same CTest name prefix. That mode consumes source-tree headers and built libraries; keep its evidence distinct from this installed-SDK exercise.

## Diagnose failures

If a symbol with a C++-encoded name cannot be resolved, first inspect the exact header selected by the compiler and whether it declares the function with C linkage. Do not wrap arbitrary C++ standard headers inside an external C block as a workaround.

If a C symbol is unresolved, compare the installed header and library identities, exported components and required transitive libraries. If loading fails after successful linking, inspect runtime dependencies and architecture. Retain these failures separately rather than reporting compilation as an application launch pass.

See [C and C++ compiler consumers](../../docs/development/compiler-cpp-consumers.md) for the header contract.

Sammy Hegab · Umicom Foundation · MIT
