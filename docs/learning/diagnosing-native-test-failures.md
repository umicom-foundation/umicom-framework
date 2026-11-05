# Diagnosing native interface test failures

This guide assumes a configured Windows UCRT64 Applications build tree with testing enabled. It explains how to investigate a failure without hiding it. Close running copies of the test program before rebuilding.

## What a native test checks

A native interface test creates the same GTK widgets used by an application. A widget is a visible control, such as a button, or a container that arranges other controls. The test drives a small workflow and checks its result. A failed test is useful evidence, but the failure may belong to application code, its test setup, or the installed runtime libraries. Keep these possibilities separate until the evidence identifies the cause.

Start with the test name, its first diagnostic, and the source and runtime versions used for that run. A screenshot of an error box does not identify the source revision of the executable. A skipped test is missing coverage, not a successful test.

## 1. Keep a separate log for each attempt

A log records what happened during one attempt. Use a new timestamped folder so another attempt does not overwrite it. Preserve configure, build and test output separately. Stop when configuration or compilation fails: an older executable can still exist and produce misleading test results.

CTest runs registered tests. The -R option selects names that match a regular expression. Anchors ^ and $ mean the start and end of a name, so an exact expression runs one case. The Framework Media example below tests layout without opening a broker connection.

Run these commands only after the build tree has been configured successfully:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$LogRoot = "C:\umicom\_local\logs\native-review-$(Get-Date -Format yyyyMMdd-HHmmss)"
New-Item -ItemType Directory -Force $LogRoot
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake --build --preset windows-ucrt64-all-debug --target umicom-video-workspace-gtk-test --parallel 2 2>&1 | Tee-Object "$LogRoot\media-build.log"
if ($LASTEXITCODE -ne 0) { throw "Build failed; inspect media-build.log before testing." }
ctest --preset windows-ucrt64-all-debug -R '^framework[.]video_sequence[.]umicom-video-workspace-gtk-test[.]geometry$' --output-on-failure --no-tests=error --output-log "$LogRoot\media-tests.log"
if ($LASTEXITCODE -ne 0) { throw "Test failed; retain media-tests.log for investigation." }
```

## 2. Read the earliest relevant diagnostic

A timeout means the test did not finish within its limit. It does not prove that the last function printed in the log is defective. An exception code identifies a termination category; it is not a source-level diagnosis. Read the warning or assertion before it, then compare the failing operation with nearby passing cases.

The Media workspace test installs tests/support/widget_diagnostics.h. On its first GTK warning or error on the main thread, this helper records widget addresses, types, parent addresses, allocations and size requests. Match the address in GTK’s diagnostic to the address in that table. The parent rows explain which container owns the widget. The helper does not record labels, document text or entry contents, and it never measures widgets or dispatches events from the logger.

This is diagnostic instrumentation, not a layout repair. The normal fatal-warning policy still applies. Initialising the GLib test environment lets the test participate in GLib’s noninteractive test reporting; do not remove warnings simply to get a green result.

## 3. Check the test setup

A fixture is the temporary environment prepared by a test. For an empty learning-library test, the fixture must actually be empty. Studio’s empty-library executable has its own output directory. A different executable deliberately receives a packaged guide and tests automatic discovery. This prevents a normal application build from silently changing an empty-library test.

A collapsed GtkExpander still owns its controls even when they are absent from the visible widget tree. Broker projection tests use the existing Framework logical lookup to find those controls. Finding a control is not proof that it is visible, enabled or usable: interaction tests must check those properties separately.

A GtkDropDown can automatically choose a row from a nonempty model. To test rejection of an absent selection, attach an empty model and assert GTK_INVALID_LIST_POSITION before invoking the command. Do not weaken the rejection assertion to accommodate an accidentally valid selection.

## 4. Review events and ownership

An event callback is code called in response to an action. It may close a panel before it returns. Holding a reference keeps an object alive, but does not prove it still belongs to the open window. Framework’s viewport focus handler holds the selected content during the callback and then checks its parent and root again.

The handler gives each panel one opportunity to accept F6 focus. Calling a scrolling wrapper instead can retry its child or focus the wrapper itself. The tests in tests/workbench_viewport/test_focus_lifetime.c exercise a callback that rejects focus, closes a window, changes a tab or invokes navigation again.

Waiting for asynchronous work also needs a limit. An unbounded loop that drains all pending events can stay busy forever if a source is always ready. tests/support/main_context.h provides a bounded dispatch slice. Check a deadline between slices. The outer CTest timeout remains necessary because a callback that blocks cannot be interrupted by a slice limit.

## 5. Verify the repair

Build the affected test executable and its dependencies, then run the failing case and related regression cases. Preserve the new output even if it still fails. Record the source revision, uncommitted changes and installed library versions with the result.

For a layout repair, verify narrow and wide allocations, resizing, focus and callbacks after a panel closes. A size request is a minimum, not a promise that the window manager will resize a mapped window. Component tests can vary the component’s allocation inside a stable window; native window resizing needs its own workflow test.

For an image-loading failure, preserve the GError domain, code and message. The branding test also prints the selected GdkPixbuf loader environment. Investigate those details before replacing the SVG or claiming the artwork is malformed.

Mark a repair as authored until it has been compiled and the affected workflow has run successfully. Keep an unresolved failure open when the available evidence cannot establish its cause.

## Source map and practice

Follow tests/video_sequence/test_workspace_gtk4.c into tests/support/widget_diagnostics.h to see how a GTK warning receives bounded diagnostic context. Follow adapters/gtk4/workbench_viewport_gtk4.c and its focus-lifetime test to compare object lifetime with logical ownership. Compare Studio’s UmicomStudioLearningLocations.cmake and UmicomStudioLearningPack.cmake for empty and packaged fixtures.

Practice with a saved test log: identify the test name, first warning, final outcome and missing evidence. Explain which observation would distinguish a test-setup problem from a product defect. Do not change a test’s expected result until you can explain the behaviour it is intended to protect.

