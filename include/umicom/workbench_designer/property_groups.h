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

/**
 * Represent the workbench designer property group data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerPropertyGroup {
    char group_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    size_t first_property_index;
    size_t property_count;
    size_t issue_count;
    bool expanded;
    bool editable;
} UmiWorkbenchDesignerPropertyGroup;

/**
 * Represent the workbench designer property groups data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerPropertyGroups {
    UmiWorkbenchDesignerPropertyGroup groups[UMI_WORKBENCH_DESIGNER_MAX_PROPERTY_GROUPS];
    size_t count;
    uint64_t properties_revision;
    uint64_t revision;
} UmiWorkbenchDesignerPropertyGroups;

/**
 * Initialise workbench designer property groups from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_property_groups_init(
    UmiWorkbenchDesignerPropertyGroups *groups);
/**
 * Provide the workbench designer property groups build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_property_groups_build(
    UmiWorkbenchDesignerPropertyGroups *groups,
    const UmiWorkbenchDesignerPropertyModel *properties);
/**
 * Provide the workbench designer property groups set expanded operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_designer_property_groups_set_expanded(
    UmiWorkbenchDesignerPropertyGroups *groups,
    const char *group_id,
    bool expanded);
/**
 * Find workbench designer property group while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerPropertyGroup *umi_workbench_designer_property_group_at(
    const UmiWorkbenchDesignerPropertyGroups *groups,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
