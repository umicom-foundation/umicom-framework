/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/autosave.h
 *
 * PURPOSE:
 *   Schedule bounded, debounce-aware autosave without owning a timer or event
 *   loop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_AUTOSAVE_H
#define UMICOM_WORKBENCH_DESIGNER_AUTOSAVE_H

#include "umicom/workbench_designer/persistence_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer autosave policy data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerAutosavePolicy {
    bool enabled;
    uint64_t debounce_ms;
    uint64_t maximum_delay_ms;
    uint64_t retry_delay_ms;
} UmiWorkbenchDesignerAutosavePolicy;

/**
 * Represent the workbench designer autosave data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerAutosave {
    UmiWorkbenchDesignerAutosavePolicy policy;
    uint64_t dirty_since_ms;
    uint64_t last_change_ms;
    uint64_t next_due_ms;
    uint64_t last_attempt_ms;
    uint64_t last_success_ms;
    uint32_t consecutive_failures;
    bool dirty;
    bool in_flight;
    uint64_t revision;
} UmiWorkbenchDesignerAutosave;

/**
 * Provide the workbench designer autosave policy default operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerAutosavePolicy umi_workbench_designer_autosave_policy_default(void);
/**
 * Initialise workbench designer autosave from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_autosave_init(UmiWorkbenchDesignerAutosave *autosave, const UmiWorkbenchDesignerAutosavePolicy *policy);
/**
 * Provide the workbench designer autosave mark dirty operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_autosave_mark_dirty(UmiWorkbenchDesignerAutosave *autosave, uint64_t timestamp_ms);
/**
 * Provide the workbench designer autosave due operation used by this module and its client
 * applications.
 */
bool umi_workbench_designer_autosave_due(const UmiWorkbenchDesignerAutosave *autosave, uint64_t now_ms);
/**
 * Provide the workbench designer autosave begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_autosave_begin(UmiWorkbenchDesignerAutosave *autosave, uint64_t now_ms);
/**
 * Provide the workbench designer autosave complete operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_autosave_complete(UmiWorkbenchDesignerAutosave *autosave, UmiStatus status, uint64_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
