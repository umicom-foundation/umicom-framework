/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/insertion_marker.c
 *
 * PURPOSE:
 *   Represent insertion feedback within ordered containers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/insertion_marker.h"
#include <string.h>
/*
 * Initialise visual designer insertion marker from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_insertion_marker_init(UmiRadInsertionMarker *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "insertion_marker");
    item->visible = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer insertion marker satisfies its contract before another service relies on
 * it.
 */
int umi_rad_insertion_marker_is_valid(const UmiRadInsertionMarker *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->parent_id) && item->index >= 0;}
