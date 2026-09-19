/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/interaction_recording.h
 *
 * PURPOSE:
 *   Record bounded, explicit GUI observations and export reviewable reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_INTERACTION_RECORDING_H
#define UMICOM_UI_INTERACTION_RECORDING_H
#include "umicom/ui/automation.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_RECORDING_MAX_ENTRIES 4096U
#define UMI_UI_RECORDING_DETAIL_CAPACITY 512U

/** The kind of evidence, not a claim that a user operation succeeded. */
typedef enum UmiUiRecordingKind {
    UMI_UI_RECORDING_INPUT = 1,
    UMI_UI_RECORDING_NOTE = 2,
    UMI_UI_RECORDING_SCREENSHOT = 3,
    UMI_UI_RECORDING_AUTOMATION = 4
} UmiUiRecordingKind;

/** One copied observation. Inputs carry no typed text or clipboard contents.
 * elapsedMs is monotonic time since recording began. targetId is a stable
 * control identifier, or a clearly labelled fallback type. detail is an
 * explicit note or explanation. It must never contain credentials.
 * attachment is empty or a simple local PNG filename, never an arbitrary URL.
 * outcomeKnown=0 means input was observed, not that its command succeeded.
 */
typedef struct UmiUiRecordingEntry {
    uint64_t elapsedMs;
    UmiUiRecordingKind kind;
    UmiStatus status;
    int outcomeKnown;
    char targetId[UMI_UI_ID_CAPACITY];
    char action[64];
    char detail[UMI_UI_RECORDING_DETAIL_CAPACITY];
    char attachment[64];
} UmiUiRecordingEntry;

/** Private bounded storage. Call only on its owning thread; it adds no locks. */
typedef struct UmiUiRecording UmiUiRecording;

/** A borrowed output sink; return an error when bytes cannot be written.
 * Bytes need remain valid only for this call. Export stops at the first error.
 * A sink must not reenter, modify or destroy the recording while exporting.
 */
typedef UmiStatus (*UmiUiRecordingWriteFn)(void *context, const char *bytes, size_t length);

/** Create a paused recording. Copies both identifiers. capacity is 1..4096.
 * Strings must be NUL-terminated UTF-8; buildIdentity may be empty when unknown.
 * On failure *outRecording is NULL. See examples/gui_recording/main.c.
 */
UmiStatus UmiUiRecordingCreate(const char *applicationId, const char *buildIdentity,
    size_t capacity, UmiUiRecording **outRecording);
/** Release the recording and its entries; NULL is harmless. */
void UmiUiRecordingDestroy(UmiUiRecording *recording);
/** Start or pause collection without clearing prior observations. Resuming a
 * full recording is rejected; create a fresh session to retain more entries. */
UmiStatus UmiUiRecordingSetActive(UmiUiRecording *recording, int active);
/** Return whether another observation may be appended; NULL returns false. */
int UmiUiRecordingIsActive(const UmiUiRecording *recording);
/** Number of retained entries, including entries from earlier active periods. */
size_t UmiUiRecordingCount(const UmiUiRecording *recording);
/** True after an append reached the capacity limit. No older row is erased. */
int UmiUiRecordingLimitReached(const UmiUiRecording *recording);
/** Validate and copy one event. Decreasing timestamps or malformed fields do
 * not alter existing rows. Capacity exhaustion pauses recording explicitly. */
UmiStatus UmiUiRecordingAppend(UmiUiRecording *recording, const UmiUiRecordingEntry *entry);
/** Copy an entry into caller-owned storage; index starts at zero. */
UmiStatus UmiUiRecordingAt(const UmiUiRecording *recording, size_t index,
    UmiUiRecordingEntry *outEntry);
/** Append an existing automation result, omitting input values and observed
 * text. Imported results are labelled separately from observed human input.
 * The result status is a driver-step outcome, not a domain-state assertion. */
UmiStatus UmiUiRecordingAppendAutomation(UmiUiRecording *recording,
    uint64_t elapsedMs, const UmiUiAutomationStepResult *result);
/** Stream escaped UTF-8 JSON or a self-contained HTML table. These exports do
 * not write files, perform network access or change application state. PNG
 * attachments stay alongside the HTML file. The caller closes its own sink.
 */
UmiStatus UmiUiRecordingWriteJson(const UmiUiRecording *recording,
    UmiUiRecordingWriteFn write, void *context);
/** Stream one already-recorded entry as one JSON line. index is zero-based.
 * This lets an adapter flush each observation to a local session log without
 * rewriting all earlier entries. No input text is added by this operation. */
UmiStatus UmiUiRecordingWriteEntryJson(const UmiUiRecording *recording, size_t index,
    UmiUiRecordingWriteFn write, void *context);
UmiStatus UmiUiRecordingWriteHtml(const UmiUiRecording *recording,
    UmiUiRecordingWriteFn write, void *context);
#ifdef __cplusplus
}
#endif
#endif
