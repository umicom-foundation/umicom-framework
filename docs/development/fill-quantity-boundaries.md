![Umicom](../../resources/brand/umicom-logo.svg)

# Keep simulated fill quantities within their bounds

The canonical double-valued fill services are shared by trading clients. A low-level order update applies a new execution report to one order. The trading workspace adds retained execution identity, replay handling and a staged position update. Neither operation sends a broker order or provides a market-matching engine.

Read [fill.c](../../src/trading/fill.c), [position.c](../../src/trading/position.c) and the execution path in [workspace.c](../../src/trading/workspace.c) together. Their ownership boundaries differ, so their successful results have different meanings.

## Admit a quantity before publishing it

1. Validate the report and its order identity.
2. Require an accepted or partially filled order with finite, usable stored values and an available revision.
3. Compute the represented remaining quantity. The new report must fit that amount.
4. Compute the proposed total. It must remain finite, stay within the requested quantity and advance the stored quantity.
5. Calculate the weighted price in a private candidate.
6. Publish the candidate only after all checks succeed.

The former absolute allowance of `1e-9` was independent of quantity scale. For a request of about `1e-12`, that allowance could exceed the entire order. It also allowed a small nonempty remainder to be marked filled. The earlier branches remain in explained disabled blocks for review.

Completion now means that the represented cumulative quantity equals the represented requested quantity. A smaller cumulative value remains partially filled. An oversized report is refused; the function does not silently clamp it.

## Understand representation limits

A positive double may be too small to change a much larger stored double. For example, adding one to `0x1p99` cannot be represented as a distinct double on the supported binary-double targets. A successful acknowledgement would then claim a new fill without a corresponding stored quantity.

The order fill function returns `UMI_STATUS_CAPACITY_EXCEEDED` when that increment is lost. The position function applies the same rule to both increases and reductions. A lost reduction must not record realised profit or loss while leaving exposure unchanged. All refusals preserve the complete destination.

These contracts concern the supplied represented numbers. They do not infer an exchange's lot increment, round external decimal text or promise decimal-exact accounting. The existing [integer-lot execution records](../../include/umicom/trading/core/execution_fill.h) provide a separate representation for clients with explicit lot/tick units. Choose and validate units at the input boundary rather than adding a universal epsilon.

For the double-valued interface, a final fill can use the represented remaining quantity:

~~~c
double remaining = order.request.quantity - order.filled_quantity;
/* Validate the new report and let the shared owner admit it. */
report.fill_quantity = remaining;
UmiStatus status = umi_order_apply_execution(&order, &report);
~~~

This excerpt assumes a privately owned test order and a correctly prepared new report. It is not a way to invent a missing external fill or rewrite received evidence.

## Keep replay separate from arithmetic

The low-level order function does not retain execution IDs. Repeating a report directly applies another new fill if the state and quantity permit it. Applications needing replay protection use `umi_trading_workspace_record_execution`.

The workspace first checks the retained execution ID. An exact replay is harmless, including after completion or cancellation. Reusing an ID for different economics is a conflict. For a new ID, the workspace stages the order and position, retains the execution, then publishes the state together. A failed candidate must not leave a new position, fill count or revision behind.

## Run the focused regressions

The 26 quantity cases are authored in [test_fill_quantities.c](../../tests/trading_execution/test_fill_quantities.c). They cover tiny partial/full fills, overfill refusal, a representable final remainder, lost positive increments, large and subnormal values, invalid reports, exhausted revisions and workspace replay/cancel behavior. Four position cases check long/short increases and reductions, followed by a representable tiny close.

After configuring the shared Framework with the normal owner toolchain, run these commands. Stop on a failed command.

~~~powershell
Set-Location C:\umicom\Umicom-Applications\framework
cmake --build --preset windows-ucrt64-debug --target umicom-trading-execution-quantity-test
ctest --test-dir build/windows-ucrt64-debug -R "^framework\.trading_execution\.quantity\." --output-on-failure
~~~

Keep the existing fill, position, workspace and session-reconciliation checks in the complete qualification run. The focused cases do not replace them. Record the exact source, compiler and actual outcomes; these authored tests have no implied passing result.

If an input is refused near a quantity boundary, inspect its units, represented values and existing fill history. Preserve the original report. Do not lower the remaining quantity, alter an execution ID, remove a guard or increase a tolerance simply to make the update succeed.
