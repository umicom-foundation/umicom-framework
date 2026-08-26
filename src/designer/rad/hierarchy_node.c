/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/hierarchy_node.c
 *
 * PURPOSE:
 *   Represent one node in the designer object hierarchy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/hierarchy_node.h"
#include <string.h>
UmiStatus umi_rad_hierarchy_node_init(UmiRadHierarchyNode *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->node_id, sizeof item->node_id, "hierarchy_node");
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "hierarchy_node");
    return UMI_STATUS_OK;
}
int umi_rad_hierarchy_node_is_valid(const UmiRadHierarchyNode *item){if(item==NULL)return 0;return umi_rad_id_valid(item->node_id);}
