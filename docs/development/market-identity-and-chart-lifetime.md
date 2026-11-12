![Umicom](../../resources/brand/umicom-logo.svg)

# Keep a chart view attached to its instrument

A market workspace stores several related arrays: registered instruments, current market records, retained candles and chart navigation. An array position is temporary. The instrument identifier is the identity that the user expects to keep.

Framework owns this relationship so Studio and Trader can present the same state. The service does not open a broker connection, trade an instrument or prove that a native chart has rendered correctly.

## Admit the watchlist before finding a record

1. Initialize a complete **UmiWatchlist** with **umi_watchlist_init**.
2. Add instruments through **umi_watchlist_add**. Supply a valid identifier, symbol, venue, currency and finite positive multiplier.
3. Use **umi_watchlist_find** to obtain an index. The service captures the bounded query and validates every active record before returning a match.
4. Use the index only while the same owner is alive, unchanged and accessed serially. Copy the instrument if another operation may edit the list.
5. Treat every non-OK status as a refusal. The new lookup leaves its output unchanged on all refusals.

~~~c
size_t index = SIZE_MAX;
UmiStatus status = umi_watchlist_find(watchlist, "MARKET.A", &index);
if (status == UMI_STATUS_OK) {
    /* A value copy no longer borrows the array slot. */
    UmiInstrument selected = watchlist->instruments[index];
    (void)selected;
}
~~~

The query must be readable through its terminator or **UMI_FINANCE_ID_CAPACITY** bytes. A missing terminator returns **CAPACITY_EXCEEDED**; an empty query returns **INVALID_ARGUMENT**. This is a byte limit, not permission to read an arbitrary pointer.

A count above capacity retains the existing **INVALID_ARGUMENT** result. A malformed active instrument or repeated active identity returns **INVALID_STATE**, even when an earlier row matches. Unused array slots are ignored. In a valid full list, adding an existing identity returns **ALREADY_EXISTS** and adding a new identity returns **CAPACITY_EXCEEDED**.

Output storage must be writable and aligned, and must not overlap the watchlist. Query and output may share a separate allocation because query capture happens first. Neither the validation service nor an index supplies a lock.

## Move related state together

Imagine three rows:

| Slot | Instrument | Visible bars | Pin | Interval |
| --- | --- | --- | --- | --- |
| 0 | MARKET.A | 5 | 1000 ms | 1 minute |
| 1 | MARKET.B | 7 | 2000 ms | 5 minutes |
| 2 | MARKET.C | 9 | 3000 ms | 15 minutes |

Removing MARKET.B moves MARKET.C from slot 2 to slot 1. Its navigation must move with it. Moving only prices and candle history leaves C using B's view, which is a valid-looking but incorrect association.

The workspace now compacts navigation with the other per-market arrays and clears the retired navigation slot. Adding an instrument initializes its view again. A zero view follows the newest candles, fits retained bars and uses the provider's source interval. Re-adding the same identifier starts with that default navigation too.

~~~c
UmiChartNavigation view = {
    .visible_bars = 9U,
    .anchor_ms = 3000,
    .pinned = 1,
    .interval_ms = 900000U
};
UmiStatus status = umi_trading_workspace_select_instrument(workspace, "MARKET.C");
if (status == UMI_STATUS_OK)
    status = UmiTradingWorkspaceSetChartNavigation(workspace, "MARKET.C", &view);
/* Check status before presenting the requested view as accepted. */
~~~

Navigation edits require the named instrument still to be selected. A valid query naming another selection returns **INVALID_STATE**. An unterminated chart query returns **CAPACITY_EXCEEDED** without changing the output or owner. Existing market selection and removal APIs retain their no-match mapping, so an unadmitted lookup returns **NOT_FOUND** there.

Removing an instrument is not a general chart-document purge. Drawing, document, undo and persistence retention have their own owners. The navigation repair does not claim to delete those records or implement a broker unsubscribe.

## Qualify the behavior

Run each command separately from the Framework source root and stop on errors. Set up the repository's documented native toolchain first.

~~~powershell
cmake -S . -B build/market-lifecycle -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/market-lifecycle --target umicom-trading-watchlist_admission-test umicom-trading-workspace_market_lifecycle-test
ctest --test-dir build/market-lifecycle -R "^framework[.]trading[.]admission[.](watchlist_admission|workspace_market_lifecycle)$" --output-on-failure
~~~

The watchlist regression covers malformed later rows, duplicate retained identities, invalid counts, full-list precedence, maximum-length identifiers, borrowed input and overlapping output refusal. The workspace regression removes first, middle, last and sole instruments, then reuses slots and checks every surviving view. It also checks that refused queries preserve revision, selection and navigation.

These are authored memory-only checks. Expected success is not a report of execution. Run the existing trading, chart navigation, document restore and application suites as part of broader qualification. A passing core test alone does not establish GTK behavior, native font rendering, broker connectivity or live trading.

## Exercise: distinguish identity from position

Draw the three-row table on paper and remove the first row. Write the new position of every surviving instrument. Repeat for its candles, navigation and watchlist entry. If any array uses a different move, explain what the user would see.

Then extend a controlled fixture with a malformed second instrument and search for the first. Expect **INVALID_STATE** and an unchanged output index. This checks a different contract: finding one match must not hide invalid retained state later in the owner.
