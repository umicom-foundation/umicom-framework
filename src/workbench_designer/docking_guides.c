/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/docking_guides.c
 *
 * PURPOSE:
 *   Implement guide state independently of drawing toolkit details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/docking_guides.h"
#include "internal.h"


/*
 * Initialise workbench designer docking guides from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_docking_guides_init(
    UmiWorkbenchDesignerDockingGuideModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
}

/*
 * Provide the workbench designer docking guides build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_docking_guides_build(
    UmiWorkbenchDesignerDockingGuideModel *model,
    const UmiWorkbenchDesignerDropTargetSet *targets,
    const char *target_node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || targets == NULL || target_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_docking_guides_init(model);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < targets->count; ++index) {
        const UmiWorkbenchDesignerDropTarget *target = &targets->targets[index];
        UmiWorkbenchDesignerDockingGuide *guide;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(target->target_node_id, target_node_id) != 0) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->count >= UMI_WORKBENCH_DESIGNER_MAX_GUIDES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        guide = &model->guides[model->count++];
        (void)memset(guide, 0, sizeof(*guide));
        (void)umi_workbench_designer_copy_text(
            guide->guide_id, sizeof(guide->guide_id), target->target_id);
        guide->zone = target->zone;
        guide->bounds = target->bounds;
        guide->enabled = target->enabled;
    }
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer docking guides activate operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_docking_guides_activate(
    UmiWorkbenchDesignerDockingGuideModel *model,
    const char *guide_id)
{
    size_t index;
    bool found = false;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || guide_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        bool active = strcmp(model->guides[index].guide_id, guide_id) == 0;
        model->guides[index].active = active;
        /* Apply this operation only while the related capability or state is available. */
        if (active) found = true;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!found) return UMI_STATUS_NOT_FOUND;
    (void)umi_workbench_designer_copy_text(
        model->active_guide_id, sizeof(model->active_guide_id), guide_id);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer docking guide active operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerDockingGuide *umi_workbench_designer_docking_guide_active(
    const UmiWorkbenchDesignerDockingGuideModel *model)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || model->active_guide_id[0] == '\0') return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->guides[index].active) return &model->guides[index];
    }
    return NULL;
}
