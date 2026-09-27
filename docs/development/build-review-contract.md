# Build review contract

Sammy Hegab, Umicom Foundation — MIT

The Master Controller retains the original operation owner. The build Slave Controller owns execution; the review is a read-only consumer, not another runner or store.

`UmiBuildHistoryCopyRecent` copies the newest retained records in chronological order while holding the existing mutex once. The caller keeps history alive during the call and supplies independent output storage. The function reports omitted records still in the ring, not lifetime eviction totals.

`UmiBuildReviewCapture` validates fixed strings, enum ranges, diagnostic counts and positions before publishing an opaque owned snapshot. It allocates at most 16 `UmiBuildResult` records; the GUI asks for eight. Destruction cannot race readers. Filters remain immutable for a call. Failed construction returns a null handle.

`UmiBuildReviewFromResult` copies one caller-owned immutable result. `UmiBuildReviewImportLog` copies up to 65,535 NUL-free bytes and uses the existing build parser. An imported log always has an unrecorded outcome. A malformed or dropped line does not become a fabricated error or successful operation.

The existing result ABI is not expanded. The review cannot recover a source revision, original working directory or output-truncation fact that the producer did not retain. It explicitly reports those fields unavailable/unknown. Timing is the producer's recorded timing, not an independently measured timestamp. Filter totals do not rewrite the outcome.

Rendering allocates a complete bounded report in two passes over immutable storage. Control bytes, invalid UTF-8 and specified bidi-format characters are escaped for display; raw bytes remain in the model. Reports may contain sensitive source or command text. No automatic upload, file export, navigation or command invocation exists.

The canonical parser in `src/diagnostics/compiler_parser.c` remains the grammar authority. The smaller legacy compiler ABI returns capacity errors for fields or positions that cannot fit. Existing ABI layouts and APIs remain; the superseded implementation is in an explained `#if 0` block.

The production GTK adapter is attached to the existing `umicom_ui_gtk4` target, which links the core review. It owns a snapshot, never a Studio pointer. Signal data promotes a weak window reference and checks widget rooting before accessing view state. `GtkDropDown` takes ownership of its model. Close and reopen to refresh; there is no polling thread. GUI capture and rendering are bounded synchronous work and require target-platform responsiveness tests.

The Framework integration helper defers target composition until the owning directory has declared its dependencies. It does not add subdirectories during deferred execution. The standalone example builds exact canonical dependency sources, not a fake platform; its exported SDK is intentionally a subset, not the complete Framework installation. Do not mix its exported targets with a complete Framework SDK in one consumer.

Studio adds a single build-panel action through its existing weak-owner action binder. Existing `src/app/build.c`, editor controls, Test Explorer and source navigation are untouched. Only operations already recorded in this build service appear; absence is not proof of non-execution.
