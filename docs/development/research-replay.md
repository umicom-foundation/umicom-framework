# Research replay: ownership and integration

The research_replay target consumes canonical UmiQuote and UmiInstrument records and the existing completed-trade accumulator. It does not replace market_tape, the strategy service, replay clock, broker interface, Data Server or order authority. It adds the missing explicit connection between a trusted strategy decision and a later executable observation for this bounded quote-only model.

## Lifetime and state

Create validates all inputs and copies them. State is READY, RUNNING, COMPLETED, CANCELLED or FAILED. Step validates tentative numeric results, invokes the trusted callback and publishes one trace row and model only on success. Callback failure is terminal. Run is a budgeted convenience over Step. No method changes an input dataset, writes a database or creates a thread. The caller must serialise access and retain its user-data lifetime.

Cancel is an explicit between-call operation. A completed or cancelled run is immutable. Recreate a run and reset callback state to replay from the beginning. TraceAt exposes only committed rows and copies them. Snapshot copies state. A future query/seek/checkpoint service must preserve strategy-state identity; no seek is silently emulated here.

## Execution ordering

A pending target can fill only on an observation with a later array index and at or after its deadline. Buy uses ask, sell uses bid. An adverse basis-point adjustment changes execution price. Displayed size must cover the whole signed change; a reversal uses twice the position size. After a fill the callback sees the new position. HOLD retains the pending target; requesting a target replaces its deadline or cancels it when already at that position.

The canonical backtest aggregate records each CLOSED position, with both commissions and the already adjusted execution prices. The replay's all-paid-fees and open-position mark cover the entry that has not yet closed. No final liquidation is invented. The principal independent comparison uses 4,000 quotes and 1,000 completed trades against a long-double recurrence, plus prefix/no-future and step/run comparisons.

## CMake and ABI

UmicomResearchReplayIntegration.cmake defers composition until canonical trading, trading_ui and native_launcher targets exist. Minimal unrelated hosts do not receive fabricated targets. The new public API uses C structures, opaque handles, status codes and borrowed callbacks. Existing snake-case backtest names remain; new operations use UpperCamelCase.

The isolated example builds the same canonical source files with a deliberately small exported SDK. The client subdirectory finds that SDK rather than compiling Framework private sources. Do not install the focused subset over a complete SDK. Existing application modules receive no source changes; adding graphical or live-provider integration remains subsequent work.

## Source preservation

The original backtest_engine.c bodies remain as a disabled reference before the new definitions. The existing Desktop System CMake helper only gains an appended include. No old public structure or file header is removed. The native tools do not invoke Python, shell scripts or a compiler at runtime.
