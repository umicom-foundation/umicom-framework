/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/lock.h
 *
 * PURPOSE:
 *   Explain layout locking and whether a requested designer action may mutate
 *   the document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_LOCK_H
#define UMICOM_WORKBENCH_DESIGNER_LOCK_H

#include "umicom/workbench_designer/command.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerLockState {
    bool locked;
    bool built_in;
    bool read_only;
    bool administrator_override;
    char reason[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerLockState;

void umi_workbench_designer_lock_state_build(UmiWorkbenchDesignerLockState *state, const UmiWorkbenchLayoutDocument *document, bool administrator_override);
bool umi_workbench_designer_lock_allows(const UmiWorkbenchDesignerLockState *state, UmiWorkbenchDesignerCommandKind command_kind);

#ifdef __cplusplus
}
#endif

#endif
