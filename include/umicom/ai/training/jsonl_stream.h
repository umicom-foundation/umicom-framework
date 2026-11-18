/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/jsonl_stream.h
 *
 * PURPOSE:
 *   Visit a selected local teaching dataset with bounded input retention and explicit callback progress.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_JSONL_STREAM_H
#define UMICOM_AI_TRAINING_JSONL_STREAM_H
#include "umicom/ai/training/jsonl_pairs.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Stream an ordinary absolute UTF-8 regular-file path through the canonical
 * platform input owner. The final leaf cannot be a link/device; trusted parent
 * aliases may be followed. Same-handle metadata checks are not an immutable
 * snapshot, directory confinement or a security sandbox.
 *
 * Reuse the JSONL schema, byte/row caps and LF/CRLF/final-CR rules in
 * jsonl_pairs.h. Input retention is one fixed PAIR_LINE_BYTES+1 owned buffer,
 * plus the platform reader's 64 KiB stack chunk and the bounded record parser.
 * The line buffer and temporary pair are cleared before release; this is not
 * erasure of OS caches or caller copies. No full-corpus allocation is made.
 *
 * The synchronous visitor borrows each pair until it returns; it must not
 * retain the pointer or change report/token lifetime. Prior callback effects
 * are NOT rolled back on later record, read, metadata or close failure.
 * acceptedRows and consumedBytes describe accepted rows, not bytes fetched
 * ahead by the platform. RECORD/VISITOR failures identify the one-based line.
 * File admission/read/identity/close and pre-I/O or final cancellation use
 * INPUT with failedLine zero; accepted progress is preserved if already seen.
 * Only OK with COMPLETE means the entire visit succeeded. Empty input is
 * INVALID_ARGUMENT, matching the existing memory visitor.
 *
 * Cancellation is checked before I/O, between chunks/records and before final
 * success. Blocking native reads and caller callbacks are not interruptible.
 * Use a worker thread. Keep path, token, report and context valid through return.
 * The report is initialized on every call when nonnull; it must not alias
 * callback state. This operation does not write files, train or load a model.
 */
UmiStatus UmiAiTrainingJsonlVisitFile(const char *absolutePath,
    const UmiCancellationToken *cancel,UmiAiTrainingPairVisitor visitor,
    void *context,UmiAiTrainingJsonlReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
