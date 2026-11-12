![Umicom](../../resources/brand/umicom-logo.svg)

# Keep protective controls available

A revision counter helps a client recognize state changes. It must not prevent an operator from engaging a kill switch. Framework distinguishes actions that record or strengthen protection from commands that release a gate.

This distinction belongs in the shared workspace. Native views use the existing action controller, and direct clients use the same owner APIs. None of these operations sends a cancellation to an external broker or establishes a certified trading safety system.

## Understand the boundary

| Operation | At the final workspace revision |
| --- | --- |
| Record provider health | Retain the observed flags, invalidate preview evidence, keep revision at its maximum |
| Disarm live mode | Clear the armed flag, invalidate preview evidence, keep the maximum revision |
| Engage the kill switch | Engage, retain the reason, disarm live mode and invalidate preview evidence |
| Arm live mode | Refuse with `CAPACITY_EXCEEDED` after checking the existing environment and readiness requirements |
| Reset the kill switch | Refuse with `CAPACITY_EXCEEDED` and retain the complete owner state |
| Preview or submit an order | Refuse through the existing revision-admission checks |

Below the maximum, an accepted protective operation advances the revision once. At the maximum it saturates: it retains the maximum value instead of adding one and wrapping to zero. Health recovery can still be recorded accurately, but it does not create new revision capacity or authorize an order.

## Use checked reset results

1. Use `UmiTradingWorkspaceResetKillSwitch` when a client needs a result.
2. Treat `OK` as confirmation of the local reset only.
3. On `CAPACITY_EXCEEDED`, leave the protection indicator engaged and explain that the workspace must be retired through its session workflow.
4. Re-read the snapshot after a control action. At the maximum, equality of two revision values does not establish equal control state.
5. Keep external reconciliation separate. A local switch does not acknowledge or cancel an order at a venue.

~~~c
UmiStatus status = UmiTradingWorkspaceResetKillSwitch(workspace);
if (status != UMI_STATUS_OK) {
    /* Retain the engaged state. Report the returned status to the operator. */
}
~~~

The older `umi_trading_workspace_reset_kill_switch` function remains available with its original void signature. It delegates to the checked operation. A void return is not evidence of success; a caller using that interface must inspect the resulting snapshot. New clients should use the checked form.

The Framework UI controller now records the returned reset status and message before notifying its observer. A refusal still notifies the view, so the observer must inspect `last_status` rather than interpreting a callback as success. No application needs a second reset implementation.

## Read exhausted observations conservatively

A snapshot at `UINT64_MAX` remains readable. Protective actions may change flags while that revision stays fixed. Refresh and inspect the actual fields instead of caching the state solely by revision.

Session reports remain readable historical copies. Their current-state check returns false at the maximum even if the report was just captured. This deliberately avoids certifying a state that can change without another representable revision. It is not a claim that the report's copied orders or arithmetic are invalid.

The new semantics do not qualify every remaining workspace mutation path. Market-data, selection and other owners have separate admission and lifetime contracts. Do not infer complete workspace safety from the focused control checks.

## Qualify direct and UI-facing behavior

After integrating and configuring the current Framework source, run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-protection-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]trading_execution[.]protection[.]" --output-on-failure
~~~

The expected result is 26 passing cases. The fixture checks ordinary, final and exhausted revisions for health, engagement, disarming, arming and both reset interfaces; controller notifications; repeated engagement; recovered health; invalid arming conditions; and null arguments. It exercises the actual workspace owner with boundary injection, without exposing a public revision setter.

These tests are authored and statically reviewed, not executed evidence. The owner's compiler, linker, test run and native UI review must establish the result. In the native application, confirm that a refused reset leaves the switch displayed as engaged and shows the refusal message.

Continue with [draft-change admission](draft-change-admission.md) and [observation revisions](trading-observation-revisions.md).
