/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/views/workflow.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral Workflow Gates view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_VIEW_WORKFLOW_H
#define UMICOM_IDE_INTEGRATION_VIEW_WORKFLOW_H
#include "umicom/ide_integration/platform.h"
#include "umicom/ide_integration/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Initialise ide workflow view from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_workflow_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
