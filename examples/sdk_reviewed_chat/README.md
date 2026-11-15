![Umicom](../../resources/brand/umicom-logo.svg)

# Review and save a streamed local-model reply

This C23 console application uses the installed Umicom Framework SDK. It stores a proposed request in SQLite, shows the model, loopback endpoint and prompt, and asks you to type `RUN` before generation. Framework owns request review, persistence, HTTP, decoding, cancellation, worker lifetime and final response validation.

1. Install a compatible Framework SDK with `ai_workspace`, `web`, `platform`, SQLite, libcurl and json-c support. Use the same supported compiler/runtime family as the SDK.
2. Copy this directory into your own project. Select the absolute installed SDK prefix; a Framework checkout is not an installed package.
3. Start your trusted local model server separately. Determine its actual model ID and port. The example neither downloads models nor starts a server.
4. Configure and build the example. Stop on errors.
5. Use a new job ID for each proposed request. Review the printed details and type `RUN` only if they are correct.

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUMICOM_SDK_ROOT="C:\SDKs\UmicomFramework"
cmake --build build
& ".\build\umicom-sdk-reviewed-chat.exe" --allow-network ".\chat.sqlite" "lesson.one" 8080 "your-actual-model-id" "Explain ownership of a C allocation."
```

Replace the SDK prefix, model and port with actual values. `--allow-network` admits the loopback transport; typing `RUN` approves the specific stored request. These are local workflow decisions, not authenticated reviewer identities. The database stores prompts and successful answers as local application records; this example does not encrypt them.

The request begins only after RUNNING has been written. Complete UTF-8 text is printed while the producer runs. After joining, Framework validates the final response and reports two statuses: `Model result` and `Save result`. Exit 0 requires success from both and from cleanup. Model wording varies; printed partial text is never proof that the answer was accepted or saved. Ctrl+C requests cancellation and normally exits 130 after cleanup.

Saved responses currently hold at most 4095 UTF-8 bytes. Overflow leaves partial text observable but fails generation; increasing the wire limit does not increase that persistence limit. Requests have a 60-second exchange deadline. The supported wire route is text-only `POST /v1/chat/completions` on `127.0.0.1`. Reported model names do not authenticate a process or weights.

If a final save fails, do not automatically repeat generation. Reopening the workspace converts a surviving RUNNING record to interrupted. Inspect the earlier job through the workspace API and prepare a new reviewed request only after deciding to retry. Reusing a job ID is refused instead of overwriting its history. If native joining or checked release fails, the example retains dependent storage until process exit.

For UNAVAILABLE, check the SDK's SQLite and local HTTP capabilities and the HTTP library's thread-safe initialization support. For protocol or citation failures, inspect the reported status and server behavior; do not convert the printed draft into a successful stored answer. The shared branding helper stages approved SVG/ICO resources and embeds the Windows icon, which still needs visual qualification.

Read [reviewed local streams](../../docs/learning/reviewed-local-streams.html) for the lifecycle and focused regression commands. This example is authored; installed-consumer configuration, compilation, local inference, icon appearance and execution remain unverified.
