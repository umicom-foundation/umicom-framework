/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/watch_edit.h
 * PURPOSE: Own explicit watch edits independently of native UI and expression evaluation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_WATCH_EDIT_H
#define UMICOM_DEBUG_WATCH_EDIT_H
#include "umicom/debug/selection.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDebugWatchSettings {
    int enabled;
    char expression[1024];
} UmiDebugWatchSettings;
typedef struct UmiDebugWatchEdit UmiDebugWatchEdit;
typedef struct UmiDebugWatchChange {
    UmiDebugWatchSnapshot before, after;
    bool changed, removed;
} UmiDebugWatchChange;
/** Copy a nonempty expression of at most 1023 bytes; enabled is exactly 0/1.
 * No expression is evaluated. Failure leaves out unchanged. */
UmiStatus UmiDebugWatchSettingsInit(UmiDebugWatchSettings *out, int enabled, const char *expression);
/** Capture owned row evidence. On failure *out is NULL. A capture can outlive
 * its workspace, but actions require the same live owner. Serialize access. */
UmiStatus UmiDebugWatchEditCapture(UmiDebugWorkspace *workspace, size_t index, UmiDebugWatchEdit **out);
void UmiDebugWatchEditDestroy(UmiDebugWatchEdit *edit);
UmiStatus UmiDebugWatchEditRead(const UmiDebugWatchEdit *edit, UmiDebugWatchSnapshot *out);
/** Watch, session and configuration changes invalidate the capture (BUSY).
 * Selecting a different frame does not invalidate a local property edit. */
UmiStatus UmiDebugWatchEditValidate(UmiDebugWorkspace *workspace, const UmiDebugWatchEdit *edit);
/** No-op keeps revisions/value. A changed expression or enabled flag clears
 * the old evaluation value/type/session and marks it not evaluated (valid=0).
 * Remove explicitly removes the selected watch. Neither operation contacts an
 * adapter or disk. out is required, zero on failure; inputs must not overlap it. */
UmiStatus UmiDebugWatchEditApply(UmiDebugWorkspace *workspace, const UmiDebugWatchEdit *edit,
    const UmiDebugWatchSettings *settings, UmiDebugWatchChange *out);
UmiStatus UmiDebugWatchEditRemove(UmiDebugWorkspace *workspace, const UmiDebugWatchEdit *edit,
    UmiDebugWatchChange *out);
#ifdef __cplusplus
}
#endif
#endif
