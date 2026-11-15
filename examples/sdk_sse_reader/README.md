![Umicom](../../resources/brand/umicom-logo.svg)

# Read event streams with the installed Web SDK

This C23 example feeds authored response headers and byte fragments through Framework's response admission and bounded SSE receiver. It reconstructs split UTF-8 and line endings, reads two events from one final fragment, and prints their independent message copies. It performs no network request or model inference.

1. Build and install Framework with the Web component and the SSE parser API.
2. Copy this example folder into your own project directory.
3. Select the absolute installation prefix of your compatible SDK.
4. Configure, build and run. Stop on the first error.

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUMICOM_SDK_ROOT="C:\SDKs\UmicomFramework"
cmake --build build
& ".\build\umicom-sdk-sse-reader.exe"
```

The prefix is an example; use your actual SDK location. CMake requires an installed package, checks the environment contract and links `Umicom::web`. Framework's branding helper stages the approved SVG/ICO resources and Windows icon. The example does not clone Framework or fall back to an adjacent source checkout.

Expected output:

```text
Authored event update, id 7: Native €
Authored event message, id 7: Second event
```

The second event keeps ID 7 while its name defaults to `message`. Your terminal's font and output encoding can affect the euro sign's appearance. The parser and copied message preserve its UTF-8 bytes.

Feed returns a consumed-byte count. Resume at that offset after taking a ready event; a network callback can contain several events, or only part of one. Take allocates an independent message. Release it with `UmiWebSseMessageFree`, including when downstream processing fails. Allocation failure retains the ready event for retry, but this small example reports the error and exits.

Use one externally serialized parser per response. Set separate line, payload, name, ID and total-input limits. The strict text profile rejects malformed UTF-8 and embedded NUL; it does not reproduce browser replacement decoding. Finish discards an unterminated event and never creates a final event by itself.

For a real HTTP adapter, additionally validate status and Content-Type, bound headers, apply endpoint policy and deadlines, and terminate the request on parser failure. Retry metadata does not authorize reconnection. A provider JSON delta still needs provider-specific validation before entering an AI response stream.

Read [receiving server-sent events](../../docs/learning/receive-server-sent-events.html). Installed-SDK compilation and execution remain separate qualification steps; Framework configuration alone does not prove this sample runs.

## Admit response metadata first

The example now supplies a complete informational header block followed by HTTP 200 and an explicit UTF-8 event-stream content type. `UmiWebSseResponseHeader` consumes complete transport header lines. `UmiWebSseResponseBody` admits each fragment's byte count before the SSE parser sees it. Successful transport completion must precede `UmiWebSseResponseFinish`; this fixture has no actual transport and finishes only after all authored input was supplied.

The response checker bounds header lines, total header bytes, informational blocks and body bytes. It rejects duplicate framing fields, a Content-Length combined with chunked encoding, unexpected media types and unsupported content encoding. It does not decode JSON, authenticate a local model or reconnect. An SSE parser finishing successfully also does not prove a model answer is complete; that requires the provider's separate terminal sequence.

If compilation reports missing `sse_response.h` or functions, the selected installed SDK predates response admission. Update and qualify the package, then repeat configuration. Do not add source-tree include paths to conceal an incompatible installed SDK. Read [response admission](../../docs/learning/admit-event-stream-responses.html) for the profile and failure cases.
