/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/responsive_preview.c
 *
 * PURPOSE:
 *   Create responsive preview copies while preserving the authoritative semantic
 *   layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/responsive_preview.h"
#include "internal.h"


/*
 * Initialise workbench designer responsive preview from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_responsive_preview_init(
    UmiWorkbenchDesignerResponsivePreview *preview)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return;
    (void)memset(preview, 0, sizeof(*preview));
}

/*
 * Add workbench designer responsive preview only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_workbench_designer_responsive_preview_add(
    UmiWorkbenchDesignerResponsivePreview *preview,
    const UmiWorkbenchDesignerResponsiveProfile *profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || profile == NULL || profile->profile_id[0] == '\0' ||
        profile->viewport_size.width <= 0.0 || profile->viewport_size.height <= 0.0 ||
        profile->scale <= 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preview->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(preview->profiles[index].profile_id, profile->profile_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (preview->count >= 16U) return UMI_STATUS_CAPACITY_EXCEEDED;
    preview->profiles[preview->count++] = *profile;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (preview->active_profile_id[0] == '\0') {
        (void)umi_workbench_designer_copy_text(
            preview->active_profile_id, sizeof(preview->active_profile_id),
            profile->profile_id);
    }
    preview->revision += 1U;
    return UMI_STATUS_OK;
}

/* Add responsive only after its inputs and available capacity have been checked. */
static UmiStatus responsive_add(
    UmiWorkbenchDesignerResponsivePreview *preview,
    const char *id,
    const char *label,
    UmiWorkbenchDesignerPreviewDevice device,
    double width,
    double height,
    bool portrait,
    bool hide_secondary,
    bool collapse_sidebars)
{
    UmiWorkbenchDesignerResponsiveProfile profile;
    (void)memset(&profile, 0, sizeof(profile));
    (void)umi_workbench_designer_copy_text(profile.profile_id, sizeof(profile.profile_id), id);
    (void)umi_workbench_designer_copy_text(profile.label, sizeof(profile.label), label);
    profile.device = device;
    profile.viewport_size.width = width;
    profile.viewport_size.height = height;
    profile.scale = 1.0;
    profile.portrait = portrait;
    profile.hide_secondary_panels = hide_secondary;
    profile.collapse_sidebars = collapse_sidebars;
    return umi_workbench_designer_responsive_preview_add(preview, &profile);
}

/*
 * Provide the workbench designer responsive preview seed operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_responsive_preview_seed(
    UmiWorkbenchDesignerResponsivePreview *preview)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = responsive_add(preview, "desktop-wide", "Desktop 1920 x 1080", UMI_WORKBENCH_DESIGNER_PREVIEW_DESKTOP, 1920.0, 1080.0, false, false, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = responsive_add(preview, "laptop", "Laptop 1440 x 900", UMI_WORKBENCH_DESIGNER_PREVIEW_LAPTOP, 1440.0, 900.0, false, false, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = responsive_add(preview, "tablet-landscape", "Tablet landscape", UMI_WORKBENCH_DESIGNER_PREVIEW_TABLET, 1180.0, 820.0, false, false, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = responsive_add(preview, "tablet-portrait", "Tablet portrait", UMI_WORKBENCH_DESIGNER_PREVIEW_TABLET, 820.0, 1180.0, true, true, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = responsive_add(preview, "mobile", "Mobile", UMI_WORKBENCH_DESIGNER_PREVIEW_MOBILE, 430.0, 932.0, true, true, true);
    return status;
}

/*
 * Provide the workbench designer responsive preview activate operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_designer_responsive_preview_activate(
    UmiWorkbenchDesignerResponsivePreview *preview,
    const char *profile_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preview->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(preview->profiles[index].profile_id, profile_id) == 0) {
            UmiStatus status = umi_workbench_designer_copy_text(
                preview->active_profile_id,
                sizeof(preview->active_profile_id), profile_id);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) preview->revision += 1U;
            return status;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the workbench designer responsive preview active operation used by this module
 * and its client applications.
 */
const UmiWorkbenchDesignerResponsiveProfile *umi_workbench_designer_responsive_preview_active(
    const UmiWorkbenchDesignerResponsivePreview *preview)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preview->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(preview->profiles[index].profile_id,
                   preview->active_profile_id) == 0) {
            return &preview->profiles[index];
        }
    }
    return NULL;
}

/*
 * Perform workbench designer responsive preview through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_workbench_designer_responsive_preview_apply(
    const UmiWorkbenchDesignerResponsiveProfile *profile,
    const UmiWorkbenchLayoutDocument *source,
    UmiWorkbenchLayoutDocument *out_document)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || source == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_document_copy(out_document, source);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!profile->hide_secondary_panels && !profile->collapse_sidebars) {
        return UMI_STATUS_OK;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_document->node_count; ++index) {
        UmiWorkbenchLayoutNode *node = &out_document->nodes[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (profile->hide_secondary_panels &&
            node->dock_region == UMI_WORKBENCH_LAYOUT_DOCK_RIGHT &&
            (node->flags & UMI_WORKBENCH_LAYOUT_NODE_PRIMARY) == 0U) {
            node->visibility = UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (profile->collapse_sidebars &&
            (node->dock_region == UMI_WORKBENCH_LAYOUT_DOCK_LEFT ||
             node->dock_region == UMI_WORKBENCH_LAYOUT_DOCK_RIGHT)) {
            node->preferred_size.width = 260;
        }
    }
    return UMI_STATUS_OK;
}
