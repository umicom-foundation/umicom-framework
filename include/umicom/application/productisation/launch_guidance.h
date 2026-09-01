/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/launch_guidance.h
 *
 * PURPOSE:
 *   Describe a selected group of applications before any process is started.
 *   The snapshot combines runtime eligibility with Framework-owned workspace
 *   guidance so launchers can explain start, activation and warning outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_LAUNCH_GUIDANCE_H
#define UMICOM_APPLICATION_PRODUCTISATION_LAUNCH_GUIDANCE_H

#include "umicom/application/launch_selection.h"
#include "umicom/application/productisation/workspace_guide_portfolio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_LAUNCH_GUIDANCE_TEXT_CAPACITY 512U
#define UMI_PRODUCT_GUIDED_LAUNCH_MAX_ENTRIES \
    UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS

/* This state tells a user interface what its main message should explain. */
typedef enum UmiProductLaunchGuidanceState {
    UMI_PRODUCT_LAUNCH_GUIDANCE_NOT_SELECTED = 1,
    UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_START = 2,
    UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_ACTIVATE = 3,
    UMI_PRODUCT_LAUNCH_GUIDANCE_UNAVAILABLE = 4,
    UMI_PRODUCT_LAUNCH_GUIDANCE_MISSING_GUIDE = 5
} UmiProductLaunchGuidanceState;

/* One entry owns all text needed to render an application launch choice. */
typedef struct UmiProductGuidedLaunchEntry {
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char display_name[UMI_APPLICATION_RUNTIME_NAME_CAPACITY];
    char recommended_layout_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char explanation[UMI_PRODUCT_LAUNCH_GUIDANCE_TEXT_CAPACITY];
    UmiApplicationLaunchAction resolved_action;
    UmiApplicationRuntimeState runtime_state;
    UmiProductLaunchGuidanceState guidance_state;
    unsigned readiness_percent;
    int selected;
    int eligible;
    int running;
    int guidance_available;
    int acceptance_ready;
    int ready_to_execute;
    uint64_t runtime_revision;
} UmiProductGuidedLaunchEntry;

/* The plan is a fixed-capacity, read-only preview of one selection revision. */
typedef struct UmiProductGuidedLaunchPlan {
    uint32_t structure_size;
    UmiProductGuidedLaunchEntry
        entries[UMI_PRODUCT_GUIDED_LAUNCH_MAX_ENTRIES];
    size_t entry_count;
    size_t selected_count;
    size_t ready_to_execute_count;
    size_t start_count;
    size_t activate_count;
    size_t unavailable_count;
    size_t missing_guidance_count;
    size_t guidance_warning_count;
    uint64_t selection_revision;
    int executable;
} UmiProductGuidedLaunchPlan;

/** Build a non-mutating preview from the current user selection and guides. */
UmiStatus umi_product_guided_launch_plan_build(
    const UmiApplicationLaunchSelection *selection,
    const UmiProductWorkspaceGuidePortfolio *portfolio,
    UmiProductGuidedLaunchPlan *out_plan);

/** Validate bounds, entry state and aggregate counters before presentation. */
UmiStatus umi_product_guided_launch_plan_validate(
    const UmiProductGuidedLaunchPlan *plan);

/** Return a borrowed entry when its index belongs to the validated plan. */
const UmiProductGuidedLaunchEntry *umi_product_guided_launch_plan_at(
    const UmiProductGuidedLaunchPlan *plan,
    size_t index);

/** Find a borrowed entry by the stable application identifier. */
const UmiProductGuidedLaunchEntry *umi_product_guided_launch_plan_find(
    const UmiProductGuidedLaunchPlan *plan,
    const char *application_id);

/** Convert a guidance state into short text suitable for logs and tests. */
const char *umi_product_launch_guidance_state_text(
    UmiProductLaunchGuidanceState state);

#ifdef __cplusplus
}
#endif

#endif
