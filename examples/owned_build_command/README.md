# Inspect an owned build command

This C23 client uses an installed Umicom Framework SDK to parse a literal command,
copy its owned arguments and refuse an incomplete replacement. It prints evidence;
it does not execute the displayed CMake command.

The Windows executable receives the canonical Framework ICO and SVG resources
through the shared application-branding helper. No additional font files are
downloaded or bundled.

1. Install a Framework SDK containing the current build component, SDK environment
   metadata and application-branding helper.
2. From this example directory, configure against that exact prefix:
   ~~~powershell
   cmake -S . -B build -G Ninja -DUMICOM_EXPECTED_SDK_PREFIX=C:/sdk/umicom -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   ctest --test-dir build --output-on-failure
   .\build\owned-build-command.exe inspect
   .\build\owned-build-command.exe copy
   .\build\owned-build-command.exe refusal
   ~~~
3. On a Unix-like host use the corresponding absolute SDK prefix and
   ./build/owned-build-command. Follow the SDK's runtime-library setup if its
   configured dependencies require it.

The inspect mode lists four arguments after the program name; build folder remains
one argument. The copy mode erases the original record before reading the copy.
The refusal mode reports an incomplete quote and retains the earlier command.
Each mode finishes by stating that no child process was started.

A missing copy symbol indicates an older library or mismatched headers. Reinstall
a consistent SDK instead of adding a source-tree fallback. A missing package
component or metadata helper is a configure failure, not an invitation to link an
unrelated checkout. Stop on errors and retain the command output.

The three consumer cases are authored and unexecuted. Passing them would establish
only these host-side record operations, not a real project build, package install
or native application launch. See the [owned command lesson](../../docs/learning/owned-build-commands.html)
for the ownership model and its limits.
