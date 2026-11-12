![Umicom](../../resources/brand/umicom-logo.svg)

# Cancel a local order without losing its history

A cancellation ends the unfilled part of a local order. It does not erase fills already received. Umicom Framework provides one value operation, `UmiOrderCancel`, for its simulation broker and trading workspace. This function updates local state only; it does not contact a broker or establish an external acknowledgement.

Read the [public contract](../../include/umicom/trading/order_state.h), [state implementation](../../src/trading/order_state.c) and [workspace owner](../../src/trading/workspace.c) together. The caller owns serialization and any surrounding transaction.

## Understand the transition

| Current state | Result | Mutation |
| --- | --- | --- |
| Accepted or partially filled | Success when a revision remains | Cancelled state and one new order revision |
| Already cancelled | Success, including at the maximum revision | None |
| New, validated, filled, rejected or unknown | Invalid state | None |
| Accepted or partially filled at the maximum revision | Capacity exceeded | None |

The requested quantity, accumulated fill quantity and average fill price are preserved. A partial position remains a position after the remaining order is cancelled. Repeating a successful cancellation does not manufacture a new revision or an additional fill.

## Use the value operation

1. Obtain an order owned by your local session.
2. Serialize access to that order.
3. Check any surrounding revision or transaction capacity before calling.
4. Call the canonical operation.
5. Publish only the state that the operation actually changed.

~~~c
#include "umicom/trading/order_state.h"

/* The caller owns this local order and serializes access. */
UmiStatus cancel_local_order(UmiOrder *order)
{
    return UmiOrderCancel(order);
}
~~~

This fragment demonstrates the value boundary. It does not validate the original request, persist an audit record or implement broker transport. A live adapter must retain its own command and acknowledgement lifecycle; a local success must never be presented as a remote cancellation confirmation.

## Use the trading workspace

The workspace command `umi_trading_workspace_cancel_selected_order` first resolves the selected order. It checks the workspace revision before a new cancellation and delegates order-state changes to `UmiOrderCancel`. A refusal preserves the complete owner.

After a successful change, the workspace increments its own revision and reconciles the selection. With the open-orders filter selected, a cancelled order disappears from that view and the selection can become empty. A later command can therefore return `UMI_STATUS_NOT_FOUND`: this describes the missing selection, not a failed rollback of the earlier cancellation.

The simulation broker keeps connection and environment checks in the adapter, then calls the same value operation. Retained orders stay bound to their simulation or paper environment.

## Qualify the boundary

The regression fixture checks the legal states, repeated calls, the last available revision, exhausted revisions, partial fills, simulation and paper broker clients, complete workspace preservation on refusal, and selection retirement under the open-orders filter.

From the Framework repository, the owner can configure and run the focused checks:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-cancel-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.]trading_execution[.]cancel[.]"
~~~

Run each command separately and stop on an error. Confirm the build directory against the preset before running CTest. The fixture includes the real workspace implementation to inject revision exhaustion without adding a mutable production API; the remaining owners come from the Framework static library.

The expected result is that all registered cancellation cases pass against the just-built executable. These checks are authored and await owner execution. Static source review does not establish compilation, linking, test success or GUI behavior.

## Diagnose a refusal

- **Invalid state:** inspect the retained order state. A filled order has no remainder to cancel.
- **Capacity exceeded:** retain the order and session evidence. Do not wrap a revision or reset it to pretend this is the same session.
- **Not found:** inspect the current selection and filter; retain the prior command result.
- **Permission or connection failure:** inspect the broker environment and connection before issuing another local command.

Continue with [fill quantity boundaries](fill-quantity-boundaries.md) to see why a small execution must not round into a complete fill or leave position quantity unchanged.
