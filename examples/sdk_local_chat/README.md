![Umicom](../../resources/brand/umicom-logo.svg)

# Stream from a local model with the installed SDK

This C23 console client sends a prompt to an already-running trusted local server, displays complete UTF-8 text as it arrives, and supports Ctrl+C cancellation. Framework owns the codec, loopback HTTP policy, response checks, terminal sequencing, worker and text storage. The example does not start a server, download weights, call a hosted endpoint or claim that the selected model belongs to Umicom.

1. Build and install a compatible Framework SDK containing `ai_workspace`, `web`, `platform` and the new local-stream APIs. Native libcurl/json-c support must be enabled.
2. Select a trusted server with the supported text-only streaming chat-completions route. Check its actual model identifier and port. A reported model name does not authenticate the process or loaded file.
3. Copy this directory into your own project and configure against the explicit installed SDK prefix. Use compatible compiler and dependency runtimes.
4. Build, then invoke `--allow-network` only when you intend to send the prompt to that local service. Stop on errors.

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUMICOM_SDK_ROOT="C:\SDKs\UmicomFramework"
cmake --build build
& ".\build\umicom-sdk-local-chat.exe" --allow-network 8080 "your-actual-model-id" "Explain ownership of a C allocation."
```

Replace the sample prefix, port and model ID with your actual values. There is no default model or automatic download. Package guards refuse source-tree dependencies; Framework's branding helper stages approved SVG/ICO resources and the Windows icon.

Expected behavior is generated text followed by exit code 0 only after valid protocol completion and successful HTTP transfer. Wording depends on the model. Ctrl+C requests cancellation, joins the worker and exits 130; printed text remains partial. A 60-second deadline bounds the exchange. Native thread teardown must still finish before release.

If initialization reports UNAVAILABLE, check that the SDK has HTTP/JSON support and that its native HTTP library reports thread-safe global initialization. If a stream is rejected, inspect the status instead of treating printed text as a complete answer. The adapter accepts one text choice, bounded output, matching reported model ID, ordered finish and [DONE], and an HTTP/1 event-stream response. Tools, remote credentials, redirects and compressed responses are outside this example.

The request is encoded and copied before its input allocation is released. The signal handler changes only a `sig_atomic_t` flag; the main thread owns cancellation and controller calls. Copied text comes from Framework's synchronized owner. No console or GUI pointer reaches the worker. A failed native join/release retains storage until process exit.

Read [background local streams](../../docs/learning/background-local-model-streams.html) for lifecycle and regression commands. This example is authored. Installed-SDK configuration, compilation, model inference, icon rendering and execution still require qualification.
