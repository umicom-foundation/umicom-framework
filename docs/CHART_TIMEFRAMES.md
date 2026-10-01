# Build chart timeframes from retained observations

A timeframe groups observations into candles of a chosen duration. Framework
provides **Source**, **1 minute**, **5 minutes**, **15 minutes**, **1 hour**,
**4 hours** and **1 day (UTC)**. Source displays each retained provider bar.
The other choices group those records into fixed UTC intervals. UTC is the
shared time reference used by these charts, independent of the computer's clock zone.

## Try the native control

1. Open Trader's Chart tool and select an instrument with retained price data.
2. Choose **5m** in **Timeframe**. The status names the interval and separates
   displayed candles from the number of underlying source bars.
3. Use zoom, earlier/later and Fit. Navigation counts displayed candles. Fit
   resets zoom and historical pinning while keeping the chosen timeframe.
4. Add an SMA or EMA. Its period now counts candles of the selected interval.
   For example, a period of 20 on a 5-minute chart uses 20 observed 5-minute
   candles. Missing intervals are not counted as invented candles.
5. Choose **Source** to inspect the original records again. Changing timeframe
   does not change orders, the order draft, or stored drawing coordinates.

## Understand what the chart knows

Each aggregate uses the first open, greatest high, least low, last close and
sum of observed volume. For five one-minute bars starting at 10:00 through
10:04 UTC, the five-minute candle starts at 10:00. Daily candles begin at UTC
midnight. They do not follow an exchange's trading session or daylight-saving
changes. Weekly, monthly, custom and session-based intervals are not supplied.

The chart works with the records already retained in the workspace. Selecting
a larger interval does not download older history, increase retention, fill
missing intervals or certify that any candle is complete. Both the oldest
and newest buckets can contain only part of their interval. Simulator event
bars can represent individual observations, not full minute histories.
For that reason, volume is the sum of supplied observations, not a promise
of complete market volume. An SMA with fewer candles than its period has no
line until enough candles are retained.

An observation that crosses a requested bucket boundary cannot be split
reliably. Selection returns UNAVAILABLE and keeps the previous setting.
Choose Source or a compatible larger interval. A provider end exactly at
the next bucket boundary is accepted as an exclusive endpoint. Overlapping
observations are rejected for aggregation; Source preserves them as provided.
Invalid prices, duplicate or unordered start times and volume overflow also
reject the projection. No partially built output is published.

## Keep and restore a timeframe

1. Choose the interval, zoom and historical position you want to keep.
2. Select **Save chart**. This explicitly saves the current instrument's view
   and drawings in the bound chart storage. Selecting an interval alone does
   not write storage.
3. Later choose **Preview saved chart**. Read both saved and current timeframes
   in Saved chart details before selecting **Restore preview**.
4. If the current timeframe changes after preview, preview again. The earlier
   review no longer describes the current view and cannot be restored.

Timeframes are independent per instrument in the workspace. The global study
choice and period remain outside chart checkpoint persistence. Restoring a
saved view preserves its interval even if the newly retained provider data
cannot support it; the scene is then unavailable. Source can recover the
view when the provider records themselves are valid.

Old nine-field view metadata loads with interval zero, meaning Source. New
saves use view version 2 and an explicit interval. Older application versions
cannot read this new view metadata and may offer an older recovery copy.
Back up the profile database before returning to an older version. The outer
checkpoint still uses the same transactional storage and digest checks.
Read [chart checkpoints](CHART_CHECKPOINTS.md) for storage ownership and recovery.

## Use the shared C interface

1. Supply ordered `UmiChartObservedBar` values, containing a candle and its
   observed end timestamp, to `UmiChartTimeframeAggregate`. Empty input is valid.
2. Allocate output capacity in candles. Input is bounded by
   `UMI_CHART_MAX_POINTS`; failures leave the output and summary unchanged.
3. For a trading workspace, call `UmiTradingWorkspaceSetChartTimeframe` with
   the selected instrument's identity. It validates compatibility before
   publishing the new interval. Stale instrument identities are rejected.
4. Call `UmiTradingWorkspaceBuildChartCandles` for the shared projection.
   Rendering and navigation use this same function. Provider records stay owned
   by the workspace; returned candles are caller-owned copies.
5. Initialize `UmiChartNavigation` to zero before setting individual fields.
   The appended `interval_ms` field and scene summary extension require
   rebuilding libraries and consumers together. Do not mix old prebuilt structs
   with this interface. The lower-level navigation setter validates interval
   membership but does not project provider data; use the timeframe setter for
   an interactive interval change.

Use the workspace's owning UI thread. Standalone native hosts must detach
the chart before destroying its context or borrowed persistence service.
`UmiChartTimeframeFormatUtc` formats signed Unix milliseconds without process
timezone settings. It uses Gregorian dates and minute precision, including
dates before 1970.
