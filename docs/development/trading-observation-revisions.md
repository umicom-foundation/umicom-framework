![Umicom](../../resources/brand/umicom-logo.svg)

# Read trading revisions within their owner

A revision identifies changes made by one owner during its lifetime. The trading workspace snapshot now exposes the workspace's actual revision. Its chart and trade-tape snapshots retain their existing separate revision fields.

Earlier snapshot code added child revisions and the number of nonzero positions. That sum could wrap even when the owner revision had not wrapped. It could also hide a closing fill: the owner advanced once while the number of nonzero positions fell by one. Two different states then produced the same sum.

## Read each domain separately

1. Retain the workspace identity whose snapshot you are reading.
2. Use `snapshot.revision` for the trading owner's observation.
3. Inspect `snapshot.charts.revision` when observing the chart subsystem.
4. Inspect `snapshot.trade_tape.revision` when observing the trade tape.
5. Keep position counts and quantities as data, rather than adding them to an identity.

A child can change through its own API without changing the trading owner. The nested fields remain available for that reason. This correction does not redesign the child services or promise that revisions from unrelated workspaces are globally comparable.

~~~c
UmiTradingWorkspaceSnapshot current;
UmiStatus status = umi_trading_workspace_snapshot(workspace, &current);
if (status == UMI_STATUS_OK) {
    /* Compare each field only with an earlier observation of this same owner.
     * A reader interested in child domains must inspect their fields too. */
}
~~~

The snapshot API still performs its existing selection reconciliation. It is not a general const capture API. Local session-report capture is a separate operation and uses the same trading owner revision without borrowing the live book.

## Recognize the boundary cases

- Opening or closing a position must not add or subtract an extra revision while reading.
- Repeated reads do not synthesize later identities.
- An owner at `UINT64_MAX` can still be observed without arithmetic wraparound in this field.
- A trade-tape-only change remains visible in the tape snapshot while the owner revision stays unchanged.
- A session report and workspace snapshot taken without intervening owner mutation agree on the trading revision.

A reader that previously used the old sum as an all-domain cache key must compare its relevant fields separately. Do not replace the sum with another arithmetic hash and treat it as a collision-free identity.

## Owner qualification

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-snapshot-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]trading_execution[.]snapshot[.]"
~~~

Run each command separately and stop on errors. The fixture checks empty and populated owners, final revisions, repeated reads, a position-closing fill, cancellation, report agreement and a separate tape change. It includes the real owner for boundary injection rather than exposing a public revision setter.

The expected result is that all registered snapshot cases pass against the new executable. The source and checks are authored; compiler, linker, test and native UI qualification remain unexecuted.

This change does not qualify every mutation path's revision capacity or every frontend's numeric presentation. Those require their own review and execution evidence. Continue with [execution publication](execution-publication.md) and [local cancellation](order-cancellation.md) for the guarded mutation paths.

## Protective changes at the final revision

Health observations, kill-switch engagement and live disarming remain available at `UINT64_MAX`. These operations retain the maximum revision instead of wrapping. Refresh the snapshot and inspect its control fields at that boundary; revision equality alone cannot establish equal state. A session-report current check conservatively returns false at the maximum, while the report remains readable. See [protective trading controls](protective-trading-controls.md) for checked reset results and qualification.
