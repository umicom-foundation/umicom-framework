# Capture and check a local trading session

Use `umicom/trading/session_report.h` with `Umicom::trading` when an application
needs a stable review of its retained orders, fills and positions. The report
owns its copied data, so another market update cannot change the evidence the
user is reading or exporting. It can outlive the workspace.

## Use the shared owner

1. On the workspace's owning thread, call `UmiTradingSessionReportCapture` with
   the canonical workspace and an optional instrument filter. A null or empty
   filter means all instruments. The filter is an ASCII-case-insensitive substring
   of instrument ID, symbol, venue or currency. It does not modify other filters.
2. Inspect `UmiTradingSessionReportSummary`. Retained counts describe the complete
   local book; row counts describe the filter result. The account, current
   environment and owner revision identify the capture. They are not a signature
   or proof of a complete external account history.
3. Read copied order, execution, position, currency and issue rows with the
   corresponding `...At` functions. Outputs are cleared on failure. No pointer
   into the workspace or report is returned. Execution rows include their resolved
   order metadata, or explicitly indicate that the order could not be found.
4. Call `UmiTradingSessionReportDescribe(report, NULL, 0, &required)` for the
   text size including its terminator. Allocate that many bytes and call again.
   A short destination reports the required size and leaves empty output.
5. Call `UmiTradingSessionReportExportCsv` for an independent `UmiCsvDocument`.
   Destroy it with `UmiCsvDocumentDestroy`. Typed floating-point cells retain
   round-trip decimal precision using the shared CSV formatter. Text output
   uses the active numeric locale; CSV numeric cells use a decimal point.
6. Destroy the report with `UmiTradingSessionReportDestroy` when finished.

Capture reads the canonical owner directly instead of calling the general UI
snapshot, which may repair selection. It does not submit, cancel, reserve,
authenticate, arm live trading or write a persistence store.

## What the consistency check means

Framework replays fills in retained arrival order with `umi_order_apply_execution`
and `umi_position_apply_fill`, using each order's captured instrument definition.
It compares cumulative quantity, average price, terminal state, position quantity,
average cost and realised profit/loss. It also checks references, duplicate IDs,
account identity and the recorded order environments. Exact floating-point
comparison is intentional: both paths use the same values, order and arithmetic.
This is not a tolerance-based comparison against an external broker calculation.

One issue row can contain several flags. Quantities, prices, state, contract,
duplicate identity, missing relation, account, environment and realised P&L each
have named bits. Issues cover the entire retained book even when a filter has no
matching source rows. They are observations, not repair commands. Structurally
invalid or nonfinite data causes capture to fail rather than emitting unsafe text.

Currency totals use matching retained positions and remain separate by currency.
Any whole-book discrepancy withholds these totals. Summation overflow is itself
an issue and also withholds totals. These are gross realised amounts with the
instrument multiplier already applied, excluding commissions, financing and tax.
There is no invented cash, net-P&L, FX conversion or unrealised-price source.

This report cannot detect broker records the workspace never received. It does
not reconcile a broker after reconnect or implement durable financial-session
recovery. The workspace retains at most 128 orders, 128 execution reports and
64 positions. A clean local replay is not proof of release readiness.

## Compose a native panel safely

`UmiGtk4TradingSessionReportCreate` in
`umicom/trading_ui/gtk4/session_report.h` creates explicit filter, refresh and CSV
controls. It borrows the workspace. Call `UmiGtk4TradingSessionReportDetach`
before destroying that workspace, including when another component retains the
widget. Detachment disables callbacks while leaving captured text readable.

The shared trading suite mounts this section alongside its existing executions
and trade-performance panels. A weak observer detaches the section during suite
shutdown or mount destruction. The section survives ordinary body refreshes,
keeping typed filters and the displayed capture. A layout recreation may replace
the section; it is not a persistent named report or a saved financial session.

`UmiTradingSessionReportIsCurrent` checks the original owner identity, revision,
account and environment. A new workspace with the same account and revision is
not the original owner, even if its allocation reuses the same memory address.
A changed revision makes the report stale but does not invalidate its copied
evidence. Native **Copy captured CSV** always exports that evidence, not an
unsubmitted filter edit or a silently refreshed book. A failed refresh retains
the previous report and presents its error.
