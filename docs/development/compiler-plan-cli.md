![Umicom](../../resources/brand/umicom-logo.svg)

# Inspect a compiler plan with the native CLI

A compiler plan is a proposed executable and an ordered list of arguments. It is useful when a build command looks wrong: you can inspect the selected action, language standard, include directories and definitions before handing any work to a process runner.

The native **umicom compiler plan** command uses the same Framework request and provider services available to C applications. It prints one UTF-8 JSON document to standard output. It does not discover a compiler, open the source file, change directory or execute the proposed driver. Exit zero means the preview was produced; it says nothing about whether that driver can compile the source.

1. Use a native Umicom executable built from source that includes this command.
2. Ask for its option contract.
3. Select the actual driver, language standard and intended working directory explicitly.
4. Review the returned argument array before separately qualifying a toolchain or building the project.

~~~powershell
umicom compiler plan --help
umicom compiler plan --provider gcc --executable "C:\msys64\ucrt64\bin\gcc.exe" --language c --action compile --standard c23 --source "src\main.c" --working-directory "C:\projects\hello" --output "build\main.o" --include "include" --define "APP_MODE=1" --warnings-as-errors
~~~

This preview names an object file but does not create it. A project still needs its build configuration, libraries and link stage. Existing **umicom configure**, **build**, **test**, **install** and **launch** commands remain separate project operations. This command does not replace them or introduce an automatic execution step.

For a syntax-only proposal, select **check** and omit the output option. The command refuses **--output**, **--debug** and **--position-independent** for a check, rather than accepting options it will not use.

~~~powershell
umicom compiler plan --provider clang --executable "C:\tools\llvm\bin\clang.exe" --language c --action check --standard c23 --source "src\main.c" --working-directory "C:\projects\hello" --include "include"
umicom compiler plan --provider msvc --executable "C:\tools\compiler\cl.exe" --language cpp --action check --standard c++20 --source "src\main.cpp" --working-directory "C:\projects\hello"
~~~

Replace the illustrative paths with your installation. A requested standard is passed to the driver mapping; the preview does not verify that a particular installed compiler release supports it. The request starts with Framework's host target metadata. Clang includes that target in its argument list. This first CLI surface does not expose cross-target selection, assembly, Rust, Zig, UAI or Umicc planning, link/archive actions, or raw driver options.

## Request position-independent code

For a GCC or Clang C/C++ **compile** proposal, add **--position-independent**. The shared planner emits one **-fPIC** argument after structured warning options. Position-independent code can address data without assuming one fixed load address. It is a compilation choice, not a request to link a shared library or produce a bootable image.

~~~powershell
umicom compiler plan --provider clang --executable "C:\tools\llvm\bin\clang.exe" --language c --action compile --standard c23 --source "src\library.c" --working-directory "C:\projects\library" --output "build\library.o" --position-independent
~~~

The example only previews an argument array. The selected platform and installed driver must support that flag; Windows host metadata does not itself establish support. Do not treat the preview as a portability check. GCC documents target-dependent PIC support, and Clang lists the switch in its command-line reference. See [GCC code-generation options](https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html) and [Clang's argument reference](https://clang.llvm.org/docs/ClangCommandLineReference.html).

MSVC, syntax checks and other language/provider combinations currently refuse this explicit intent. That refusal describes Framework's reviewed mappings, not a universal claim about those tools. Omitting the flag preserves the driver's defaults; it does not force position dependence. Applications using the C request API set **position_independent = true** and receive **UMI_STATUS_NOT_IMPLEMENTED** for unmapped combinations, with the previous output plan unchanged. Raw driver options remain available through the C API and may conflict with structured choices; review them separately.

## Read the output as data

The schema has these fields:

| Field | Meaning |
| --- | --- |
| schema_version | JSON contract version, currently 1 |
| executed | Always false for this preview |
| toolchain_verified | Always false; availability is not probed |
| provider | Selected provider identifier |
| executable | Exact driver string supplied by the caller |
| working_directory | Intended directory string; no directory change occurs |
| language | Framework language enum: C is 1, C++ is 2 for this CLI |
| action | Framework action enum: compile is 1, check is 6 for this CLI |
| arguments | Ordered strings, excluding the executable |

Spaces, quotes, backslashes and control characters are JSON-escaped. Argument boundaries remain separate array elements. Do not join the array and feed it to a shell: shell quoting and process authorization are different responsibilities. A JSON consumer should preserve each string and its order, and use a separately reviewed process API if execution is intended.

A C application can call **umi_compiler_plan_json** on a complete plan. The serializer shares Protocol's UTF-8 JSON writer, rejects malformed storage and invalid UTF-8, and preserves the caller's complete output buffer on failure. Allocate **UMI_COMPILER_PLAN_JSON_CAPACITY** bytes for its documented maximum. The plan and output must remain owned and stable throughout the call, and must not overlap. Serialization neither trusts nor changes cached argument pointers.

## Recover from a refusal

Each required singleton option must appear once. Missing values, duplicate options, unknown providers/actions and unsupported options return exit 2. Repeated **--include** and **--define** values are accepted up to their request capacities and retain their order within each group. The shared planner can still refuse a plan whose final argument count or formatted argument length exceeds its bounds. Long paths are never silently shortened. Values beginning with two hyphens are interpreted as missing option values; use explicit paths that do not collide with that grammar.

If output fails, check the destination pipe or terminal. A stream can already contain a partial document when an operating-system write fails; consumers must require both complete JSON and exit zero. In-memory serializer failure preservation does not make a stream transactional.

The owner-run regressions include **framework.compiler.plan_json**, **framework.cli.compiler.admission**, **framework.compiler.position_independent**, and sixteen **framework.cli.compiler.plan** cases: the original three-provider/two-action/two-language matrix plus four GCC/Clang C/C++ PIC cases. They inspect JSON and exact argument order using deliberately nonexistent driver/source names. They do not launch compilers. These cases are authored; compiler, native CLI, installed-consumer and Studio runtime qualification remain to be executed.
