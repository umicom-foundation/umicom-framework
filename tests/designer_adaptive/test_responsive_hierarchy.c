/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_responsive_hierarchy.c
 *
 * PURPOSE:
 *   Validate track component hierarchy participation and responsive variant counts for designer tree views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/responsive_hierarchy.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerResponsiveHierarchyNode n; CHECK(umi_designer_responsive_hierarchy_node_init(&n,"button","form",3U)==UMI_STATUS_OK); CHECK(n.responsive&&n.variant_count==3U);
    return 0;
}
