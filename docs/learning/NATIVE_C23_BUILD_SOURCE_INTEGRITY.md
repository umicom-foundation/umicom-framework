# Understanding Missing Source Files in a C23/CMake Build

Umicom Framework develops reusable services in C23 and Assembly. CMake
connects source files, public headers, libraries and tests into one build.
Before changing a compiler option, distinguish a **configuration failure** from
an error in a C source file, an executable that cannot start, or a test whose
assertions failed.

## 1. Four different failure stages

A CMake configuration can fail before Ninja receives an executable to build.
For example, an `include()` call names a `.cmake` file that is not present, or
a public-header inventory declares a file that does not exist. Those are
**source-composition failures**, not C23 syntax errors.

At the compilation stage, a compiler might reject incompatible pointer types,
a missing symbol, or invalid C23 syntax. A linker problem occurs later, when
object files are available but an expected implementation is missing.

CTest is another stage. It may report `Not Run` when a test executable was not
built or cannot start. Only a test process that actually runs can establish
whether its own assertions pass or fail. A test registered in CMake is not
proof that the executable exists.

## 2. Verify declared interfaces and implementations together

Consider an example component with:

```text
include/umicom/example/widget.h
src/example/widget.c
tests/example/test_widget.c
```

A public header declares the interface. The C file defines its behaviour. A
CTest test should verify both normal and rejected inputs, including state
ownership. Adding an empty source file simply to satisfy CMake is not a fix:
it would hide the missing implementation and commonly fail at link time.

A shared service such as Umicom's record chain is preferable to reimplementing
its hashes in Bank, TMS or individual applications. A committed financial event
should be encoded using its existing canonical codec before hashing; raw C
struct memory may contain padding and is not a stable interchange format.

## 3. Inspect a Windows UCRT64 build

In PowerShell, run these commands from the Umicom Applications root:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake --preset windows-ucrt64-all-debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Configuration failed.' }
cmake --build --preset windows-ucrt64-all-debug --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
ctest --preset windows-ucrt64-all-debug --parallel 2 --output-on-failure --no-tests=error
```

The explicit failure checks are deliberate. If configuration fails, do not
start the build or full test suite against an obsolete build directory. It may
contain earlier test registrations and missing executables unrelated to the
current source error.

For a particular test, begin with a **focused build target** and its CTest
name. To investigate a process that cannot start, check whether the executable
exists, whether its runtime DLLs are available and whether endpoint protection
has quarantined it. Never disable security controls merely to obtain a green
build report.

## 4. File integrity and publication policy

Public source code contains stable interfaces, implementations, tests,
licences and educational explanation. Build command sequences and local merge
instructions may be supplied separately to contributors. Do not mix personal
merge notes into course lessons or turn error outputs into claims of verified
build success.

## 5. Exercises

1. Explain why `include()` failure is a CMake configuration error rather than
   a C23 language error.
2. Identify the missing file that causes a header-inventory error in a sample
   project, and determine what behaviour the corresponding implementation must
   provide before creating it.
3. Describe the difference between a CTest `Not Run` result and an assertion
   failure from a running test.
4. Demonstrate how a canonical event encoder and a shared cryptographic record
   chain allow another module to verify ordered history without duplicating
   the hashing algorithm.

**Key principle:** complete source composition, then compile, then link, then
execute tests. A later stage cannot repair the absence of an earlier one.
