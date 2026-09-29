# Task-queue native shutdown contract

This extends `Umicom::platform`; there is no new queue engine. The existing blocking shutdown, cooperative cancellation and reference-counted task records are retained. The immediately preceding thread-control-block lifetime fix is a prerequisite included in the verified source baseline.

## Operations

`UmiThreadCanTryJoin` advertises a native build capability. `UmiThreadTryJoin` returns BUSY while native teardown is incomplete, OK after consuming the join obligation, INVALID_ARGUMENT for a null/already-joined handle, INVALID_STATE for self-join, INTERNAL_ERROR on native join failure and NOT_IMPLEMENTED on unsupported profiles. OK does not imply a callback returned an integer; use `UmiThreadTryGetExitCode` independently. Owner-side operations require a live owner reference and serialisation.

`UmiTaskQueueCaptureShutdown` copies phase and counters coherently; an error preserves output. `UmiTaskQueueTryFinishShutdown` requires stopped admission, retains partial reaping progress, and does not wait for live workers. The joining flag shares exclusion with the legacy blocking path. It reaps later slots even when an earlier slot is busy. It reports no successful completion with stranded running/queued accounting.

`UmiTaskQueueReleaseStopped` requires STOPPED and exclusive lifecycle ownership. It clears the queue owner only after all native-handle releases succeed. A failed release can leave some handles already released, but their slots are null and retry does not close them twice. Other queue users must already be excluded before release. The queue itself is not reference-counted.

## Boundaries

Queue mutex acquisition, cancellation of up to 65,536 pending tasks and native scheduling are not hard real-time operations. Native polling supports Windows and non-Android Linux. On other profiles no blocking call is substituted. The test-only UMICOM_THREAD_DISABLE_TRY_JOIN definition exercises that unsupported boundary.

Callers remain responsible for payload and progress-callback lifetime. Task callbacks must return normally through the runner; asynchronous thread termination, longjmp and pthread_exit from a queued task remain unsupported. Do not enable real trading, payment or physical-device actions in these tests.

## Roadmap

Continues roadmap 44 (lifetime/cancellation/concurrency). Does not complete roadmap 43 (Windows/GTK qualification), retrofit application close handlers, change the kernel, or implement Umicoin.
