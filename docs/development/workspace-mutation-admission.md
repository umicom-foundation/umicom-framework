![Umicom](../../resources/brand/umicom-logo.svg)

# Reserve an observation before changing a workspace

A trading workspace coordinates several owners: market rows, candle histories, a public trade tape, alerts, an order book and selections. A command must check whether its observation can be published before changing a child owner. Checking afterward can return a failure even though the operation already changed data.

The shared Framework workspace now admits its revision before the existing instrument, market-data, browsing, environment, refresh and alert operations perform their first mutation. This keeps application clients thin: they inspect one returned status rather than trying to undo a partially applied change themselves.

## Know which revision is needed

| Operation group | Admission before mutation |
| --- | --- |
| Add or remove an instrument | Workspace revision |
| Update quote, bar, depth or public trade | Workspace and affected market-row revisions |
| Change market state | Workspace and affected market-row revisions |
| Filter/select instruments or select an order | Workspace revision |
| Change a chart study or environment | Workspace revision |
| Refresh selection state | Workspace revision |
| Add, remove, enable or acknowledge an alert | Workspace revision |

At an exhausted required revision, a valid command returns `CAPACITY_EXCEEDED` before changing the owner. Existing argument checks and identity lookups still apply. Child services can reject their own commands independently; outer admission does not promise that a child has capacity.

A chart-study request that already matches its stored study and period retains its existing no-op behavior. Other listed setters keep their established edit semantics, including repeated selections. This change does not turn every setter into an idempotent command.

## Follow one market update

For a quote update, the workspace validates the quote, finds the instrument, verifies contract metadata and rejects older evidence. It then admits both observation counters before asking the alert book to evaluate the new midpoint. Only after dependent evaluation succeeds does it publish the quote and advance the two revisions.

For a public trade, admission occurs before the tape append. A refused outer revision therefore cannot leave an accepted print in the child tape while the market row still shows the earlier value.

For a bar, the owner checks time ordering and observation capacity before replacing a forming candle or shifting a full history window. Capacity refusal preserves the existing history.

## Handle a refusal in a client

1. Keep incoming data in a temporary value.
2. Call the workspace's canonical update operation.
3. Publish frontend success only after `OK`.
4. If capacity is exhausted, preserve the retained session evidence and stop sending mutations to that owner.
5. Retire and replace the owner through an explicit lifecycle workflow. Do not reset counters in place or assume a new workspace continues the old provider sequence automatically.
6. Report child failures distinctly. A revision guard does not repair malformed provider data or a failed child transaction.

~~~c
UmiStatus status = umi_trading_workspace_update_quote(workspace, &incoming);
if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
    /* The exhausted outer or market observation was detected before mutation.
     * Retain the existing displayed quote and report the capacity failure. */
}
~~~

Emergency engagement, live disarming and provider-health observations use the separate [protective-control policy](protective-trading-controls.md). They remain possible while their counters saturate. They must not be replaced with generic mutation refusals.

## Understand the limits of this change

<!-- The earlier limitation is retained for review; the shared depth validation guide now describes its implemented boundary.
The checks reserve outer and market-row observations. They do not certify every nested service, input validator or cache. Alert-book revision handling and market-depth validation require independent review. A readable snapshot also retains its existing selection-reconciliation behavior; it is not a general immutable capture.
-->
The checks reserve outer and market-row observations. They do not certify every nested service or cache. Alert-book revisions use their own [admission and staged-removal contract](alert-book-admission.md). Depth identity and active levels are checked by the shared [market-depth validator](market-depth-validation.md). A readable snapshot also retains its existing selection-reconciliation behavior; it is not a general immutable capture.

At a maximum revision, inspect actual protective fields after refreshing. Revision equality alone cannot establish unchanged protective state. Historical session reports remain readable, but their current-state checks are conservative at that boundary.

## Qualify the command paths

After integrating and configuring Framework, run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-workspace-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework[.]trading_execution[.]workspace[.]" --output-on-failure
~~~

The expected result is 61 passing cases: ordinary, final and exhausted outer revisions across 17 command paths, plus final and exhausted market-row revisions for five data/state operations. Refusal checks compare complete owner storage and the separately allocated trade tape. Success checks verify the original domain effect.

The fixture includes the real opaque owners for boundary injection. It introduces no public testing setters. These cases are authored and statically reviewed; compilation, linking, test execution and native application behavior remain owner-qualified.

Continue with [trade-tape capacity](trade-tape-capacity.md), [draft admission](draft-change-admission.md) and [execution publication](execution-publication.md).
