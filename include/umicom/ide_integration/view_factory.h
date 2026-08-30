/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/view_factory.h
 *
 * PURPOSE:
 *   Create toolkit-neutral IDE integration views by stable ID.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_VIEW_FACTORY_H
#define UMICOM_IDE_INTEGRATION_VIEW_FACTORY_H
#include "umicom/ide_integration/view_ids.h"
#include "umicom/ide_integration/views/views.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_ide_view_factory_create(
    UmiIdeIntegrationPlatform *platform,
    const char *view_id,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
