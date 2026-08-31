/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/workspace_guide.h
 *
 * PURPOSE:
 *   Describe the layouts and unfinished product work that a shared welcome
 *   screen can present for any thin Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_WORKSPACE_GUIDE_H
#define UMICOM_APPLICATION_PRODUCTISATION_WORKSPACE_GUIDE_H

#include "umicom/application/experience.h"
#include "umicom/application/productisation/adoption.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The fixed capacity keeps snapshots safe to pass across module boundaries. */
#define UMI_PRODUCT_WORKSPACE_GUIDE_LAYOUT_CAPACITY 32U

/* One choice explains a Framework-owned layout without copying its panels. */
typedef struct UmiProductWorkspaceGuideChoice {
    char layout_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char title[UMI_PRODUCTISATION_ID_CAPACITY];
    char description[UMI_PRODUCTISATION_TEXT_CAPACITY];
    size_t panel_count;
    uint32_t flags;
    int default_layout;
} UmiProductWorkspaceGuideChoice;

/* The complete snapshot is suitable for welcome pages, launchers and menus. */
typedef struct UmiProductWorkspaceGuide {
    uint32_t structure_size;
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char display_name[UMI_PRODUCTISATION_ID_CAPACITY];
    char executable_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char recommended_layout_id[UMI_PRODUCTISATION_ID_CAPACITY];
    UmiProductWorkspaceGuideChoice
        choices[UMI_PRODUCT_WORKSPACE_GUIDE_LAYOUT_CAPACITY];
    size_t choice_count;
    size_t total_panel_placements;
    size_t planned_feature_count;
    size_t foundation_feature_count;
    size_t implemented_feature_count;
    size_t verified_feature_count;
    char next_feature_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char next_feature_title[UMI_PRODUCTISATION_ID_CAPACITY];
    char next_feature_summary[UMI_PRODUCTISATION_TEXT_CAPACITY];
    UmiExperienceFeatureState next_feature_state;
    UmiExperiencePriority next_feature_priority;
    UmiExperienceOwnership next_feature_owner;
    unsigned readiness_percent;
    int has_next_feature;
    int acceptance_ready;
} UmiProductWorkspaceGuide;

/* Build a safe guide entirely from the canonical Framework experience. */
UmiStatus umi_product_workspace_guide_build(
    const UmiProductApplicationAdoption *adoption,
    UmiProductWorkspaceGuide *out_guide);

/* Validate a guide before a toolkit or another process consumes it. */
UmiStatus umi_product_workspace_guide_validate(
    const UmiProductWorkspaceGuide *guide);

/* Return a borrowed choice at an index, or NULL when the index is invalid. */
const UmiProductWorkspaceGuideChoice *umi_product_workspace_guide_at(
    const UmiProductWorkspaceGuide *guide,
    size_t index);

/* Find a borrowed choice by its stable Framework layout identifier. */
const UmiProductWorkspaceGuideChoice *umi_product_workspace_guide_find(
    const UmiProductWorkspaceGuide *guide,
    const char *layout_id);

/* Return the recommended starting choice selected by the experience owner. */
const UmiProductWorkspaceGuideChoice *umi_product_workspace_guide_recommended(
    const UmiProductWorkspaceGuide *guide);

#ifdef __cplusplus
}
#endif

#endif
