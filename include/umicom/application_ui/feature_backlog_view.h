/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_ui/feature_backlog_view.h
 *
 * PURPOSE:
 *   Project incomplete features with state, priority and ownership for implementation centres.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_UI_FEATURE_BACKLOG_VIEW_H
#define UMICOM_APPLICATION_UI_FEATURE_BACKLOG_VIEW_H

#include "umicom/application/runtime/experience_projection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by application ui feature backlog without
 * changing their state.
 */
size_t umi_application_ui_feature_backlog_count(
    const UmiApplicationExperienceDefinition *experience);
/**
 * Provide the application ui feature backlog row operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_ui_feature_backlog_row(
    const UmiApplicationExperienceDefinition *experience,
    size_t backlog_index,
    UmiApplicationFeatureProjection *out_row);

#ifdef __cplusplus
}
#endif

#endif
