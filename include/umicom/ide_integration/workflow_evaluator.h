/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_evaluator.h
 *
 * PURPOSE:
 *   Evaluate authoritative Framework snapshots into explicit workflow gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_WORKFLOW_EVALUATOR_H
#define UMICOM_IDE_INTEGRATION_WORKFLOW_EVALUATOR_H

#include "umicom/ide_integration/active_context.h"
#include "umicom/ide_integration/workflow_report.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ide workflow evaluate operation used by this module and its client
 * applications.
 *
 * Evaluate on the bound services' owning thread using a current context. The
 * test gate reads retained result records without changing them. It does not
 * establish discovery coverage, source-revision identity or latest-session
 * selection; hosts remain responsible for the evidence scope.
 */
UmiStatus umi_ide_workflow_evaluate(
    const UmiIdeIntegrationBindings *bindings,
    const UmiIdeActiveContext *context,
    const UmiIdeWorkflowPolicy *policy,
    UmiIdeWorkflowReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
