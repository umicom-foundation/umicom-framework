/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/navigation_designer.c
 *
 * PURPOSE:
 *   Own adaptive navigation-model selection and reorder operations for visual authoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/navigation_designer.h"
#include <string.h>

/* Snapshot the semantic navigation model so visual reordering remains explicit. */
UmiStatus umi_designer_navigation_designer_init(UmiDesignerNavigationDesigner *designer,
                                                const UmiAdaptiveNavigationModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(designer==NULL||model==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(designer,0,sizeof *designer); designer->model=*model; return UMI_STATUS_OK;
}
/* Reorder one item while preserving all command identities and item payloads. */
UmiStatus umi_designer_navigation_designer_move(UmiDesignerNavigationDesigner *designer,
                                                size_t from_index,
                                                size_t to_index)
{
    UmiAdaptiveNavigationItem moved;
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(designer==NULL||from_index>=designer->model.count||to_index>=designer->model.count)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(from_index==to_index)return UMI_STATUS_OK;
    moved=designer->model.items[from_index];
    /* Apply this branch only when its contract condition is satisfied. */
    if(from_index<to_index)/* Visit each bounded item once so every record receives the same rule. */ for(i=from_index;i<to_index;++i)designer->model.items[i]=designer->model.items[i+1U];
    /* Use this fallback path when the earlier condition does not apply. */
    else /* Visit each bounded item once so every record receives the same rule. */ for(i=from_index;i>to_index;--i)designer->model.items[i]=designer->model.items[i-1U];
    designer->model.items[to_index]=moved; ++designer->revision; return UMI_STATUS_OK;
}
