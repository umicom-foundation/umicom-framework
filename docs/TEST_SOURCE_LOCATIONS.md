# Open a source location from test evidence

A test can print a useful file and line when an assertion fails. For example,
`tests/check.c:29: expected == actual` names line 29 in a source file. Framework
can recognise that location, keep the test and run that produced it, and open
it through the same document coordinator used by the editor.

## Capture the evidence first

1. Include `umicom/test_platform/source_links.h` and link `Umicom::test_platform`.
2. Call `UmiTestEvidenceCreate` with the result and output registries, the exact
   selected test ID, and either one session ID or `NULL` for all retained runs.
3. Pass that capture to `UmiTestSourceLinksCreate`. The new collection owns its
   values, so you may destroy the original evidence capture immediately.
4. Read `UmiTestSourceLinksGetSummary` and display the retention counts. At most
   32 results and 64 output records enter the evidence capture; source links
   have a separate limit of 256. A count describes retained evidence, not a
   promise that every test result has been kept.
5. Read each link with `UmiTestSourceLinksAt`. Its item, session and record IDs
   explain where the location came from. Destroy the collection when finished.

Results are considered newest first, with failure details before the shorter
message. Output follows in its captured order. Equal path, line and column in
the same session share one link, preserving the first record's explanation.
Another session keeps a separate link. Path spelling is compared exactly,
including case; this does not assert that two spellings name different files.

## Recognise supported messages

The standalone `UmiTestFailureParseText` function also works on one line. It
accepts these forms, with optional CTest prefixes such as `205: `:

```text
tests/check.c:29: expected == actual
C:/work/check.c:29:4: assertion failed
C:\work (debug)\check.c(29,4): assertion failed
tests/check.c:29:4: error: unknown name
```

Compiler and CMake messages reuse the compiler diagnostic parser. Supported
ANSI colour and erase sequences are removed before interpretation. Positive
line and column numbers are required; a missing column is recorded as zero.
Zero means the start of the line when opening a document. Columns use UTF-8
byte positions, not screen character widths.

The parser does not infer a file from `check failed at line 70`, join fragments
from different output records, or follow web addresses. Drive-relative paths
such as `C:check.c` are rejected because their meaning depends on a process's
per-drive current directory. Malformed and unsupported lines remain available
in the original evidence. An overlong destination or message is rejected in
full; it is never shortened into a link to another file. The summary reports
unparsed lines, duplicates and omissions caused by capacity limits.

## Open only after the user chooses a location

1. Include `umicom/test_ui/source_navigation.h` and link `Umicom::test_ui`.
   This library has no GTK requirement.
2. Keep a copy of the selected `UmiTestSourceLink`. Before activating it, a UI
   should check that the selected test, evidence revisions and document owner
   still match the state that was shown to the user.
3. Show the source path and run. For a relative path, choose and display an
   absolute base directory. A historical result does not contain its original
   working directory, so do not silently assume it is the current directory.
4. Call `UmiTestSourceLinkOpen(documents, &link, baseDirectory, &offset)` on the
   document owner's thread. Absolute paths need no base. Relative paths without
   an absolute base return `UMI_STATUS_INVALID_ARGUMENT`.
5. Check the status. A failed file open leaves the previously active editor's
   cursor alone. If the file opens but its recorded line no longer exists, the
   source tab stays open and the function returns `UMI_STATUS_NOT_FOUND`.

The coordinator reuses an already open draft without replacing its unsaved
text. Navigation does not edit, save, reload, or execute a test. The location is
historical evidence: a later edit may move the assertion to another line.

For the location reported by test discovery, use `UmiTestItemOpenSource`. CTest
usually identifies the `add_test` statement in a CMake file. That is a different
location from a failed assertion in C source. A discovery line of zero opens
line one. Both APIs reject a location too large for the shared diagnostic
contract instead of silently cropping it.
