![Umicom](../../resources/brand/umicom-logo.svg)

# Preserve compiler identities at construction

A compiler provider describes a driver; it does not run or discover it. Its executable path is an identity that must arrive intact. Silently shortening a path can select a different file if a later process service uses the descriptor. A shortened target can also misrepresent the intended platform.

The shared factory implementation now admits all text before construction. The GCC, Clang, MSVC, Rust, Zig, assembler, UAI and Umicc convenience functions use this implementation. A string must terminate within its destination capacity. There is no truncation fallback.

1. Pass the explicit driver and target strings to the appropriate factory.
2. Inspect the returned provider before using it. Refused text yields an invalid, zero-initialized provider with revision zero. A valid factory result starts at revision one.
3. Treat a null or empty driver as a declared but unavailable provider. A nonempty driver sets the availability field without checking that the file exists.
4. Submit a request to the shared provider planner and inspect its status. Successful metadata construction is not proof of a working compiler or supported target.

~~~c
UmiCompilerProvider provider = umi_compiler_gcc_provider(driver_path, target_triple);
if (provider.revision == 0U) {
    /* Ask the caller to correct the complete input; do not trim it. */
    return UMI_STATUS_INVALID_ARGUMENT;
}
~~~

The maximum accepted executable text is 2,047 bytes plus its terminator. The target text limit is 191 bytes plus its terminator. These are storage limits, not promises about host path or target support. Strings passed from other code must refer to readable storage until their terminator or the documented bound.

The owner regression target is **umicom-compiler-factory-admission-test**, registered as **framework.compiler.factory_admission**. It covers all eight factories, exact storage boundaries, overlong and unterminated input, optional defaults and invalid private identity input. It does not start a compiler or inspect the filesystem. The old construction remains disabled beside its replacement for engineering review. The regression is authored and awaits execution.
