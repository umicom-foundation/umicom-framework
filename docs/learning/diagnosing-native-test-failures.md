# Diagnosing native interface test failures

![Umicom](../../resources/brand/umicom-logo.svg)

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


## Retained controls and keyboard navigation

A widget can still exist without belonging to the visible widget tree. For example,
GTK keeps an expander's body alive while the section is collapsed, but removes that
body from its internal box. An ancestor lookup from a field inside the closed body
can therefore return null. The field has not been deleted and its draft text should
still be present.

The Media split shortcut keeps a borrowed pointer to the section owner in
`UmiVideoSequencePanel.splitSection`. The workspace owns that section. The command
checks that the panel is open, reveals the inspector, expands the same section and
focuses the existing time field. It does not create a replacement form or submit an
edit. See `adapters/gtk4/video_sequence_presentation.inc` and
`tests/video_sequence/workspace_presentation.inc`.

Use this investigation sequence when a shortcut appears to do nothing:

1. Check whether the section is open and whether its body is mapped.
2. Compare `gtk_expander_get_child()` before and after collapse. It should identify
   the same retained body when the application deliberately preserves drafts.
3. Check the field's root and ancestry separately from its object lifetime.
4. Invoke the real shortcut and verify that the original field becomes mapped,
   receives focus and retains the draft value. Verify that viewing the section
   has not added an undo entry.
5. Repeat after hiding the inspector and after a callback retires the panel.

Framework's workstation focus cycle similarly snapshots only mapped, sensitive,
focusable targets in a live region. It holds references during callbacks and checks
root membership again before trying another target. Forward navigation tries the
first eligible widget; reverse navigation tries the last. It neither opens closed
sections nor selects hidden notebook pages. A callback that redirects focus or
retires the workspace must stop that navigation attempt.

The focused regression cases are `framework.ui_workstation.focus_cycle.collapsed-section`,
`collapsed-reverse` and `expanded-section` under the same prefix. Run the existing
reentry, detach, retire, notebook and pane cases too: a fix for collapsed sections
must preserve those lifetime and navigation contracts. These tests require a working
GTK display; a skipped run does not verify the behavior.

## Capture a rendered frame, not an assumed one

A mapped widget can need another frame after a focus or layout change.
`GtkWidgetPaintable` may draw nothing when its widget is not ready to draw. A null
render node at that moment is not sufficient evidence that the screenshot code or
renderer is permanently broken.

The Media fixture queues a draw and retries its snapshot within a two-second
deadline, dispatching bounded main-context slices between attempts. It still fails
if the root is unmapped, has no size, or produces no node by the deadline. GTK
warnings remain fatal. This avoids treating a fixed sleep as proof that rendering
finished and avoids accepting an empty image as screenshot evidence.

If the bounded wait expires, keep the mapped state and allocation diagnostic,
source identity, GTK version and renderer information. Do not turn the assertion
into a skip or claim success without an actual saved image.

GTK references: [expander implementation](https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gtk/gtkexpander.c)
and [WidgetPaintable documentation](https://docs.gtk.org/gtk4/class.WidgetPaintable.html).
The implementation link tracks upstream development; compare the version installed
in the test environment before attributing a runtime failure to that library.


## When a previously working CLI path disappears

A standalone Framework build and an Applications build have separate binary directories. Rebuilding one does not recreate the other's executable. A linker failure can leave the requested program absent even when thousands of other targets built successfully.

1. Check the exact path with PowerShell's Test-Path command.
2. Read that build tree's log for the failed executable target and the first linker diagnostic. A duplicate-symbol error can arise when two static libraries export the same private helper name. Give cross-file private helpers a component-specific linker prefix; keep public API names unchanged.
3. Rebuild the affected target after integrating its source repair. Do not interpret a file's presence as proof that it is current or qualified.
4. Run the explicit executable path with --help. For repo lock ., the dot selects the current repository directory, independently of the executable's directory. Locking records checked-out dependency revisions; it does not fetch, build or publish them.

## Native path spelling and dependency traversal

Path text and path identity are different. On Windows, normalization may turn src/main.c into src\main.c. Parent checks must recognise the component separator, while src and src2 remain different names. Read src/platform/rooted_files.c and tests/platform/test_relative_path_relation.c. The regression checks both operand orders: reversing a parent relationship must reverse its direction. Invalid input must leave the caller's output unchanged.

A dependency graph is a set of operations connected to prerequisites. An acyclic graph has no path back to its starting operation. Dense graphs can contain many paths to the same prerequisite. If a traversal marks a node only when removing it from its stack, pending duplicates can fill a bounded stack and falsely suggest a cycle. Mark a node when adding it to the stack instead, so each operation occupies at most one pending slot.

Read src/developer/pipeline.c and DenseDependencies in tests/test_developer_pipeline.c. The exercise admits 1024 edges, rejects real reverse cycles, checks that refusal preserves the dependency count and revision, and checks the capacity boundary separately. These assertions need compiler and runtime qualification; reading them is not a passing result.

## Reach the assertion you intend to test

A logging fixture that tests two writers targeting one file must first supply valid CTest metadata. Otherwise metadata validation correctly returns before the collision check. Keep a separate missing-metadata assertion, then supply a synthetic tool record and retain the no-process/no-file assertions. The injected executor must continue to prevent an actual tool launch.

A Windows worker test that creates production common controls also needs the production activation manifest. Merely linking comctl32 does not select version 6 of that runtime. Compare tests/boot_media/CMakeLists.txt with examples/boot_media/resources/media.manifest. Do not remove InitCommonControlsEx assertions or mark them skipped to hide an initialization failure. Microsoft describes the activation requirement in its [common-controls deployment guide](https://learn.microsoft.com/en-us/windows/win32/controls/cookbook-overview).


## Separate build failures from tests that never ran

Configuration writes build instructions. Compilation translates C source into object files. Linking combines those files and libraries into an executable. CTest starts that executable and interprets its exit status. A successful configuration therefore does not prove that the executable exists or that its tests pass.

1. Search the build log for `FAILED:`, `fatal error:`, `undefined reference` and `error:`. Read the complete diagnostic around each failed target. Independent targets can continue building after an error.
2. Compare the missing executable named by CTest with its target and dependencies. One failed shared library can prevent many test programs from linking. Several test names can also use the same executable with different arguments.
3. Keep outcomes separate: **Not Run** means CTest could not execute the check; **Failed** means a check ran and reported failure; **Skipped** means the configured prerequisite was unavailable or the test explicitly declined execution. None is a pass.
4. Repair the build first. Run a focused incremental build and stop on a nonzero exit code before testing. Do not install or launch stale files left by an earlier successful build.
5. Run the affected regression cases after their targets build. Follow with the complete configured suite. A growing temporary log is a progress snapshot, not the final suite result.

An include path belongs to the target compiling the source. Studio’s About dialog reads its public version header, so the Studio product target needs its own public include directory even when another executable already has it. A test that directly creates GTK objects must include the GTK declarations.

A test can have a link dependency even when a particular case does not call that function. The CodeGuard ownership fixture includes the production service to inject allocation failures. That service also exposes asynchronous jobs. Its target compiles the real job source and links the platform task service, while retaining the instrumented result/service code. Linking a second complete CodeGuard implementation would obscure that ownership boundary.

## Update an API without weakening memory ownership

GTK deprecated `gdk_texture_new_for_pixbuf` in version 4.20. The shared `adapters/gtk4/pixbuf_texture_private.h` converts already-decoded RGB or RGBA pixels to GdkMemoryTexture. It does not change the approved SVG artwork or add another image decoder. See the [deprecated constructor](https://docs.gtk.org/gdk4/ctor.Texture.new_for_pixbuf.html) and [memory texture contract](https://docs.gtk.org/gdk4/ctor.MemoryTexture.new.html).

**Stride** is the number of bytes between the start of one pixel row and the next. A pixbuf can omit padding after its last row; a memory texture needs a complete buffer for its stride and height. The converter checks dimensions, channel count and multiplication limits, then copies only each row’s actual pixels into tightly packed storage. GBytes owns that storage, so releasing the original pixbuf does not leave the texture borrowing invalid memory.

The regression fixture checks RGB, RGBA, transparent pixels, padded rows, null input and release of the source buffer. No window is required by that fixture. About-dialog and Media display tests remain necessary to qualify native appearance and integration.

A compiler warning about a dangling pointer deserves separate investigation. The MCP fixture now keeps its callback action text inside its owning fixture instead of leaving a pointer to a function-local array. Formatting warnings should be corrected by making statement boundaries clear; suppressing the warning does not establish correct ownership or control flow.

## Check layouts without inventing an application failure

A small console tool may put its entry point in `main.c`; larger products may use `src/main.c` or `src/console/main.c`. The application surface audit accepts these layouts when the file exists and its CMake file mentions that source. A root filename must be a separate token: `tools/main.c` must not satisfy `main.c` accidentally. This is a textual layout check, not a compiler, a CMake interpreter or proof that the program works.

Practice: explain why an absent test executable, a test assertion and an unsupported source-layout assumption need different repairs. Inspect `tests/build_configuration/test_surface_audit.cmake` and identify the positive root-entry case and the three refusal cases. Keep the refusal checks when adding another supported layout.
