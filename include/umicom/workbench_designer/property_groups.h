/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/property_groups.h
 *
 * PURPOSE:
 *   Group property-inspector rows into stable sections with expansion, issue and
 *   editability state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PROPERTY_GROUPS_H
#define UMICOM_WORKBENCH_DESIGNER_PROPERTY_GROUPS_H

#include "umicom/workbench_designer/properties.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_PROPERTY_GROUPS 16U

typedef struct UmiWorkbenchDesignerPropertyGroup {
    char group_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    size_t first_property_index;
    size_t property_count;
    size_t issue_count;
    bool expanded;
    bool editable;
} UmiWorkbenchDesignerPropertyGroup;

typedef struct UmiWorkbenchDesignerPropertyGroups {
    UmiWorkbenchDesignerPropertyGroup groups[UMI_WORKBENCH_DESIGNER_MAX_PROPERTY_GROUPS];
    size_t count;
    uint64_t properties_revision;
    uint64_t revision;
} UmiWorkbenchDesignerPropertyGroups;

void umi_workbench_designer_property_groups_init(
    UmiWorkbenchDesignerPropertyGroups *groups);
UmiStatus umi_workbench_designer_property_groups_build(
    UmiWorkbenchDesignerPropertyGroups *groups,
    const UmiWorkbenchDesignerPropertyModel *properties);
UmiStatus umi_workbench_designer_property_groups_set_expanded(
    UmiWorkbenchDesignerPropertyGroups *groups,
    const char *group_id,
    bool expanded);
const UmiWorkbenchDesignerPropertyGroup *umi_workbench_designer_property_group_at(
    const UmiWorkbenchDesignerPropertyGroups *groups,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
