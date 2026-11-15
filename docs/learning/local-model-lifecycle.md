# Build a local model lifecycle in C

This six-lesson workshop follows the implementation of Umicom Local Models. You will separate model files from inference engines, prepare reviewable downloads, supervise a model process and connect an application through Framework's existing AI provider. The examples use C23. Read the [operator guide](../local-model-manager.html) for complete commands and recovery steps.

## Lesson 1 Separate weights engine and client

A GGUF file stores model metadata and tensor data. It is not the chat application, the inference engine or an installer. The engine loads weights and executes numerical operations; a client sends requests and displays responses. Keeping these responsibilities separate lets Studio, Media, Trader and RAG share a local server without embedding the same model loader in every process.

The Umicom service owns decisions about downloads, file identity and process lifetime. The selected external llama.cpp executable owns inference. Its C/C++ implementation is an external dependency boundary; Umicom's manager is C and does not require a Python server. Assembly should enter only through a measured platform or inference need, not through duplicated application orchestration.

~~~text
Umicom product or console
    -> Framework model plan and session owner
        -> selected local llama-server process
    -> Framework AI runtime and loopback provider
        -> HTTP request to 127.0.0.1
~~~

Do not confuse the two paths. Starting a process does not send a prompt. Registering a provider does not download a model. Selecting a catalogue row does not prove the file exists. These distinctions make both the interface and its tests easier to reason about.

**Exercise.** Classify each observation: catalogue entry found, SHA-256 matches, process submitted, process running, HTTP response received. Which is evidence of completed inference? Only the final observation can establish a response for that particular request; none establishes overall model quality.

**Source reading.** Compare the public contracts in **include/umicom/local_models/manager.h** with the existing **include/umicom/ai/provider.h**. The former owns model lifecycle; the latter describes an application-facing generation interface.

## Lesson 2 Plan before downloading

A reviewable plan contains the source identity, destination and bounds. The native source type carries a repository, a full commit identifier, a filename, an expected SHA-256 and a byte count. The plan builder creates a public HTTPS URL from those fields. It does not accept an arbitrary URL carrying tokens or query parameters.

~~~c
const UmiModelCatalogueEntry *entry =
    UmiModelCatalogueFind("qwen2.5-0.5b-q4");
UmiModelDownloadPlan plan;
UmiStatus status = entry == NULL ? UMI_STATUS_NOT_FOUND :
    UmiModelDownloadPlanCreate(&entry->source, selected_path, &plan);
/* Display plan.url, plan.destination, plan.source.bytes and sha256.
 * Do not call Download until the user has reviewed this exact plan. */
~~~

The apply operation validates the plan again. Otherwise a caller could modify a public structure after review and redirect execution somewhere else. Consent is a separate boolean argument, not a property a downloaded model can grant itself. Large-file redirects stay HTTPS and have a fixed limit; they do not carry credentials from a local credential file.

The output service creates a new file exclusively. That rule avoids silently replacing valuable existing weights. A cancelled transfer may leave a partial file; the application must report that state instead of treating existence as installation. Reopen and hash stored bytes after the download, because the bytes received over the network are not automatically proof of durable file contents.

**Exercise.** Preview a valid catalogue download, then try a relative destination, a branch name instead of a commit, a filename containing **../**, and an unknown model ID. The invalid cases should fail before a transfer. Do not use a real model download to test these admission rules.

**Regression reading.** **tests/local_models/test_plan.c** covers consent, URL tampering and bounded arguments. Its cancellation case requests cancellation before apply so no real network or file creation is needed. A test that happens to pass because a machine has no network is not a reliable permission test; assert the exact refusal status.

## Lesson 3 Inspect large files with bounded memory

Loading an entire multi-gigabyte file just to hash it wastes memory before the inference engine even starts. Framework's **UmiInputFileVisit** reads a regular file through one native handle and passes at most 64 KiB to a visitor. Each pointer is borrowed only during its callback. SHA-256 state retains a small compression buffer rather than the whole file.

~~~c
/* A visitor can return CANCELLED or another failure to stop the read.
 * Publish a digest only after Visit returns OK: later I/O can still fail. */
UmiStatus Observe(const unsigned char *bytes, size_t size, void *context)
{
    UmiSha256 *hash = context;
    return UmiSha256Update(hash, bytes, size);
}
~~~

The model inspector also retains the first 24 bytes for the basic GGUF header. Magic bytes distinguish the expected format; the version selects an understood header layout. Count checks reject obvious truncation or implausible values. They do not parse every tensor, infer memory needs or certify compatibility. A correct file digest proves identity with a reviewed expected digest, not that the publisher or model is trustworthy.

The platform layer rejects final symbolic links and special files, and checks observable metadata before and after the read. Trusted parent directories remain a requirement. A separate engine later opens the path again, so the model must stay stable through launch and use; this is not a proof against an adversarial writer swapping paths.

**Exercise.** Use a tiny synthetic file in a newly created test directory. Test wrong magic, an unsupported version, a short header and a wrong hash separately. Never describe the synthetic file as loadable model weights. Then exercise a file larger than two chunks to catch errors in byte counters and offsets.

**Regression reading.** **test_stream.c** checks chunk sizes, early cancellation and final-leaf rejection. **test_inspect.c** checks a Unicode filename and wrong identity. The fixtures create unique directories and retain their evidence; they never delete an arbitrary file selected by a test caller.

## Lesson 4 Preserve ownership during failure

A common model-switch bug closes the current model before the replacement finishes loading. If the replacement fails, the user loses the working session. This implementation uses a simpler explicit rule: starting into an occupied owner returns **BUSY**. The caller must stop the previous session as a separate action. This avoids implying an atomic seamless swap that the service does not implement.

~~~c
UmiModelSession *session = NULL;
/* Build and display a UmiModelRunPlan first. Run Start on a worker. */
UmiStatus status = UmiModelSessionStart(&plan, user_approved, &session);
if (status != UMI_STATUS_OK) {
    /* A cleanup failure can retain an owner: keep it available for Stop. */
    report_start_failure(status);
}
/* Poll UmiModelSessionRead; running is a process state, not readiness. */
~~~

The service delegates process creation to Framework's supervisor. Paths and arguments are distinct strings, not one interpolated command. The working directory is the model's captured parent. A hidden child window keeps background engine output in the owning application. A cooperative file lease discourages two managers from starting the same selected model path at once; it is not a system-wide GPU or port allocator.

Connect generation through the existing AI runtime:

~~~c
UmiAiProvider provider = {0};
UmiStatus status = UmiModelLocalProviderCreate(
    8080, 30000, application_cancellation, &provider);
if (status == UMI_STATUS_OK) {
    status = umi_ai_provider_registry_add(&runtime.providers, &provider);
    if (status != UMI_STATUS_OK)
        provider.destroy(provider.instance); /* ownership did not transfer */
}
~~~

The cancellation object is borrowed and must outlive the provider. At shutdown, stop and join application request workers, destroy the AI runtime and its providers, then stop the model session. If **UmiModelSessionStop** leaves the owner non-null on an error, retain it and retry cleanup. Do not free the wrapper and forget a still-owned native process.

**Exercise.** Start the finite server fixture, try to start into the same owner, and confirm that the original pointer remains intact. Try a second owner for the same guard. Stop normally and reacquire the guard. Repeat with a wrong model digest and verify that no session was published. These are process and ownership tests, not inference or HTTP readiness tests.

## Lesson 5 Separate process state from endpoint reports

A process can be running while its model is loading, after it has failed to bind a port, or while another process owns that port. A file digest answers a different question again: which bytes were inspected? Build the client around separate observations rather than collapsing them into one ready flag.

Read UmiModelHealthProbe in src/local_models/session.c. It checks bounds and explicit consent before delegating to UmiAiWorkspaceLocalHealthRead. The shared loopback transport owns both chat POST and health GET restrictions. No application needs a second HTTP client merely to show health.

1. Preview models health 8080 3000 while no server is running. It must describe the request without contacting the port.
2. Use the explicit --probe flag against a trusted server. Record whether the failure is transport, JSON admission or a reported unavailable state.
3. Compare that observation with UmiModelSessionRead and a reviewed chat result. Explain why none can replace the other two.
4. Inspect test_health.c: the native fixture asserts one GET request with no prompt body, credentials or redirect follow-up. The fixture never loads model weights.

The codec admits a bounded complete JSON response and publishes a staged result only after all checks. Its offline cases cover wrong types, embedded NUL, bad UTF-8, contradictory fields, an incorrect HTTP code, trailing data and oversized input. The transport cases cover actual loopback behavior, proxy bypass, timeout and cancellation. A cancellation token remains borrowed until the request has stopped.

Exercise: let a previous successful observation remain in the result object, then cancel the next request before it starts. The API leaves the object unchanged and returns CANCELLED. Explain how a UI that ignores the return value could display stale success. Add a timestamp or freshness label in the application model; do not alter the raw observation to conceal the failed read.

The manager does not bind this health response cryptographically or through a process-owned channel to its child. A later design may add authenticated endpoint identity, but a local port number is insufficient evidence. Keep that limitation visible when reviewing application behavior. Follow the operator guide for source and HTTP-OFF qualification commands.

## Qualify the complete workflow

The authored tests are registered under **framework.local_models**. The operator guide provides build and test commands. Record compiler results, actual passes and skips separately from source review. Real engine qualification additionally needs a trusted engine identity, matching model digest, completed load, a reviewed chat response and a clean normal stop. Test a busy port and an incompatible model so the UI cannot accidentally show success after a failed child.

The native console is a useful first client because it exposes each transition explicitly. A future graphical client can reuse these contracts without duplicating downloads, process ownership or JSON transport. Keep heavy hashing and downloads on workers; keep controls on the UI thread. Design progress, cancellation and failure states before adding an attractive catalogue grid.

Image and video engines require different model packages and execution adapters. Retrieval also needs a deliberate choice between existing text search and an embedding provider. Keep those capabilities explicit; a text chat server is not an image renderer, video generator or embeddings service.

## Further reading

- [GGUF specification](https://github.com/ggml-org/ggml/blob/master/docs/gguf.md) explains the format beyond this manager's basic header inspection.
- [llama.cpp server](https://github.com/ggml-org/llama.cpp/blob/master/tools/server/README.md) describes the selected external engine's arguments and HTTP API.
- [Hugging Face model downloads](https://huggingface.co/docs/hub/models-downloading) explains publisher files and revision selection.

## Lesson 6 Bound retries and report the current attempt

A retry loop needs a stopping rule that readers and callers can inspect. An unbounded loop can occupy a worker forever when a port never opens. A fixed sleep does not prove that loading completed. The readiness plan instead limits request count, individual request timeout and pauses, while leaving the existing single-probe API unchanged.

Read **src/local_models/readiness.c**. UmiModelReadinessBudget validates every public field and widens arithmetic to uint64_t before multiplying. The formula is attempts × timeout + (attempts − 1) × pause. Subtracting one is valid only after checking that attempts is at least one. The last attempt has no following pause. The result describes configured work; it does not promise exact elapsed time under an operating-system scheduler.

~~~c
UmiModelReadinessPlan plan;
uint32_t nominal_millis = 0U;
UmiStatus status = UmiModelReadinessPlanCreate(
    8080U, 3U, 50U, 100U, &plan);
if (status == UMI_STATUS_OK)
    status = UmiModelReadinessBudget(&plan, &nominal_millis);
/* With these inputs the nominal budget is 400 ms. No I/O occurred. */
~~~

The wait copies an admitted plan before executing it and stages its report in local storage. Each completed probe records its status and replaces the health fields. If the probe failed, those fields are zero and has_health is false. This prevents a prior unavailable or ready-looking object from being mistaken for the most recent network result. Unlike the single-probe API, the admitted wait publishes useful progress even when the overall operation fails. The distinction is part of the public contract.

1. Preview models wait 8080 3 50 100. Confirm that the command prints a nominal budget without requiring a server.
2. Read test_readiness.c and calculate the one-attempt and exact-60000-ms boundary cases by hand. Change a bound in a temporary experiment only if the corresponding rejection expectations change deliberately.
3. Read the wait-loading-ready fixture: the first response is unavailable, the second is ready. Explain why a third request must not happen.
4. Read wait-stale: an unavailable reply is followed by malformed JSON. The report must count two completed probes, carry PARSE_ERROR and contain no current health observation.
5. Compare exhausted unavailable replies with exhausted dropped connections. Both waits return TIMEOUT, but last_probe_status distinguishes a valid unavailable report from I/O failure.

Cancellation is a request to stop work, not permission to destroy another thread’s borrowed memory. The wait checks before each probe and during sliced pauses; the shared transport checks while processing a request. The owner requests cancellation, waits for its worker to finish, then destroys the token. Resetting the token during execution can erase the request before the worker observes it.

The fixture’s cancellation-after-response case permits cancellation during transport cleanup or the following pause. It asserts that no second request occurs, rather than requiring a fragile exact timing window. The malformed and oversized cases stop immediately; temporary connection failures and documented loading replies use the remaining bounded attempts. Unexpected HTTP statuses are retried as I/O errors under the current transport mapping, so do not describe this as an HTTP-status-specific retry policy.

Run the qualification commands in the operator guide with HTTP enabled, then use an independent HTTP-OFF build for the unavailable-transport case. Inspect skips rather than counting them as success. Finally, integrate the blocking API through an application worker: display attempt count and outcome separately from the retained process snapshot, and leave the user’s running session alone if waiting fails.

Sammy Hegab · Umicom Foundation · Source examples: MIT.
