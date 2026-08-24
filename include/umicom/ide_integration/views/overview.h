/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/views/overview.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral IDE Integration view.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_VIEW_OVERVIEW_H
#define UMICOM_IDE_INTEGRATION_VIEW_OVERVIEW_H
#include "umicom/ide_integration/platform.h"
#include "umicom/ide_integration/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_ide_overview_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
