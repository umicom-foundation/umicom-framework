![Umicom](../../resources/brand/umicom-logo.svg)

# Publish a local execution as one change

An execution report describes a fill. In a local trading workspace, accepting that report affects three records: the order, the position and the retained execution history. The workspace revision identifies their combined observation. Updating only part of this state would leave readers with contradictory evidence.

The canonical entry point is `umi_trading_workspace_record_execution` in [workspace.c](../../src/trading/workspace.c). It receives an already-owned local report; it does not connect to an exchange, authenticate a provider or settle a trade.

## Follow the admission order

1. Validate the incoming report before comparing its identifiers.
2. Search retained execution evidence using the bounded execution store.
3. If the identifier is already retained, compare the complete supported report fields. An exact replay succeeds without mutation; different economics under that identifier return `UMI_STATUS_ALREADY_EXISTS`.
4. For a new event, require execution-store capacity and an available workspace revision.
5. Resolve its order through the canonical bounded order-store lookup. Malformed retained identifiers or an out-of-range count refuse the operation.
6. Stage the order fill and proposed position in local values. Check order revision, quantity, price, position capacity and representable arithmetic.
7. Retain the execution report.
8. Publish both prepared values and increment the workspace revision. No fallible operation follows the retained report.

These steps require serialized access to the workspace. They are an in-memory transaction, not a database durability guarantee.

## Keep replay available at the final revision

Suppose a workspace reaches `UINT64_MAX` after accepting a report. Receiving the same report again does not need another revision because nothing changes. A different report needs publication capacity and is refused. Resetting the counter would let an older observation appear current again.

This distinction also matters after cancellation. An exact earlier fill remains retained evidence even when its order is now cancelled. A new fill is checked against the current order lifecycle and is refused for a cancelled order.

~~~c
/* workspace and report belong to the caller's serialized local session. */
UmiStatus status = umi_trading_workspace_record_execution(workspace, &report);
if (status == UMI_STATUS_ALREADY_EXISTS) {
    /* Preserve both messages for investigation; do not invent a replacement ID. */
}
~~~

The fragment shows error interpretation only. Production ingestion must additionally establish provider identity and retain its own transport evidence.

## Qualify with the real owner

The regression fixture includes the real workspace implementation so it can inject final revisions and malformed bounded-store state without adding fault setters to the product API. It checks complete owner-value preservation on refusal and replay, along with order, position and execution counts on an accepted final change.

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-admission-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]trading_execution[.]admission[.]"
~~~

Run the commands separately and stop on errors. All registered admission cases should pass against the executable just built. This coverage is authored and awaits owner execution; source inspection has not established compiler, linker, test or GUI results.

If a new event is refused, retain the original report and diagnose the returned status. Do not discard retained history, erase partial fills, change identifiers to evade a conflict or reset revisions inside the same session.

Read [fill quantity boundaries](fill-quantity-boundaries.md) and [local cancellation](order-cancellation.md) for the two value operations used by this owner.
