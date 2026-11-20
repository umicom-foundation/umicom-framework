# Notes queue shutdown laboratory

![Umicom](../../resources/brand/umicom-logo.svg)

[![Umicom icon](../../resources/brand/umicom-icon.svg)](../../resources/brand/umicom.ico)

The default build compiles the canonical platform subset and retains the previous thread-lifetime regression suite. Run `umicom-queue-shutdown drain` or `cancel`. No data is persisted or sent externally.

See `docs/learning/close-a-background-queue.html` for the complete lesson. The public API is in `platform/task_queue.h` and `platform/threading.h`. The host tools under Applications and umicomOS consume this same source.

This is not the full SDK: it exports only the focused platform and base targets. Install into a dedicated prefix, not over a complete production SDK. Windows execution and GUI integration still need target-platform acceptance.

The public-header client includes two scenarios that both start with one
running callback and one queued task. A drain refuses a late submission and
completes both accepted tasks. Cancellation has its own outcome and counters.
See [Check background work through an installed SDK](../../docs/guides/check-sdk-shutdown.html)
for the commands and the expected evidence.
