/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/responsive_hierarchy.h
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

#ifndef UMICOM_DESIGNER_ADAPTIVE_RESPONSIVE_HIERARCHY_H
#define UMICOM_DESIGNER_ADAPTIVE_RESPONSIVE_HIERARCHY_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerResponsiveHierarchyNode {
    char node_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    char parent_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    uint16_t variant_count;
    int responsive;
} UmiDesignerResponsiveHierarchyNode;
/* Initialise one responsive hierarchy node. */
UmiStatus umi_designer_responsive_hierarchy_node_init(UmiDesignerResponsiveHierarchyNode *node,
                                                      const char *node_id,
                                                      const char *parent_id,
                                                      uint16_t variant_count);

#ifdef __cplusplus
}
#endif
#endif
