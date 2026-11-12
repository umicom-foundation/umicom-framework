![Umicom](../../resources/brand/umicom-logo.svg)

# Keep build preparation failures in operation history

A build can fail before a compiler starts. The project root may be unavailable, a command may not fit its bounded storage, or the runner may be unable to allocate its diagnostic parser. These are completed attempts with useful evidence. Showing the preceding successful build as the latest operation would hide the new refusal.

Framework's synchronous build runner reserves an operation identity before preparing paths and commands. After that point, every return uses the same completion helper. The helper finalises the caller's result and copies it into the configured bounded history. CLI and Studio clients that use this runner receive the same behaviour; they should not append a second copy themselves.

## Read the result correctly

1. Submit a valid profile through the runner API.
2. Keep the runner, history, cancellation token, clock and result storage alive until the synchronous call returns.
3. Inspect both the returned status and the result's state.
4. If preparation failed, explain its status and retain the operation ID for later review.
5. Read history when presenting previous attempts. A retained result is an observation, not proof that a compiler or application launched.

| Outcome | Result and history |
| --- | --- |
| Path, launch-plan or provider preparation refused | A reserved operation completes as failed and is offered to history. |
| Working directory cannot be stored | Capacity refusal completes with the reserved ID. |
| Environment or diagnostic parser cannot be prepared | Failure is retained; no child process is started. |
| Process succeeds, fails, times out or is cancelled | Existing output, diagnostics, exit status and completion state are retained once. |
| Invalid runner/output argument | No operation is admitted and no history entry is added. |
| Operation identities are exhausted | A failed result with ID zero is returned; no invented operation is appended. |
| No history was configured | The caller still receives the completed result. No history is created implicitly. |

Preparation failures have exit code -1, duration zero and empty child output. They did not observe a child-process exit. An available command description can help diagnosis, but an empty command is expected when preparation stopped before command formatting.

History copies result storage. The caller can reuse its own result after the synchronous call returns without changing the retained entry. A full history evicts the oldest entry according to its existing capacity. Independent runners sharing one history reserve IDs from that history, so they do not each restart at one.

## Keep status ownership explicit

The execution status remains the function's return value. History append is best effort, as it was for process completion before this change. A history-storage refusal does not rewrite a compiler failure as a different execution result. Consequently, the API does not promise durable logging or guaranteed persistence. Use a separately checked evidence sink if a workflow requires those guarantees.

The result does not carry a new public preparation-stage enum. Diagnose the returned status together with the profile and the point reached in the operation. Do not infer authentication, project trust, source revision, executable identity or native UI readiness from the presence of a history entry.

A queued workflow may already have its own completion collector. This change does not authorise appending that result again or bypassing the queue's owner. The synchronous engine and queued project-session paths should each have one history producer.

## Use the shared runner from a C client

~~~c
UmiBuildResult *result = NULL;
UmiStatus status = umi_build_result_create(&result);
if (status == UMI_STATUS_OK) {
    status = umi_build_runner_run(runner, UMI_BUILD_PHASE_BUILD, result);
    /* Inspect status and result before updating the application's presentation.
     * The configured runner has already offered an admitted result to history. */
    umi_build_result_destroy(result);
}
~~~

Here, `runner` is an already created owner with a validated profile. Creation and configuration are intentionally separate from submission. Call this synchronous operation on an appropriate worker in an interactive application; copying its history does not make execution asynchronous.

## Diagnose and recover

For a source-root refusal, verify the project location and access before retrying. For command or directory capacity, shorten or correct the configured value rather than truncating it. For environment parsing, review the non-secret definitions. For parser allocation, investigate resource pressure; do not launch without the diagnostic owner merely to obtain a green status.

A retry is a new operation. Keep the old failure as evidence, and do not clear history to make a failed attempt disappear. When identity capacity is exhausted, use a reviewed new owner lifecycle rather than resetting an existing history's IDs.

## Owner qualification

After integrating and configuring the matching Framework source, run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-build-preparation-history-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]build_preparation_history[.]" --output-on-failure
~~~

The expected result is 62 passing scenarios. The fixture compiles the real runner with private controlled preparation and process boundaries, while retaining the real history, result, environment-plan and diagnostic-parser owners. It covers legacy, observed and automatic-test entry points; each applicable preparation refusal; ordinary process outcomes; no-history callers; invalid arguments; identity exhaustion; ring eviction; and independent runners sharing history.

These deterministic cases do not qualify process creation, CMake, CTest, installed SDK use or Studio rendering. Existing live-build-output process fixtures remain necessary for those process boundaries. Source and test registration have been statically inspected; compilation, linking, test execution and native presentation remain unverified until owner qualification.
