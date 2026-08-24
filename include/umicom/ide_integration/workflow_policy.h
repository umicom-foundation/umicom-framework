/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_policy.h
 *
 * PURPOSE:
 *   Define reusable readiness policy for build/test/package/self-host workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_WORKFLOW_POLICY_H
#define UMICOM_IDE_INTEGRATION_WORKFLOW_POLICY_H
#include "umicom/ide_integration/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeWorkflowPolicy {
    int require_workspace;
    int require_document;
    int allow_dirty_document;
    int require_no_errors;
    int require_no_fatals;
    int require_tests;
    int require_all_tests_passed;
    int require_source_control;
    int require_clean_source_control;
    int require_debugger_idle;
    int require_no_pending_ai_approval;
    int require_ai_idle;
    uint64_t revision;
} UmiIdeWorkflowPolicy;

void umi_ide_workflow_policy_init(UmiIdeWorkflowPolicy *policy);

#ifdef __cplusplus
}
#endif
#endif
