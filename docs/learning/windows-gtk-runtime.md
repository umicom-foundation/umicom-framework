# Checking the GTK runtime on Windows

GTK creates Umicom's native windows and text controls. The runtime is the compiled
library an application loads when it starts. A successful Umicom build does not
prove that the installed GTK runtime is free from defects.

## Understand the input-method warning

An input method handles typing, including language switching and composed
characters. The Windows implementation in GTK 4.24.0 and 4.24.1 can retain a
message callback after its input context is destroyed. A later Windows message
may then use an object that no longer exists.

The upstream GTK correction is
[30727f8a524dbaa69118a60d6bbef4b3447245b1](https://github.com/GNOME/gtk/commit/30727f8a524dbaa69118a60d6bbef4b3447245b1).
It records the display that owns the callback and removes that callback during
disposal as well as focus loss.

Framework reports this as an advisory during configuration. It does not disable
tests or stop configuration. A later Ninja error must be investigated separately.

## Check the package before replacing it

1. Close Umicom applications before changing their GTK runtime. Finish or stop
   builds first, so one build does not use a mixture of package versions.
2. In an MSYS2 UCRT64 terminal, inspect the installed package:

   ~~~sh
   pacman -Q mingw-w64-ucrt-x86_64-gtk4
   ~~~

   The output identifies the package and its version, such as 4.24.0-1.
3. Inspect the distributor's GTK recipe and release information. Confirm its source
   includes the correction above, or an equivalent reviewed backport. A newer
   package number alone is not evidence of this repair. Upgrading only from
   4.24.0 to 4.24.1 is insufficient.
4. When a corrected package is available, follow the distributor's supported
   full-system update procedure. See
   [Updating MSYS2](https://www.msys2.org/docs/updating/). Retain the package
   identity and update output with the qualification logs.
5. If no corrected package is available, request a maintained package containing
   the upstream fix. Keep the advisory open. Do not replace DLLs with unrelated
   downloads or change the diagnostic version check to declare the defect fixed.

A distributor may backport the fix without changing GTK's version. In that case
the conservative version-based advisory may still appear. Record the package's
source evidence; the warning alone cannot identify such a backport.

## Rebuild and qualify the actual runtime

1. Reconfigure and build the existing Umicom Applications build directory after
   installing a corrected runtime. This refreshes bundled runtime files as well
   as application sources. Preserve the build directory for incremental builds.
2. Run the focused native window checks after the build succeeds:

   ~~~powershell
   Set-Location "C:\umicom\Umicom-Applications"
   $env:Path = "C:\msys64\ucrt64\bin;$env:Path"
   ctest --preset windows-ucrt64-all-debug -R "^framework[.]window_input_lifetime[.]" --output-on-failure --no-tests=error
   ~~~

   A display-unavailable skip does not establish a pass. These tests exercise
   focused entry controls, direct window teardown and parent-driven teardown.
3. Exercise TMS independent windows and text-entry dialogs in Studio and Trader.
   Type into a field, switch input language, close the dialog, then interact with
   another window. Repeat opening and closing windows. Save any crash or GTK
   diagnostic with the package and source revisions.
4. Repeat the normal full test suite. A small lifecycle test cannot establish that
   every native window or input method is correct.

Framework's window input-lifetime helper releases focus before a native surface
is removed. It protects windows that use it but does not replace GTK's own disposal
repair. Do not disable input methods as a permanent workaround: users must retain
their normal text-entry support.
