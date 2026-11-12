![Umicom](../../resources/brand/umicom-logo.svg)

# Admit a draft change before editing the ticket

An order ticket is a draft. Its last risk preview describes the values that were evaluated at a particular time. Changing the ticket must invalidate that preview and publish an observable change. Framework owns both operations so a command-line client and a native Trader view use the same rules.

A workspace revision is a 64-bit unsigned observation number. It has a final representable value, `UINT64_MAX`. Adding one there would wrap to zero and could make a new state look older than an earlier observation. The draft commands reserve that capacity before changing any owner state.

## Commands covered

The following entry points validate their arguments, check revision capacity, then perform their existing edit and advance the revision once:

| Command | Existing behavior retained |
| --- | --- |
| `umi_trading_workspace_set_draft_side` | Choose buy or sell and invalidate the preview |
| `umi_trading_workspace_set_draft_type` | Set type and time in force; clear a stop price when the type does not use it |
| `umi_trading_workspace_set_draft_quantity` | Require a finite positive quantity |
| `umi_trading_workspace_set_draft_prices` | Require finite, nonnegative prices |
| `UmiTradingWorkspaceSetRiskPricePolicy` | Validate and replace the price-evidence policy |
| `UmiTradingWorkspaceResetDraft` | Restore ticket defaults, retain the selected instrument and invalidate preview evidence |

These commands are edits even when the supplied value equals the current value. A repeated setter still invalidates the preview and consumes one revision. Reset likewise requires a revision. This preserves their established behavior; callers should avoid issuing setters merely to read or refresh displayed values.

## Use the result in a client

1. Read the user's proposed value into client-owned temporary storage.
2. Call the Framework setter and inspect its return value.
3. On success, refresh the displayed ticket and request a new preview when appropriate.
4. On refusal, retain the current ticket display and explain the returned status. Do not display the proposed edit as accepted.
5. Re-evaluate risk when submitting. An earlier preview is not permission to submit.

~~~c
UmiStatus status = umi_trading_workspace_set_draft_quantity(workspace, requested);
if (status == UMI_STATUS_OK) {
    /* Read a new snapshot; the old preview is no longer current. */
} else if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
    /* The ticket and its existing preview were left unchanged. */
} else {
    /* Correct the invalid value or report the returned domain error. */
}
~~~

Argument validation still occurs first. For example, an invalid quantity remains `INVALID_ARGUMENT` even when the revision is exhausted. A valid edit at that boundary returns `CAPACITY_EXCEEDED`. No order is submitted, cancelled or filled by a draft edit.

After a successful edit, `UmiTradingWorkspaceRiskEvidence` returns `NOT_FOUND` until a new preview or submission supplies evidence. Its output remains untouched when no evidence exists. On a capacity refusal, existing evidence remains available because the ticket was not edited.

## Recover deliberately

Reaching the final revision requires retiring the owner through the application's session-lifecycle workflow. Preserve any necessary session records first. Do not reset the counter in place, replay a submission automatically or copy private workspace memory into a new owner. A newly created workspace has a separate lifetime; an old revision number is not a global identifier.

These guards cover the six commands listed above. They do not certify every workspace mutator, market-data update, child registry or emergency control. In particular, a kill switch needs its own protective-action semantics and must not be replaced with a generic early return because a display counter is full.

## Qualify the boundary

From the Framework checkout in the owner's workspace, after integrating and configuring the current source:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-draft-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]trading_execution[.]draft[.]" --output-on-failure
~~~

Stop on a command failure. The expected result is 30 passing cases covering ordinary edits, the final revision, exhausted edits, repeated values at exhaustion and invalid arguments at exhaustion. The fixture includes the actual workspace owner to inject boundary state; installed clients continue using its opaque public API.

A compile or link failure must be corrected before interpreting test results. The authored suite has not been executed as part of this source delivery. Native ticket refresh, user messaging and session retirement require separate application qualification.

Continue with [local order policies](local-order-policies.md), [execution publication](execution-publication.md) and [unsigned revision display](unsigned-ui-values.md).
