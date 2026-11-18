![Umicom](../../resources/brand/umicom-logo.svg)

# Keep source warnings tied to the text they describe

A document can change between starting a source check and displaying its result.
The optional Framework source-check adapter gives a caller a small, explicit
contract for that situation. It delegates parsing and warnings to Umicom C and
attaches the caller's document revision. It does not execute the checked program,
launch a compiler process or implement a second language analyzer.

## Start with the existing compiler contract

The [source-check header](../../include/umicom/toolchain/cvm_source_check.h) depends
on the source-warning-capable Umicom C SDK. Its current warning identifies a local
integer whose value is never read by supported source expressions. A write alone
is not a read. Compound updates and increments are reads. Reads in unselected
branches count because this is lexical analysis, not path-sensitive dead-store
analysis. Unsupported language constructs remain errors.

The ordinary [preview service](cvm-preview.md) still compiles and executes. Choose
source checking when the user wants an observation before execution. Neither a
zero-warning report nor accepted syntax establishes runtime arithmetic safety.
An initializer such as 1 / 0 can pass checking and fail when later executed.

## Build the optional example

1. Locate a matching Umicom C installation containing both source_check.h and
   the umc_check_source symbol. A new header beside an old library is insufficient.
2. Open a terminal in the Framework source checkout. Use a new example build
   directory if an existing cache belongs to another checkout or generator.
3. Configure this small consumer, then build and run its regression tests. This
   does not require the complete Framework desktop build. The following paths
   are examples for an SDK installed in C:/sdk/umicom-c; replace that prefix with
   the actual matching installation.

~~~powershell
cmake -S examples/cvm_source_check -B build/cvm-source-check -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_PREFIX_PATH=C:/sdk/umicom-c -DBUILD_TESTING=ON
cmake --build build/cvm-source-check --parallel 2
ctest --test-dir build/cvm-source-check --output-on-failure
~~~

Run each line separately and stop on errors. A missing source_check.h indicates
that the selected SDK does not provide this source contract. An unresolved
umc_check_source symbol indicates a missing or mismatched compiler library.
Correct the dependency selection; do not remove the check or substitute a fake
successful report.

For an explicit source-tree development check, configure a different build
folder with -DUMICOM_C_PREVIEW_SOURCE=C:/src/umicom-c instead of selecting an
installed SDK. This deliberately composes that named compiler checkout. It never
chooses a neighboring directory automatically. A source-tree result does not
qualify the installed package layout or exports.

The example executable prints one warning for revision 41 and four instructions
checked, not executed. It then rejects that result for revision 42. Its Windows
icon and staged scalable artwork come from the Framework branding helper; there
is no desktop runtime deployment requirement for this small console example.

## Call the service from an application

The essential request is:

~~~c
#include "umicom/toolchain/cvm_source_check.h"

/* Inside an application function, with stable caller-owned storage: */
const char source[] = "int main(void){int unused=7;return 42;}";
uint64_t revision = 41U;
UmiCvmSourceCheckResult result;
UmiStatus status = umi_cvm_check_source(source, sizeof source - 1U,
    revision, &result, sizeof result);
if (status == UMI_STATUS_OK &&
    umi_cvm_source_check_matches(&result, sizeof result, revision)) {
    /* Present result.report.warnings[0..warning_count). */
}
~~~

The revision is an opaque identifier owned by the application. Zero is allowed.
Advance or otherwise replace it whenever the document changes. It is not a content
hash, timestamp or authentication token. Reusing the same revision after changing
bytes can make an old result appear current; the adapter cannot discover that
mistake by comparing two numbers.

A real editor should capture a stable text snapshot and its revision together,
check that snapshot, then compare the result against the current document revision
before displaying it. Coordinate concurrent access through the editor's existing
ownership rules. This adapter creates no thread, lock, queue, callback or worker.
Separate concurrent requests need separate result objects and stable input spans.

## Understand the two failure contracts

The lower-level umc_check_source API preserves its caller's report when compilation
fails. Framework instead owns a whole request result. Once the result pointer and
exact structure size are accepted, a new request clears the old report, records
the new revision and starts with completed false. A failed source check therefore
cannot leave the old warnings looking like a successful result for the new text.

Wrong result storage size or a NULL result is refused before touching that object.
This is different from an admitted result with a NULL source: the latter is a new
failed request and invalidates the old success. Do not treat every failure as
having identical publication behavior. Check the returned status, completed flag
and current revision together.

Framework translates the compiler's detailed status through the same internal
mapping used by preview. Syntax, literal and bytecode admission failures map to
parse error; source, program and related bounds map to capacity exceeded. The
embedded UmcDiagnostic retains the precise compiler status and source location.
Warnings do not change a successful status into an error.

## Display names and positions safely

Warning entries contain numeric source positions, not owned copies of the name.
Use the exact snapshot that was checked to display the indicated byte range. The
positions become stale after text edits. Offsets are zero-based; line and byte
column are one-based. Tabs and multi-byte text need a deliberate conversion to
screen coordinates. Do not assume a byte column is already a drawn caret column.

Only entries below warning_count are part of the result. A successful shorter
request replaces the prior report rather than extending it. The diagnostic
message is supplied by the compiler; the adapter does not retain application
source pointers. The input and result storage must remain disjoint and stable
throughout the synchronous call.

## Compose without changing older consumers

An application CMake project can include UmicomCPreview.cmake and call
umicom_add_cvm_source_check(my_source_checker), then link that target. This explicit
new composition adds source checking to the existing optional preview adapter.
Existing umicom_add_cvm_preview callers keep their prior dependency path and do
not automatically require the new warning header. Shared status translation is
kept in one internal owner so source-only and execution-facing statuses agree.

This adapter is not automatically added to the main Framework export and is not
wired to a new Studio gutter, Run command or OS editor. The standalone example
and tests demonstrate the service boundary. Product integration should reuse it
and add document-revision, scheduling and presentation tests before claiming a
complete editor workflow.

## Check the cases that can otherwise mislead a UI

The focused tests cover successful warnings, zero and maximum revision tokens,
syntax and literal errors after success, invalid source arguments, wrong storage
sizes, independent result objects and every truncated prefix of a source program.
They compare diagnostic fields with the lower-level compiler and verify that a
runtime division-by-zero expression is checked without execution. A deliberate
revision-reuse example explains that token comparison is not content hashing.

The existing preview and debugger regressions remain necessary after a change to
shared status translation. Installed-SDK, document editor and GUI qualification
are additional checks; a source-only adapter pass does not imply those passed.
