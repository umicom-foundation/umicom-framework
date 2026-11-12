# Editable storyboard document example

This C23 client uses the installed Umicom Framework creative_workspace component.
It creates a two-scene draft, round-trips it through the versioned document API
and demonstrates refusal of an unsupported version without replacing the draft.

The sample contains an unresolved @{harbour} tag. Saving or reopening it does not
load an image, upload a reference or authorize generation. Resolve it against a
reviewed library with UmiMediaStoryboardCreate in a separate workflow.

## Configure and build

Install the changed Framework headers, library and CMake metadata first. From
this example directory, run each command separately and stop on errors:

~~~powershell
cmake -S . -B build -G Ninja -DUMICOM_EXPECTED_SDK_PREFIX=C:/umicom/_local/install/framework
cmake --build build
ctest --test-dir build --output-on-failure --no-tests=error
~~~

Replace the prefix with your actual installed SDK. The project refuses a package
outside that prefix and verifies the installed layout. It does not fall back to
the Framework source tree. On Windows, use the same compiler/runtime environment
as the SDK and the documented DLL deployment/environment setup.

Three tests are registered: inspect, roundtrip and refusal. They perform no file
writes or provider calls. All commands are supplied for qualification; this
example is authored, not a recorded execution result.

The shared branding helper supplies the canonical Umicom SVG and Windows ICO.
It adds no new logo drawing or font download.

## Save and reopen a chosen file

The save command requires --replace because it can replace an existing document.
Choose a disposable learning destination, review its path, then run:

~~~powershell
.\build\storyboard-document.exe save C:/umicom/_local/learning/storyboard-example.json --replace
.\build\storyboard-document.exe open C:/umicom/_local/learning/storyboard-example.json
~~~

The shared atomic writer may create missing parent directories. Parent directories
must be trusted and stable. This is replacement at one explicit path, not a
concurrent-edit merge or a guarantee against another writer replacing it later.

A failed open preserves the caller's current draft. Keep a damaged file for
diagnosis; do not delete it just to make the example succeed. Inspect the returned
status and compare the format/version/scene fields. A new document version should
be handled by a deliberate migration, not silent field removal.

## Limits

Only scene title, story, requested duration and order are persisted. Assets,
unsaved widget text, prepared instructions, model selections, generation receipts,
undo history and upload permission are outside this document. A desktop editor
must capture its intended edit state and handle unsaved-change decisions before
calling these APIs.

The library performs cooperative cancellation while preparing a save. Once atomic
publication starts, its actual I/O result is returned even if cancellation arrives.
The example does not claim crash, native UI, provider or real-file qualification.
