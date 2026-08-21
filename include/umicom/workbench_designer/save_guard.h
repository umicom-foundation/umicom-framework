/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/save_guard.h
 *
 * PURPOSE:
 *   Evaluate close, switch and application-shutdown decisions for dirty, saving,
 *   conflicted or failed layout sessions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SAVE_GUARD_H
#define UMICOM_WORKBENCH_DESIGNER_SAVE_GUARD_H

#include "umicom/workbench_designer/status.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerCloseDecision {
    UMI_WORKBENCH_DESIGNER_CLOSE_ALLOWED = 1,
    UMI_WORKBENCH_DESIGNER_CLOSE_PROMPT_SAVE = 2,
    UMI_WORKBENCH_DESIGNER_CLOSE_WAIT_FOR_SAVE = 3,
    UMI_WORKBENCH_DESIGNER_CLOSE_RESOLVE_CONFLICT = 4,
    UMI_WORKBENCH_DESIGNER_CLOSE_CONFIRM_DISCARD = 5
} UmiWorkbenchDesignerCloseDecision;

typedef struct UmiWorkbenchDesignerSaveGuardResult {
    UmiWorkbenchDesignerCloseDecision decision;
    bool can_close_immediately;
    bool can_discard;
    bool must_preserve_recovery;
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
} UmiWorkbenchDesignerSaveGuardResult;

UmiStatus umi_workbench_designer_save_guard_evaluate(
    const UmiWorkbenchDesignerStatusModel *status,
    bool save_supported,
    bool discard_allowed,
    UmiWorkbenchDesignerSaveGuardResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
