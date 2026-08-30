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


void umi_workbench_designer_docking_guides_init(
    UmiWorkbenchDesignerDockingGuideModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
}

UmiStatus umi_workbench_designer_docking_guides_build(
    UmiWorkbenchDesignerDockingGuideModel *model,
    const UmiWorkbenchDesignerDropTargetSet *targets,
    const char *target_node_id)
{
    size_t index;
    if (model == NULL || targets == NULL || target_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_docking_guides_init(model);
    for (index = 0U; index < targets->count; ++index) {
        const UmiWorkbenchDesignerDropTarget *target = &targets->targets[index];
        UmiWorkbenchDesignerDockingGuide *guide;
        if (strcmp(target->target_node_id, target_node_id) != 0) continue;
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

UmiStatus umi_workbench_designer_docking_guides_activate(
    UmiWorkbenchDesignerDockingGuideModel *model,
    const char *guide_id)
{
    size_t index;
    bool found = false;
    if (model == NULL || guide_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->count; ++index) {
        bool active = strcmp(model->guides[index].guide_id, guide_id) == 0;
        model->guides[index].active = active;
        if (active) found = true;
    }
    if (!found) return UMI_STATUS_NOT_FOUND;
    (void)umi_workbench_designer_copy_text(
        model->active_guide_id, sizeof(model->active_guide_id), guide_id);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiWorkbenchDesignerDockingGuide *umi_workbench_designer_docking_guide_active(
    const UmiWorkbenchDesignerDockingGuideModel *model)
{
    size_t index;
    if (model == NULL || model->active_guide_id[0] == '\0') return NULL;
    for (index = 0U; index < model->count; ++index) {
        if (model->guides[index].active) return &model->guides[index];
    }
    return NULL;
}
