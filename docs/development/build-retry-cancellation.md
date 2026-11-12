![Umicom](../../resources/brand/umicom-logo.svg)

# Preserve cancellation when a retry is refused

Retry means asking the dependency graph to make a completed unsuccessful node eligible again. It does not mean starting a child process immediately. The graph owns the node state and attempt budget; the engine owns the cancellation token used by later execution.

The engine now asks the graph first. It resets cancellation only when the graph accepts that retry. Previously, even a missing node name or exhausted attempt budget could clear an earlier stop request before returning an error. Keeping admission first prevents a rejected command from changing unrelated execution intent.

## Follow the retry boundary

1. Read the selected node and explain its current state.
2. Call `umi_build_engine_retry` with the selected node ID.
3. If the result is not `UMI_STATUS_OK`, keep the existing cancellation indication and node selection.
4. If accepted, refresh graph presentation. The node becomes pending or ready according to its dependencies.
5. Start eligible work only through the normal execution workflow.

~~~c
UmiStatus status = umi_build_engine_retry(engine, selected_node_id);
if (status == UMI_STATUS_OK) {
    /* Refresh the graph. No child has been launched by retry itself. */
} else {
    /* Explain the refusal. Do not clear cancellation as a recovery shortcut. */
}
~~~

Studio's existing build-service retry delegates to this Framework contract and selects the node only after acceptance. Clients should reuse that boundary instead of resetting the token first in application code.

## Understand the state rules

| Selected node | Retry result |
| --- | --- |
| Failed, cancelled or timed out, with attempts remaining | Accepted; cancellation is reset after graph admission. |
| Ready, waiting, blocked, running or succeeded | Refused; cancellation remains unchanged. |
| Attempt budget exhausted | Refused; the budget is not enlarged or reset. |
| Missing node | `NOT_FOUND`; cancellation remains unchanged. |
| Null engine or node ID | `INVALID_ARGUMENT`; no retry is admitted. |

An accepted retry preserves the existing attempt count. A later graph start consumes the next attempt. It also clears the node's previous progress and terminal status according to the existing graph policy. Retained build history is separate and is not erased by retry.

A cancellation token may be owned by the engine or borrowed from its caller. In either case, refused retry must preserve its state. For a borrowed token, its generation is also unchanged on refusal. The caller remains responsible for its lifetime and destroys it only after every borrower has finished.

## Separate cancellation from completion

A requested cancellation is an instruction to cooperative work. It is not proof that the work stopped, that handles were released or that a native window closed. Retrying an already failed node cannot be used to bypass an active operation's owner.

Serialize graph and engine lifecycle mutations through their owning client. This retry function is synchronous; it does not provide a scheduler lock or make arbitrary concurrent graph mutation safe. Native Stop, completion delivery and Retry still need host-level qualification.

## Diagnose refusal

For `NOT_FOUND`, refresh the selected node identity rather than guessing another name. For `INVALID_STATE`, inspect the node state and remaining attempts. A running node needs its normal completion path; a successful node needs the intended invalidation workflow if its inputs changed.

Do not call `umi_build_engine_reset_cancel` automatically after a refusal. That would restore the same unwanted side effect outside the corrected service. An explicit reset remains available for a deliberately chosen lifecycle action, and requires its own owner decision.

## Owner qualification

After integrating and configuring the matching Framework source, run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-build-retry-cancellation-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]build_retry_cancellation[.]" --output-on-failure
~~~

The expected result is 52 passing scenarios: thirteen graph situations, each with an engine-owned or borrowed token, initially requested or clear. Refusals compare complete engine and node snapshots and preserve borrowed-token generation. Accepted cases check readiness, unchanged attempt budget, reset progress/status and unchanged unrelated nodes. The fixture changes graph state through public APIs and never calls an engine execution function.

Source and registration are statically inspected. Compilation, test execution, Studio commands, concurrent producer behaviour and native Stop/Retry presentation remain owner-run and unverified. Continue with the Framework development course's build-history module to distinguish a retained attempt from a retry request.
