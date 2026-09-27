# Market tape: observation ownership and adapter contract

Umicom Foundation — Sammy Hegab — MIT

## Responsibility

`Umicom::market_tape` is a toolkit-neutral observation owner, not an order book,
execution service, broker adapter or durable market-data store. It consumes the
existing `UmiInstrument`, `UmiQuote`, `UmiTradeTick`, `UmiMarketDepth` and `UmiBar`
records. The wrapper adds transport generation, contiguous per-instrument sequence
and receive time; it does not redefine money or instrument contracts.

The Master Controller selects services. An adapter's Slave Controller translates
its provider's sequencing and resynchronisation rules before submitting a packet.
A view reads one owned snapshot, never separate instrument-local globals.

## Lifecycle

1. Create with a fixed interval (1..86400000 ms) and positive stale threshold.
2. Register up to 16 instruments before submitting observations for them.
3. Begin a strictly newer generation with an explicit positive first sequence.
4. Apply canonical packets. No allocation or callbacks occur on this path.
5. Select an instrument and read a complete caller-owned snapshot.
6. Disconnect when continuity no longer applies.
7. Stop/join every client before destroying the tape.

The tape uses the canonical Framework mutex. Applying, selecting and reading are
serialised. Caller input/output memory must remain valid and unmodified during a
call. Mutex ownership cannot make a destroyed object safe. The practice controller
and GTK adapter are single-owner/main-context objects, not concurrent adapters.

## Sequence and rejection

Sequence numbers are contiguous PER INSTRUMENT within a generation. A venue-wide
sequence cannot be filtered by instrument and passed directly to this API. An
adapter must establish a valid per-instrument contract or use a separate channel
continuity service before publishing observations.

Duplicate/old packets, unknown identities, malformed data, stale generations and
arithmetic overflow are refused without a model change. A valid higher sequence
latches a gap but does not apply its payload. Subsequent packets cannot clear that
latch. Begin explicitly clears only that instrument's history and advances its
generation. This starts new evidence; it does not repair omitted historical data.
An accepted `UINT64_MAX` sequence closes the usable sequence range rather than
wrapping it. Model revision exhaustion also refuses further mutations.

Packets must reproduce all registered instrument fields. IDs, symbols and venues
are bounded non-empty printable ASCII tokens; currency is three uppercase ASCII
letters. Multiplier is finite and positive. This is structural identity checking,
not exchange security-master or expiry-calendar verification.

## Time and freshness

The core accepts non-negative normalised millisecond timestamps. Event time must
not exceed receipt time. Receipt time is nondecreasing per instrument, and event
time is nondecreasing within each quote/trade/depth lane. Equal times are allowed.
Read time must be at least every accepted receipt time. Reads never mutate time.

`revision` identifies model changes; `observedAtMs` identifies the freshness
projection. Do not cache freshness by revision alone. A current arrival stream
can contain a delayed source quote. Always inspect quoteFresh/tradeFresh/depthFresh
separately. A new trade never refreshes an old quote or old depth. Missing and
one-sided depth never count as fresh two-sided depth. Gaps, exhaustion and
connections that are closed make all lane freshness false.

The shared legacy quality scorer now checks signed ordering before calculating
an unsigned distance, avoiding signed overflow across `INT64_MIN`/`INT64_MAX`.
Its original implementation is retained in an explained disabled block. Its
ordinary zero-quality sentinel and existing public function name remain.

## Quantities, depth and bars

This tape profile requires finite positive trade prices and quantities, positive
quote bid/ask prices, and nonnegative quote sizes. Depth is a full replacement
snapshot, strictly descending bids and ascending asks, with finite positive
levels/sizes and no crossed top of book. Locked bid/ask is accepted. Empty and
one-sided snapshots are retained but not labelled fresh two-sided liquidity.
Depth deltas, negative-price products, busts and corrections are unsupported.

The ring retains 64 trades and 64 bars per instrument, newest at the end of the
snapshot. Bars use the canonical `UmiBar` record and fixed half-open time buckets.
No empty bar is fabricated. Trade event regression and overflowing bucket end or
volume are refused BEFORE sequence/ring mutation. The last bar may still be
forming; there is no independent finalisation watermark or trading-session clock.
Retirement counters saturate rather than wrap. They are capacity evictions, not
lost feed messages. Begin clears the selected instrument's rings and counters.

Double-valued price/volume analytics follow the existing trading vocabulary.
They are not exact decimal settlement values, weighted-average valuation evidence
or a substitute for the money/ledger services.

## Presentation and product integration

`Umicom::market_tape_gtk4` supplies a fictional two-instrument practice window.
The chart, watchlist, time-and-sales, depth and bar tabs read a single snapshot.
The chart places bars at their actual bucket times. No event timer, file loading,
network transport, disk persistence, order submission or account access is added.

Object-bound signals and the drawing callback's weak window reference prevent a
retained child from reaching freed view state. A destroyed window marks its view
closed before final release. The root check also refuses an unparented retained
button. GTK operations are confined to the main context. Real GTK lifecycle,
keyboard, accessibility and Windows DPI tests remain required on the target host.

Trader's source addition only wraps its unparented existing content with an entry
button. It never accesses private trading state. Existing layouts, simulation,
checkpoint storage and controls remain owned by their established paths.

The root helper defers attachment until canonical trading and platform targets
exist. Unrelated minimal hosts retain their previous dependency set. GTK is added
only when the real UI target and GTK >= 4.10 development package are present.
The optional installed GTK dependency fragment is absent from core-only SDKs.

## Tests and examples

`examples/market_tape` compiles the actual canonical trading quality/quote and
threading sources as a small SDK. It is not the complete Framework build.
`lesson.c` demonstrates canonical trade packets and OHLCV. `main.c` exercises a
known missing sequence and new epoch. The regression suite covers exact time
boundaries, independent lanes, sequencing, depth ordering, retention, failed
allocation, overflow refusal, copied-snapshot lifetime and concurrent snapshots.
The public step-by-step guide is `docs/learning/market-tape.html`.
