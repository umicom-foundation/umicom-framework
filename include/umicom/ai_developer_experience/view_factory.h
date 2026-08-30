/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/view_factory.h
 *
 * PURPOSE:
 *   Create any AI Developer pane from the composed Framework platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_FACTORY_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_FACTORY_H

#include "umicom/ai_developer_experience/platform.h"
#include "umicom/ai_developer_experience/views/views.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_developer_view_factory_create(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperPaneKind pane,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif
#endif
