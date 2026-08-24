/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/views/active_context.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral Active IDE Context view.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_VIEW_ACTIVE_CONTEXT_H
#define UMICOM_IDE_INTEGRATION_VIEW_ACTIVE_CONTEXT_H
#include "umicom/ide_integration/platform.h"
#include "umicom/ide_integration/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_ide_active_context_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
