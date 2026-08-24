/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/views/inline_ai.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral Inline AI view.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_VIEW_INLINE_AI_H
#define UMICOM_IDE_INTEGRATION_VIEW_INLINE_AI_H
#include "umicom/ide_integration/platform.h"
#include "umicom/ide_integration/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_ide_inline_ai_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
