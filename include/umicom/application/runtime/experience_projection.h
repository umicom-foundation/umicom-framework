/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/experience_projection.h
 *
 * PURPOSE:
 *   Project canonical experience objects into compact rows reusable by Studio, Desk and product frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_EXPERIENCE_PROJECTION_H
#define UMICOM_APPLICATION_RUNTIME_EXPERIENCE_PROJECTION_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationPanelProjection {
    const char *panel_id;
    const char *title;
    const char *region;
    const char *context_group_id;
    uint32_t flags;
} UmiApplicationPanelProjection;

typedef struct UmiApplicationLayoutProjection {
    const char *layout_id;
    const char *title;
    size_t panel_count;
    uint32_t flags;
} UmiApplicationLayoutProjection;

typedef struct UmiApplicationFeatureProjection {
    const char *feature_id;
    const char *title;
    UmiExperienceFeatureState state;
    UmiExperiencePriority priority;
    UmiExperienceOwnership owner;
} UmiApplicationFeatureProjection;

UmiStatus umi_application_panel_projection_at(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationPanelProjection *out_projection);
UmiStatus umi_application_layout_projection_at(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationLayoutProjection *out_projection);
UmiStatus umi_application_feature_projection_at(
    const UmiApplicationExperienceDefinition *experience,
    size_t index,
    UmiApplicationFeatureProjection *out_projection);

#ifdef __cplusplus
}
#endif

#endif
