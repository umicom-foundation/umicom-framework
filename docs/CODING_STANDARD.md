<!--
Umicom Framework
File: docs/CODING_STANDARD.md
PURPOSE: Explain shared C23 coding and regression practices for contributors.
AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
LICENCE: MIT
-->

# Coding standard

This guide applies to new Framework code and to application code that calls it.
It complements the existing API contracts and the [documentation style guide](DOCUMENTATION_STYLE_GUIDE.md).
It does not change an existing function's documented behaviour. Formatting helps review;
ownership checks and meaningful regression tests are what detect behavioural mistakes.

## Start from the supported contract

1. Read the public declaration, implementation, direct callers and relevant tests.
2. Check the source revision and build configuration associated with a reported failure.
   A test name alone does not identify the binary or source that ran.
3. Write down the observable behaviour to preserve and the failing input or transition.
4. Put reusable behaviour in Framework. Applications provide their configuration,
   branding and workflow composition rather than another copy of the service.
5. Keep private logs, personal information, working conversations and internal plans
   outside public source and documentation. Use synthetic data in examples and tests.

## Language and formatting

Use C23 for native services. Introduce architecture-specific Assembly only behind a
portable interface with a reference implementation and equivalence coverage. Use the
repository's existing CMake targets and warning settings. Do not silence a warning
without understanding the operation and its permitted inputs.

The root .clang-format follows the existing Applications defaults: LLVM base style,
two spaces, no indentation tabs, and a 100-column limit. Keep established surrounding
formatting when touching older code. Format the relevant function or new file; do not
reformat unrelated files. Existing generated templates can declare their own style.
Save source as UTF-8 and follow .gitattributes for line endings.

Use one statement per line and braces around multi-step conditions. Prefer an early
return for a failed precondition. Give variables names that explain their meaning or
units: byte_count, document_id, revision, timeout_ms. Preserve established public names
and ABI layouts; do not rename APIs merely to make their spelling uniform.

Every source file begins with a comment containing product name, relative File path,
PURPOSE, AUTHOR AND ORGANISATION, and LICENCE. Retain authorship and the actual licence.
For original Umicom source the author is Sammy Hegab, Umicom Foundation, and the licence
is MIT. Do not replace third-party notices. Use the comment syntax of the file language.

## Contracts, ownership and error handling

- Document who owns each allocation, how long a borrowed pointer remains valid,
  whether NULL is accepted, and which thread may use or destroy the object.
- Read required declarations directly. An include inherited accidentally through
  another header is not a reliable dependency.
- Validate lengths before arithmetic or reads. Prefer count > capacity - offset
  after checking offset <= capacity; offset + count can overflow.
- Prepare results locally and publish output only after success where the contract
  promises unchanged output on failure. Preserve that promise for aliased inputs too.
- Distinguish invalid arguments, malformed content, stale state, cancellation,
  capacity limits, unsupported operations and missing records. Keep error precedence
  documented when multiple conditions are invalid.
- Check every fallible operation that establishes a later precondition. Deliberate
  best-effort work needs a comment explaining why failure does not corrupt state.
- Keep cleanup paths complete for every partially constructed resource. Releasing a
  UI reference is different from destroying its service owner.
- Do not claim success after a required sub-operation fails. If a mutation already
  committed before a presentation failure, report both facts; do not pretend rollback.

Superseded code and important comments remain available for review under the project's
preservation policy. Disable C code explicitly with #if 0 and explain the replacement
immediately above it. Use comments or another valid mechanism in other languages.
Preserving an old implementation must not leave it accidentally executable.

## Text, paths and platform boundaries

UTF-8 byte offsets, Unicode scalar positions, UTF-16 positions and visual columns are
different quantities. Name and document the representation at each boundary. Validate
an explicit source span before interpreting its character boundaries. Keep malformed
source errors distinct from a cursor that splits a valid character. Reject embedded
NUL where the input contract is text, and do not silently truncate a length-delimited
input with strlen.

Treat a selected text range as [start, start + length): the end is excluded. Include
surviving prefix/suffix checks and undo/redo in editing regressions.

Use Framework path comparison for filesystem identity; raw strcmp compares spelling.
Convert a native path into a protocol's separator format at the integration boundary.
Do not weaken the protocol parser to compensate for an incorrectly converted caller.
Path normalization is not authorization: preserve rooted traversal and symlink checks.

Never derive test working directories directly from arbitrary case names. Windows
reserves names such as NUL, CON, AUX, PRN, COM1 and LPT1, including names with extensions.
Use a fixed prefix such as case-nul. Keep fixture roots unique and retain failed evidence.
Pass paths and build definitions as distinct checked arguments. A CMake definition is
-DNAME=VALUE; a bare NAME=VALUE is not interchangeable with it.

## GTK and asynchronous work

A GTK setter can synchronously emit a signal. That callback can close a window, edit a
different document, or replace an owner. Capture owned data and retain the required
objects before notifying observers. After notification, reacquire a live owner through
the existing lifetime mechanism and check identity and revision again. A non-NULL raw
pointer does not prove that the logical owner is still active.

Run widget operations on their owning UI context. Workers receive immutable inputs,
explicit cancellation and bounded work. Completion must handle cancellation, disposal,
owner replacement, and a successful mutation followed by failed UI publication.

Use the public API of composite controls. A returned GtkWidget may be a container,
not its visible button or entry. Tests must open a collapsed section before interacting
with its controls. Do not alter product defaults solely to make a widget-tree search pass.
A missing display or privilege is skipped coverage, never a successful behavioural test.

## Revisions and persistence

A revision identifies a meaningful published state. A rescan that finds no change may
preserve it. Tests that require a changed index must actually change the indexed data.
Test both no-op and changed observations. Also cover ABA transitions: a value can change
away and back while an earlier review is still stale.

Keep database transaction boundaries, expected revisions and commit status explicit.
A recorded actor label is not authentication. Use checked persistence and recovery
services; do not write private application data beside source code or rely on the process
working directory to find it. Financial values need the domain's explicit decimal,
scale, currency and rounding contracts rather than unqualified binary floating point.

## Build and regression coverage

1. Add the smallest test that reproduces the real failure. Do not weaken an assertion
   until the implementation's documented contract has been checked.
2. For a fixture defect, keep a regression for the original intended behaviour and
   separately exercise the invalid input or no-op transition that caused confusion.
3. Include boundary, cancellation, ownership and failure-output cases when the change
   affects those contracts. Avoid creating many identical tests just to increase counts.
4. Register executable sources, helper-header qualification inputs, test ownership and
   required libraries in the existing CMake modules. Keep build target names short enough
   for deeply nested Windows paths. Include optional-module and installed-SDK coverage
   when changing packaging or dependency composition.
5. Build the affected targets, then run the corresponding CTest cases with failure output.
   An incomplete build can leave old executables: a subsequent test run does not prove
   that the current source was exercised.
6. Record source identity, configuration, command, exit status and log location. Broaden
   qualification after focused checks pass, according to the integration change.

A Windows owner workflow can capture output with Tee-Object and CTest --output-log.
Ensure the log directory exists before starting. Inspect the native exit status after
each command and stop after a failed configure or build. PowerShell does not reliably
stop a sequence merely because a native program returned an error.

Static review, formatting checks, compilation, executed tests, installation and GUI
use are different evidence. Report each separately. Tests marked Not Run, timed out or
skipped remain outstanding until their underlying cause or required environment is
accounted for. A clean log without a captured exit status is limited evidence.

## Before requesting review

- Explain the concrete behaviour changed and why it belongs in this component.
- Check headers, API declarations, ownership and failure paths; inspect the actual diff.
- State what was executed and what remains unexecuted. Do not label authored regression
  coverage as a test pass.
- Confirm that public files contain only relevant source and contributor documentation.
- Preserve historical evidence and unresolved issues. Track repeated test names separately
  from proven recurrence of the same cause; fixes still need regression evidence.

These practices reduce recurring mistakes. They do not guarantee that future code is
free of defects; review and executed qualification remain necessary.
