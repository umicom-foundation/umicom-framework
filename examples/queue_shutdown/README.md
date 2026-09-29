# Notes queue shutdown laboratory

The default build compiles the canonical platform subset and retains the previous thread-lifetime regression suite. Run `umicom-queue-shutdown drain` or `cancel`. No data is persisted or sent externally.

See `docs/learning/close-a-background-queue.html` for the complete lesson. The public API is in `platform/task_queue.h` and `platform/threading.h`. The host tools under Applications and umicomOS consume this same source.

This is not the full SDK: it exports only the focused platform and base targets. Install into a dedicated prefix, not over a complete production SDK. Windows execution and GUI integration still need target-platform acceptance.
