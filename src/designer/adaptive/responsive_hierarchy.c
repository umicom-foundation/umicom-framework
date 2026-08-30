/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/responsive_hierarchy.c
 *
 * PURPOSE:
 *   Track component hierarchy participation and responsive variant counts for designer tree views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/responsive_hierarchy.h"
#include <string.h>

/* Add responsive metadata to a hierarchy node without duplicating the canonical component tree. */
UmiStatus umi_designer_responsive_hierarchy_node_init(UmiDesignerResponsiveHierarchyNode *node,
                                                      const char *node_id,
                                                      const char *parent_id,
                                                      uint16_t variant_count)
{
    if(node==NULL||node_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(node,0,sizeof *node);
    if(umi_designer_adaptive_copy_text(node->node_id,sizeof node->node_id,node_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(parent_id!=NULL&&parent_id[0]!='\0'&&umi_designer_adaptive_copy_text(node->parent_id,sizeof node->parent_id,parent_id)!=UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    node->variant_count=variant_count; node->responsive=variant_count>0U; return UMI_STATUS_OK;
}
