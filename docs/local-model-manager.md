# Run local text models with Umicom

![Umicom](../resources/brand/umicom-logo.svg)

Umicom Local Models is a native C console application and Framework service for reviewing, downloading and running local GGUF text models. Use it directly as **umicom-models**, or through **umicom models**. Both commands call the same implementation. A separately installed, trusted **llama-server** performs inference. Python, Streamlit and Conda are not dependencies of this manager.

This version supports public Hugging Face GGUF downloads, CPU inference and the existing loopback chat provider. It does not install an inference engine, create an Umicom-trained model, generate images or videos, enable agent tools, or configure every product automatically. GPU offload, gated downloads, resumable transfers and a graphical model catalogue need separate implementations.

## Prepare the engine and storage

1. Use a trusted llama.cpp server build that supports the flags printed by the run preview, especially **--offline** and **--no-webui**. Review its licence and build identity. Keep the executable in its own trusted folder. Do not silently update a project's pinned third-party checkout.
2. Create a model folder you control. The following is an example; substitute your own absolute path. Keep downloads and model execution outside source repositories.
3. Inspect the catalogue and the compiled capabilities before downloading anything.

~~~powershell
New-Item -ItemType Directory -Path 'C:\Models\Umicom' -Force
umicom models --help
umicom models catalogue
~~~

The built-in entry **qwen2.5-0.5b-q4** identifies Qwen2.5 0.5B Instruct Q4_K_M at repository commit **9217f5db79a29953eb74d5343926648285ec7e67**, with **491400032 bytes** and SHA-256 **74a4da8c9fdbcd15bd1f6d01d621410d31c6fc00986f5eb687824e7b93d7a9db**. Its Apache-2.0 model licence is separate from Framework's MIT licence. These values come from the [publisher's model metadata](https://huggingface.co/api/models/Qwen/Qwen2.5-0.5B-Instruct-GGUF?blobs=true); read the [pinned model card and licence](https://huggingface.co/Qwen/Qwen2.5-0.5B-Instruct-GGUF/tree/9217f5db79a29953eb74d5343926648285ec7e67) before use. Disk size does not estimate total runtime memory: context storage, tensors and engine overhead require additional RAM.

## Review and download

Run the first command to print the exact URL, destination, expected size and digest. It performs no network request or file creation. After reviewing those values, the second command downloads the model.

~~~powershell
umicom models download qwen2.5-0.5b-q4 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf'
umicom models download qwen2.5-0.5b-q4 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf' --apply
~~~

The downloader uses HTTPS with certificate verification. Hugging Face can redirect large files to an HTTPS storage host; at most five redirects are followed. No credential file or proxy is discovered automatically. Network access is part of downloading even though subsequent inference uses a local server. A corporate proxy or private model repository is outside this first transport contract.

The destination must be new, under existing trusted directories. A failed or cancelled download can leave partial bytes at that path. The manager never overwrites or deletes that file. Inspect it and choose a different new destination, or remove the rejected file yourself after review. There is no installed-model database: file existence is not a verified installation. Success requires HTTP 200, the exact size, a basic GGUF header check and the expected SHA-256 of the stored bytes.

For another public model, obtain a complete commit identifier, the actual file's SHA-256 and its byte count from the publisher. A Git blob identifier is not the large model file's SHA-256. Use lowercase hexadecimal for both the full commit and SHA-256. The explicit form is:

~~~text
umicom models download-file OWNER/REPOSITORY COMMIT40 FILENAME.gguf SHA256 BYTES NEW_ABSOLUTE_GGUF [--apply]
~~~

Only ordinary repository paths and filenames are admitted. Branch names, tokens in URLs, traversal segments and non-GGUF artifacts are rejected. This version handles one GGUF file per plan; multi-file split models and separate projector files are not managed as an atomic collection.

## Inspect a local file

~~~powershell
umicom models inspect 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf' '74a4da8c9fdbcd15bd1f6d01d621410d31c6fc00986f5eb687824e7b93d7a9db'
~~~

Inspection streams through the entire file with a 64 KiB buffer. It checks the digest, magic bytes, a little-endian GGUF version 2 or 3 header, and basic count bounds. It does not parse every tensor, certify the model's behaviour, or prove that a particular engine can load it. The selected final file cannot be a symbolic link, reparse point, directory or device; parent directories must remain trusted and stable.

## Start and stop a CPU server

The example engine path below is a placeholder for your reviewed installation. The last three values are port, context capacity and CPU threads. Start with a modest context and thread count for a small model; adjust only after measuring your machine.

~~~powershell
umicom models run qwen2.5-0.5b-q4 'C:\Tools\llama.cpp\llama-server.exe' 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf' 8080 2048 4
umicom models run qwen2.5-0.5b-q4 'C:\Tools\llama.cpp\llama-server.exe' 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf' 8080 2048 4 --apply
~~~

The preview lists separate arguments; it does not inspect files or launch a process. Apply rehashes the model, acquires a cooperative guard file and submits the child process. The fixed plan chooses **127.0.0.1**, zero GPU layers, one inference slot, a stable model alias, offline mode and no engine web UI. See the [llama.cpp server interface](https://github.com/ggml-org/llama.cpp/blob/master/tools/server/README.md) for engine flag semantics. An unsupported flag must cause a visible engine failure; the manager does not silently weaken the plan.

Keep this console open. Type **status** and press Enter to read the process state and bounded diagnostic output. **Running means a process is running; it does not mean model loading or HTTP readiness succeeded.** A busy port or insufficient memory may cause the engine to exit. Type **stop** to stop and join the owned process tree. End-of-input also stops the session. Avoid killing the manager abruptly: normal cleanup is the supported portable shutdown path, and POSIX descendants can escape process groups deliberately. This is supervision, not a security sandbox.

The guard file ends in **.umicom-run.lock** and remains after release. Its presence is not evidence of a running server. Do not delete or replace a guard while another manager uses it. All cooperating launchers must use the same trusted model path; path aliases are not a global resource scheduler.

Custom inspected local files can use:

~~~text
umicom models serve ABSOLUTE_LLAMA_SERVER ABSOLUTE_GGUF SHA256 ALIAS PORT CONTEXT THREADS [--apply]
~~~

Limits are ports 1024–65535, contexts 512–32768 and threads 1–256. The model and engine impose their own tighter limits. Known inherited llama agent, tunnel, routing and alternate-model settings are refused. Ordinary process environment is still inherited; select a reviewed engine environment. The server is not isolated from other local users or processes.

## Describe runtime settings without starting a model

The descriptor preview converts the same validated CPU run plan into Framework AI catalogue metadata. It does not inspect the file, verify its hash, register a provider, write a catalogue, probe an endpoint or start a process.

~~~powershell
umicom models describe qwen2.5-0.5b-q4 'C:\Tools\llama.cpp\llama-server.exe' 'C:\Models\Umicom\qwen2.5-0.5b-q4.gguf' 8080 2048 4
~~~

The output names the canonical local-chat provider, model alias and loopback chat endpoint. It reports configured metadata with unavailable health and an unassigned revision. Its planned output limit is 256 tokens. The SHA-256 shown is the expected digest from the plan, not a new file observation. For a custom source, use:

~~~text
umicom models describe-file ABSOLUTE_LLAMA_SERVER ABSOLUTE_GGUF SHA256 ALIAS PORT CONTEXT THREADS
~~~

Neither description command accepts **--apply**. A malformed plan is refused before output is published. Download, run/serve and chat also stop before their side effects if the console cannot deliver the review text; a later output error is reported after normal owned-session cleanup.

Applications can call **UmiModelRuntimeDescriptorCreate** from **umicom/local_models/runtime.h** and choose their own runtime identifier, label and output-token limit. Insert the descriptor with **UmiAiRuntimeCatalogueUpsertIfRevision**, using zero only for a new record. The catalogue assigns revisions; use the observed revision when replacing an existing record. Configuration alone remains unavailable to automatic healthy-runtime selection.

A successfully submitted session stores its own copy of the run plan. **UmiModelSessionReadPlan** returns a value copy even if the caller later edits its next-run form. **UmiModelSessionRuntimeDescribe** uses that captured plan, but does not read process status or promote health. Keep the session alive and serialize access. A retained cleanup owner from a failed submission cannot be described as a submitted session.

All these records use the canonical **umicom.local-chat** provider identifier. A descriptor does not register another endpoint or change an existing provider's port. Application code must bind requests to the intended provider instance and retained session; multiple catalogue records alone are not a multi-endpoint router. Model identity, readiness, resource admission and warm replacement remain separate responsibilities.

Read the [native runtime description lesson](learning/native-model-runtime-descriptions.html) and [revision checks lesson](learning/runtime-catalogue-revisions.html). The installed SDK consumer under **examples/local_models/sdk_consumer** exercises projection and conditional catalogue symbols without file inspection, network or model execution.

~~~powershell
cmake --build --preset windows-ucrt64-debug --target umicom-model-runtime-test umicom-model-cli-review-test umicom-model-session-metadata-test umicom-model-session-test
ctest --test-dir build/windows-ucrt64-debug -R '^framework[.]local_models[.](runtime|cli_review|session_metadata|session)[.]' --output-on-failure --no-tests=error
~~~

The new pure cases cover 26 inputs and output-failure boundaries. The retained session tests exercise the finite native process fixture separately. These are qualification instructions; a configured target or authored test does not establish a pass.

## Send a reviewed chat request

Open a second terminal after the engine has loaded. The first command previews the request. The second sends it to the selected loopback endpoint.

~~~powershell
umicom models chat 8080 qwen2.5-0.5b-q4 'Explain a C pointer in two sentences.'
umicom models chat 8080 qwen2.5-0.5b-q4 'Explain a C pointer in two sentences.' --send-reviewed-prompt
~~~

The client uses Framework's existing validated JSON provider and AI runtime. It requests at most 256 output tokens, disables tools, has a 60-second timeout and does not fall back to a cloud provider. It sends one user message; it does not persist a conversation. A successful response is evidence for that engine, model, prompt and machine, not a general quality or safety guarantee. The endpoint is ordinary loopback without authentication: use only a server you trust.

## Observe endpoint health separately

The native health command makes a single request to the selected loopback port. It previews by default. Start a trusted server through the existing run/serve workflow, keep its owning console open, then use another console:

~~~powershell
.\build\windows-ucrt64-debug\bin\umicom.exe models health 8080 3000
.\build\windows-ucrt64-debug\bin\umicom.exe models health 8080 3000 --probe
~~~

The second command performs GET /health with a 3000 ms total timeout and a response body smaller than 4096 bytes. It sends no prompt or credentials and follows no redirects or proxies. It does not retry, poll, start or stop an engine. The standalone umicom-models executable accepts the same health arguments.

A valid HTTP 200 response with status ok prints endpoint reports ready. The documented HTTP 503 error prints endpoint reports unavailable. These are statements supplied by whichever process answers that port; they do not prove that it is the manager-owned child, that it loaded the selected model, or that a later inference request will succeed. Keep the process snapshot, file digest, health observation and reviewed chat result separate.

Exit zero means preview succeeded or the endpoint reported ready. Exit one means an unavailable report, transport/parse failure, cancellation or output failure; the diagnostic text distinguishes them. Exit two is invalid command syntax or arguments. Do not use exit zero alone to authorize sending private prompts.

~~~c
UmiAiLocalHealth observation = {0};
UmiStatus status = UmiModelHealthProbe(
    8080U, 3000U, user_approved_probe, cancellation, &observation);
if (status == UMI_STATUS_OK) {
    /* Display the endpoint report and its HTTP status separately from
     * the retained session snapshot. This observation has no identity proof. */
}
~~~

Call the blocking API on a worker in a GUI. Keep the cancellation token alive until that worker has joined. On any refusal or transport/parse/cancellation failure the previous output object remains unchanged; clients must check the returned status instead of presenting an old observation as new. The request does not mutate session lifecycle state.

## Qualify the health boundary

After the existing Framework configuration with UMICOM_AI_WORKSPACE_LOCAL_HTTP=ON, build the added tests and run both health checks and the retained chat transport checks:

~~~powershell
cmake --build --preset windows-ucrt64-debug --target umicom umicom-models umicom-model-health-test umicom-ai-health-codec-test umicom-ai-workspace-http-client umicom-ai-workspace-codec-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.](local_models[.]health|ai_workspace[.](health_codec|http|codec))[.]" --output-on-failure --no-tests=error
~~~

The added coverage consists of 22 offline codec groups and 13 native model-health groups. The latter include an inert C loopback fixture plus consent, bounds, pre-cancellation and preview cases that make no request. Keep skipped optional HTTP coverage separate from passes. Configure another independent build with local HTTP OFF and repeat the no-network cases; missing transport must stay unavailable. These commands are qualification instructions, not a record of executed results.

If an HTML error page or unsupported JSON arrives, investigate the port and engine version. Do not accept an arbitrary HTTP 200 as ready. A 503 response is an observation, not a reason to discard or replace the owned session. Retry only after an explicit caller decision. The [llama.cpp server documentation](https://github.com/ggml-org/llama.cpp/blob/master/tools/server/README.md#api-endpoints) describes the external endpoint.

## Connect Umicom applications

The shared GTK AI workspace already uses **UMICOM_AI_WORKSPACE_PORT**, defaulting to 8080. Set the environment before launching a product that includes that workspace:

~~~powershell
$env:UMICOM_AI_WORKSPACE_PORT = '8080'
~~~

Open its chosen AI library, select the local provider and enter the exact model alias printed by the manager. Review the request before execution. The extractive preview option is a separate non-neural provider. A configured endpoint is not a live capability test. Existing workspace requests have their own limits and a 30-second timeout, so a slow CPU model may need further timeout configuration work even when the console's longer request succeeds.

Studio, Trader, Media, LLM, RAG and other AI clients can link **Umicom::local_models**. The common lifecycle is:

1. Create and show a download or run plan without side effects.
2. On an explicit user action, call the blocking download/start operation on a worker.
3. Retain the session owner and poll **UmiModelSessionRead**. Do not label submission as ready.
4. Create a provider with **UmiModelLocalProviderCreate** and register it in the application's existing AI runtime. Registration transfers ownership only on success.
5. Stop and join active requests, destroy their provider/runtime, then call **UmiModelSessionStop**. Keep a non-null owner for retry if stopping fails.

A library being linkable does not add download buttons to every product. This delivery provides the shared service and console application; dedicated product model-management screens and per-product GUI qualification remain separate work. RAG retrieval can supply reviewed text to this chat provider, but this manager does not claim an embeddings endpoint. Media can use the text response to draft prompts; image/video rendering requires its own provider and watermark workflow.

## Retain console cleanup state in an embedding application

A command can return an error while a model session still needs cleanup. Use **UmiModelsCommandWithSession** when calling the console workflow from a long-lived host. Keep its owner slot alive beyond the command call, initialize it to null, and serialize calls with the host's other console operations.

~~~c
static UmiModelSession *console_session;

int RunModelCommand(int argc, char **argv)
{
    return UmiModelsCommandWithSession(argc, argv, &console_session);
}

UmiStatus RetryModelConsoleShutdown(void)
{
    return UmiModelSessionStop(&console_session);
}
~~~

If **console_session** is non-null after the command returns, retain it and report incomplete cleanup. A new command refuses the occupied slot before parsing or executing anything. An explicit call to **UmiModelSessionStop** retries cleanup; it must not be replaced with a free, reset or pointer overwrite. Some cleanup errors consume the owner, so inspect both the error and the slot rather than assuming every error leaves something retryable.

The historical **UmiModelsCommand** entry keeps the same arguments and exit-code convention. It retains its own process-local cleanup slot. **UmiModelsCommandHasRetainedSession** queries that slot; **UmiModelsCommandRelease** explicitly retries its cleanup. These functions do not touch a session returned through an explicit caller slot. A non-null compatibility owner blocks subsequent legacy commands. If release reports an error but consumes that owner, the query becomes false; still report the error.

There is no automatic retry after command return. Process exit is not proof of completed process-tree cleanup, and a new shell invocation cannot recover the old process's in-memory owner. A long-lived host should retain the owner and expose a deliberate retry. Normal successful console commands finish with an empty slot.

~~~powershell
cmake --build --preset windows-ucrt64-debug --target umicom-model-console-owner-test umicom-model-cli-review-test
ctest --test-dir build/windows-ucrt64-debug -R '^framework[.]local_models[.](console_owner|cli_review)[.]' --output-on-failure --no-tests=error
~~~

The fourteen console-owner cases use inert process/input boundaries. They exercise retained and consumed start/stop errors, occupied-owner refusal, late output errors, compatibility cleanup and successful completion. The earlier review-output cases remain registered separately. None starts a model or proves real process-tree teardown; run the native session fixture and deployment qualification separately.

## Build and qualify from source

Use an existing supported Framework toolchain. The following Windows example builds the native manager, unified CLI and regression executables. Stop when a command fails. These are instructions for the operator to execute, not a record of passing results.

~~~powershell
Set-Location 'C:\umicom\Umicom-Applications\framework'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake --preset windows-ucrt64-debug -DUMICOM_MODEL_DOWNLOAD_HTTP=ON -DUMICOM_AI_WORKSPACE_LOCAL_HTTP=ON
cmake --build --preset windows-ucrt64-debug --target umicom-source-contracts umicom umicom-models umicom-model-plan-test umicom-model-inspect-test umicom-model-stream-test umicom-model-session-test
ctest --test-dir build/windows-ucrt64-debug -R '^framework\.local_models\.' --output-on-failure
.\build\windows-ucrt64-debug\bin\umicom-source-contracts.exe headers --root include/umicom/local_models
.\build\windows-ucrt64-debug\bin\umicom-source-contracts.exe comments --root src/local_models --root tests/local_models --root examples/local_models
.\build\windows-ucrt64-debug\bin\umicom.exe branding audit --assets resources/brand --root docs/learning
.\build\windows-ucrt64-debug\bin\umicom-models.exe --help
~~~

**ON** requires pkg-config and libcurl 7.85 or newer; local chat also requires json-c 0.15 or newer. **AUTO** keeps missing transport unavailable, and **OFF** deliberately disables it. Model inspection and the server lifecycle still compile without download HTTP. No fabricated responses replace a missing client. Read the help capability line to see what your build contains.

The regression suite uses synthetic headers and a finite process fixture, with no real model downloads or inference. FIFO/link cases can be skipped on platforms lacking the required filesystem capability; retain the reason. Also qualify the HTTP-OFF build, installed SDK consumer linkage, the real publisher download, an existing-file refusal, bad digest, busy port, engine load failure, a reviewed chat response and normal stop. Record the source revisions, engine identity, model digest and actual outcomes separately.

## Check an installed SDK consumer

Install to a separate reviewed prefix, then build the small consumer from the installed library exports. The consumer reads the catalogue and reports compiled capabilities; it does not download or run a model. Use the same architecture and toolchain as the producer.

~~~powershell
cmake --build --preset windows-ucrt64-debug --target umicom-sdk-libraries
cmake --install build/windows-ucrt64-debug --prefix 'C:\umicom\_local\sdk\local-model-review' --component FrameworkSDK
cmake -S examples/local_models/sdk_consumer -B 'C:\umicom\_local\build\model-sdk-consumer' -G Ninja -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_PREFIX_PATH='C:/umicom/_local/sdk/local-model-review;C:/msys64/ucrt64'
cmake --build 'C:\umicom\_local\build\model-sdk-consumer'
ctest --test-dir 'C:\umicom\_local\build\model-sdk-consumer' --output-on-failure
~~~

Do not use a source-tree include directory to conceal a missing installed header or dependency. A successful source build alone does not prove that consumers can import **Umicom::local_models**.


### Select the installed package explicitly

The Framework and FrameworkSDK installation components now include the complete model consumer under **share/umicom/examples/local_models/sdk_consumer**. After the build and installation above, configure that installed source in a new consumer build directory:

~~~powershell
cmake -S 'C:/umicom/_local/sdk/local-model-review/share/umicom/examples/local_models/sdk_consumer' -B 'C:/umicom/_local/build/model-sdk-selected' -G Ninja -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DUMICOM_SDK_ROOT:PATH='C:/umicom/_local/sdk/local-model-review' -DCMAKE_PREFIX_PATH:PATH=C:/msys64/ucrt64
cmake --build 'C:/umicom/_local/build/model-sdk-selected'
ctest --test-dir 'C:/umicom/_local/build/model-sdk-selected' --output-on-failure --no-tests=error
~~~

**UMICOM_SDK_ROOT** selects the Framework installation. **CMAKE_PREFIX_PATH** can still locate its external dependencies. Configuration checks the selected package prefix, imported Framework targets and their library/header paths, the complete public-header fingerprint, required ABI 2, target system, pointer width and C compiler family. It reports compiler versions; matching metadata does not certify binary compatibility. The archive, link, dependent DLL search and runtime checks still matter.

The earlier source-example command remains an alternative. When UMICOM_SDK_ROOT is empty, ordinary CMake discovery is preserved and the guide's prefix list still works. The consumer clearly reports the auto-discovered prefix and checks that installation's layout, headers and environment. It does not claim that discovery chose a separately requested root.

If configuration reports a different selected prefix, inspect **UmicomFramework_DIR** in the consumer's CMakeCache.txt. That cached package directory can survive a change to UMICOM_SDK_ROOT. Choose a fresh consumer build directory for a different SDK; preserve the earlier directory and evidence. Do not add source-tree headers or bypass the guard. Missing helpers, components or identity metadata require a matching complete SDK installation. A header fingerprint is a consistency check, not a publisher signature.

### Exercise exported generation and callback ownership

The installed example now includes **provider_ownership.c** and its local header. It registers a small C callback that writes a fixed response. This is a test double: no weights are loaded, no endpoint is contacted, and no inference engine is launched. The example still links only **Umicom::local_models** and obtains its AI dependencies through that installed target.

1. Build and install the SDK as shown above, including the updated example. Configure from the installed **share/umicom/examples/local_models/sdk_consumer** directory. This catches a missing installed source file as well as a missing public header.
2. Build the consumer. Calls to **UmiAiRequestValidate**, **UmiAiResponseValidate**, **UmiAiRuntimeGenerateChecked**, the idle checks and checked service destruction must resolve from installed libraries. A configuration pass alone cannot prove this.
3. Run **local_models.sdk_catalogue** with the existing CTest command. Its earlier catalogue, context, conversation, coding and console exercises remain in place. The additional ownership exercise runs as part of that same test.
4. Read **SdkExercise** in the installed source. An over-capacity message count must refuse before the provider runs. A valid reply is published; a provider error or an unterminated successful reply must preserve the previously displayed response.
5. Compare the return status with **UmiAiGenerationReport**. A provider error is different from a malformed successful reply: the latter returns **INTERNAL_ERROR**, with **response_status** set to **INVALID_ARGUMENT**. Check **dispatched**, **response_checked** and **response_published** before interpreting each stage.
6. Follow **SdkHealth**. Both health APIs keep the service and runtime alive during the callback, so attempted checked destruction returns **BUSY**. After each API returns normally, the example can continue. Health deliberately stays unavailable; an ownership check does not establish model readiness.
7. Inspect final cleanup. The service releases before its borrowed runtime, and the registered provider's destroy callback must run exactly once. If checked cleanup refuses, keep the owner storage alive and report the error instead of freeing a still-borrowed pointer.

The generation API stages the provider's reply in Framework storage before publishing it:

~~~c
UmiAiGenerationReport report;
UmiStatus status = UmiAiRuntimeGenerateChecked(
    &runtime, provider_id, approved, &request, &response, &report);
/* On failure response still holds its earlier value. Do not display it as
 * this attempt's result. Use status and report to describe the failure. */
~~~

These are synchronous ownership contracts. Serialize access and let callbacks return normally; an idle check is not a thread lock or a lease. The example does not validate arbitrary tool callbacks, real transport, model identity, output quality or graphical behavior.

A successful executed CTest result means these installed-library checks passed in that environment. Preserve the producer revision, selected SDK prefix, compiler and logs. A missing symbol requires a matching library/header installation, not a source include-path workaround. A diagnostic beginning **Installed SDK ownership check failed** identifies the contract to investigate. These instructions are a qualification procedure, not a claim that the consumer has already compiled or passed.

### Check package-selection regressions

~~~powershell
ctest --test-dir build/windows-ucrt64-debug -R '^framework[.]local_models[.]sdk_selection[.]' --output-on-failure --no-tests=error
~~~

Seventeen owner-run configuration cases exercise the real example with explicit package doubles: selected and discovered roots, spaces, stale cached packages, a root change, invalid paths, missing component/guard/archive, wrong prefix, changed/extra headers, an escaping library and incompatible ABI/compiler/missing metadata. Refusals repeat in the same build directory; root switching first demonstrates a successful selection. Each invocation retains its configure logs. The fixture archives are text placeholders and the branding hook is inert: these cases do not compile or run the consumer or qualify installed artwork. Complete the real SDK build, consumer test and deployment checks separately.

For the underlying mechanisms, read CMake's [package discovery](https://cmake.org/cmake/help/v3.24/command/find_package.html) and [installation components](https://cmake.org/cmake/help/v3.24/command/install.html) documentation.

## Recover from a refusal

| Result | Meaning and next step |
| --- | --- |
| Invalid argument | Check absolute paths, full commit/digest, filename and numeric bounds. The preview should work before apply. |
| Unavailable | Inspect compiled HTTP capabilities and dependencies. Do not substitute a fake inference response. |
| Already exists | The destination was not overwritten. Review the existing file and choose a new destination. |
| Busy | A session owner or guard is already active. Stop the owning session normally; do not delete a lock file to bypass it. |
| Invalid state | A digest may differ, or an inherited engine setting needs review. Compare the complete file identity and environment; do not weaken the expected hash. |
| I/O error or timeout | Check free disk space, certificates, connectivity and retained partial files. Engine diagnostics may identify memory, flags or port failures. |
| Chat fails while process runs | Wait for load completion, verify the alias and endpoint, and inspect engine diagnostics. Running alone is not readiness. |

For the ownership and C implementation concepts, continue with [the local model lifecycle lessons](learning/local-model-lifecycle.html).

## Wait explicitly for an endpoint report

A loading engine may answer with HTTP 503 before it becomes ready, or may not accept connections yet. The **wait** command repeats the shared health probe within a reviewed plan. It never launches or stops a process. The values after the port are maximum attempts, pause between attempts in milliseconds, and timeout for each request.

~~~powershell
umicom models wait 8080 10 250 3000
umicom models wait 8080 10 250 3000 --probe
~~~

The first command prints the plan without making a request. The second makes at most ten loopback GET requests and stops at the first valid ready report. Its nominal budget is 10 × 3000 + 9 × 250 = 32250 ms. There is no pause after the final attempt. Scheduling and transport cleanup can add elapsed time; this is a bounded amount of requested work, not a hard wall-clock deadline.

Plans admit 1–64 attempts, 50–5000 ms pauses, 100–60000 ms request timeouts, and a nominal total no greater than 60000 ms. A valid unavailable response, an I/O error or a request timeout can trigger another attempt. Unexpected HTTP statuses currently map to I/O error in the shared transport and therefore also consume this retry allowance. Malformed or oversized responses, unavailable compiled transport and other errors stop immediately. Redirects are never followed.

An exhausted plan returns TIMEOUT even when the final probe succeeded in reporting unavailable. The CLI prints both completed attempts and the last probe status so these two outcomes are distinguishable. Exit zero means a successful preview or an endpoint-reported ready result; exit one means unsuccessful execution or output, and exit two means invalid arguments. The standalone umicom-models host accepts the same arguments.

~~~c
UmiModelReadinessPlan plan;
UmiModelReadinessReport report = {0};
UmiStatus status = UmiModelReadinessPlanCreate(
    8080U, 10U, 250U, 3000U, &plan);
if (status == UMI_STATUS_OK) {
    /* Run on an application worker after review. */
    status = UmiModelReadinessWait(
        &plan, user_approved_probe, cancellation, &report);
}
/* Check status first. report.has_health refers only to the last probe.
 * A ready endpoint report is still not model identity or inference proof. */
~~~

Admission refusals and cancellation already requested at entry preserve the previous output. After admission, every return publishes this call’s report, including failure. If a valid unavailable response is followed by malformed JSON, the second probe clears has_health and the health fields rather than leaving the first observation looking current. Keep attempts_completed, last_probe_status and the overall return status together.

Applications must keep the cancellation token alive until the worker has joined. Request cancellation from another thread; do not reset or destroy the token while it is borrowed. Pauses check the token in slices of at most 20 requested milliseconds, subject to scheduling delays. A cancelled wait leaves the model session and its owning console intact. The CLI uses a bounded foreground wait; the API provides cooperative cancellation for application workers.

## Qualify readiness sequences

~~~powershell
cmake --build --preset windows-ucrt64-debug --target umicom-model-readiness-test umicom-model-health-test umicom umicom-models
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]local_models[.](readiness|readiness_http)[.]" --output-on-failure --no-tests=error
~~~

The added cases cover 19 admission/budget groups and 15 native loopback sequences. One admission group requires a separate HTTP-OFF build; it reports a skip when HTTP support is present. Network sequences report a skip when HTTP support is absent. Retain these reasons separately from passes. The fixture returns scripted health replies and closes selected connections; it does not load weights or claim real inference. These are commands to execute during qualification, not a record of passed tests.

Exercise the installed SDK consumer as well. It resolves the plan, budget and wait symbols using pure planning and denied approval, so it needs no server. Real-engine acceptance still requires the separate process snapshot, model digest, trusted endpoint, reviewed inference request and normal shutdown described above.

Sammy Hegab · Umicom Foundation · Source examples: MIT.
