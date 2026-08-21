/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/recovery_bridge.h
 *
 * PURPOSE:
 *   Compare saved, current and recovery documents and prepare an explicit
 *   recovery proposal without mutating authoritative state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_RECOVERY_BRIDGE_H
#define UMICOM_WORKBENCH_DESIGNER_RECOVERY_BRIDGE_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/diff.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerRecoveryChoice {
    UMI_WORKBENCH_DESIGNER_RECOVERY_KEEP_CURRENT = 1,
    UMI_WORKBENCH_DESIGNER_RECOVERY_USE_RECOVERED = 2,
    UMI_WORKBENCH_DESIGNER_RECOVERY_USE_SAVED = 3
} UmiWorkbenchDesignerRecoveryChoice;

typedef struct UmiWorkbenchDesignerRecoveryProposal {
    bool recovery_available;
    bool current_dirty;
    bool recovered_newer;
    bool saved_newer;
    UmiWorkbenchLayoutDiff current_to_recovered;
    UmiWorkbenchLayoutDiff saved_to_recovered;
    UmiWorkbenchDesignerRecoveryChoice recommended_choice;
    char summary[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerRecoveryProposal;

void umi_workbench_designer_recovery_proposal_init(UmiWorkbenchDesignerRecoveryProposal *proposal);
UmiStatus umi_workbench_designer_recovery_proposal_build(UmiWorkbenchDesignerRecoveryProposal *proposal, const UmiWorkbenchLayoutDocument *saved, const UmiWorkbenchLayoutDocument *current, const UmiWorkbenchLayoutDocument *recovered);
UmiStatus umi_workbench_designer_recovery_apply(const UmiWorkbenchDesignerRecoveryProposal *proposal, UmiWorkbenchDesignerRecoveryChoice choice, const UmiWorkbenchLayoutDocument *saved, const UmiWorkbenchLayoutDocument *current, const UmiWorkbenchLayoutDocument *recovered, UmiWorkbenchLayoutDocument *out_document);

#ifdef __cplusplus
}
#endif

#endif
