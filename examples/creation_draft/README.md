# Editable creation drafts

This C23 example links to an installed Umicom Framework SDK. It preserves a source excerpt, creative direction and an independently edited prompt. It does not contact a model or grant permission to generate media.

1. Install a Framework build containing the creation-draft document API. Use one SDK prefix for both headers and libraries.
2. From this example directory, configure and build with the commands below. Replace the prefix only if your SDK is installed elsewhere.
3. Run the four in-memory scenarios with CTest. They do not write document files.
4. For a real-file exercise, choose a disposable absolute path and explicitly approve replacement with the final flag.

~~~powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUMICOM_EXPECTED_SDK_PREFIX=C:/umicom/_local/install/framework
cmake --build build
ctest --test-dir build --output-on-failure --no-tests=error
.\build\creation-draft.exe inspect
.\build\creation-draft.exe save C:/umicom/_local/learning/creation-draft.json --replace
.\build\creation-draft.exe open C:/umicom/_local/learning/creation-draft.json
~~~

Stop when a command fails. The configuration checks that the discovered package belongs to the requested prefix and has the installed SDK layout. If it finds another SDK, inspect the package cache and correct the prefix before continuing.

The expected roundtrip output retains the user-edited harbour-bell sentence. The incomplete scenario accepts an unfinished recovery document while refusing preparation without title/source. The refusal scenario keeps the entire current document when the input version is unsupported.

Save can replace a file and create missing parents. Use a trusted folder, not a device path. Cancellation before publication prevents writing; after atomic publication begins, the actual write result takes precedence. Load refuses malformed, unknown or oversized documents without changing the destination.

The example uses Framework's application-branding helper for the canonical SVG resources and Windows ICO. It performs no GUI rendering. Source, build, installed-library compatibility and file behavior require qualification; the authored example is not evidence of execution.

Read [Editable creation drafts](../../docs/learning/editable-creation-drafts.html) for the schema and ownership walkthrough. This document does not include assets, provider settings, credentials, receipts or automatic recovery.
