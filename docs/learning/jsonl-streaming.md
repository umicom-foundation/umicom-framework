![Umicom](../../resources/brand/umicom-logo.svg)

# Visit a dataset without retaining the whole file

A stream supplies successive pieces of a file. A chunk is an I/O unit, not a JSON record: a UTF-8 character, JSON escape or CRLF delimiter can cross a chunk boundary. The Framework streaming visitor assembles complete physical lines before calling the existing strict pair parser.

Include `umicom/ai/training/jsonl_stream.h` and link `Umicom::ai`. Call `UmiAiTrainingJsonlVisitFile` with an ordinary absolute local path, optional cancellation token, callback, callback context and separate report. The selected file is read through the existing platform owner. Its final leaf must be a regular file rather than a link or device; trusted parent aliases may be followed. This is not a confinement boundary or immutable snapshot.

1. Keep a provisional result in your callback context.
2. In each callback, consume the borrowed pair immediately or copy only the fields you need into explicitly bounded storage.
3. Return a non-OK status to stop. Completed earlier callbacks are not undone.
4. After the visit returns, require OK and COMPLETE before publishing the provisional result.
5. On failure, inspect the report as progress evidence. Do not describe the accepted prefix as a complete smaller corpus.

The stream owns one 32,769-byte line allocation. One extra byte permits a final CR or the CR of a CRLF delimiter at the exact line limit. The platform adds a 64 KiB stack buffer and the existing bounded JSON parser has its own temporary allocations. There is no full-corpus allocation in this service; this is not a claim that the process uses only 32 KiB or allocates nothing. The line allocation and temporary pair are cleared before release. OS caches and caller copies are outside that clearing contract.

Existing limits remain: 16 MiB per file, 4,096 rows, 32,768 JSON bytes per physical line, 4,096 decoded UTF-8 bytes per prompt/completion and 128 bytes per source ID. File admission rejects an oversized file before callbacks. A bounded stream is not automatically an unlimited-dataset trainer.

RECORD and VISITOR report the one-based failing line. INPUT reports file admission, read, identity or close failure with failedLine zero, and may retain accepted callback progress. Pre-I/O and final cancellation also use INPUT. COMPLETE is set only on whole-operation success. Empty input remains INVALID_ARGUMENT. consumedBytes stops after the last accepted record including its delimiter; the underlying file reader may have fetched further bytes.

The call is synchronous. Put it on a worker in an interactive application. Cancellation is checked before I/O, between chunks/records and before final success, but does not interrupt a blocking native read or a callback. Keep path, token, context and report alive throughout the call. Do not invoke UI mutation from a worker unless its UI owner explicitly supports that operation.

The existing memory visitor and owned-file summary/context APIs remain available unchanged. Use those when you already own bounded input or need their exact-source aggregate contract. The new visitor deliberately does not retain every source ID, deduplicate records, split datasets, tokenize a pretrained model or train weights.

## Try the SDK consumer

The `examples/training_data` project includes `umicom-sdk-training-stream`. Its README describes selecting a matching installed SDK and building the examples. Pass an absolute JSONL file path. The consumer prints aggregate counts only after successful completion; it withholds provisional counters if a later record fails. It uses Framework's native argument capture for Unicode Windows paths.

The focused tests exercise actual native file reads, CRLF and UTF-8/escape splits at the 64 KiB boundary, exact line/row limits, cancellation, callback rejection and allocation refusal. The allocation observer covers the stream's own buffer and its clearing/release, not all operating-system or parser allocations. Installed-SDK, application integration, crash recovery and platform-specific late I/O failures require their own qualification.
