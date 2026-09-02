/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/views/navigation.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral Navigation History view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_VIEW_NAVIGATION_H
#define UMICOM_IDE_INTEGRATION_VIEW_NAVIGATION_H
#include "umicom/ide_integration/platform.h"
#include "umicom/ide_integration/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Initialise ide navigation view from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_navigation_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
