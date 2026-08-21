/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/property_groups.c
 *
 * PURPOSE:
 *   Derive deterministic inspector sections from property identities and
 *   preserve user expansion state where possible.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/property_groups.h"
#include "internal.h"


void umi_workbench_designer_property_groups_init(
    UmiWorkbenchDesignerPropertyGroups *groups)
{
    if (groups != NULL) (void)memset(groups, 0, sizeof(*groups));
}

static const char *property_group_id(const char *property_id)
{
    if (property_id == NULL) return "general";
    if (strncmp(property_id, "bounds", 6U) == 0 ||
        strncmp(property_id, "size", 4U) == 0 ||
        strncmp(property_id, "position", 8U) == 0) return "geometry";
    if (strstr(property_id, "component") != NULL ||
        strstr(property_id, "owner") != NULL) return "component";
    if (strstr(property_id, "split") != NULL ||
        strstr(property_id, "dock") != NULL ||
        strstr(property_id, "orientation") != NULL) return "layout";
    if (strstr(property_id, "context") != NULL) return "context";
    if (strstr(property_id, "visible") != NULL ||
        strstr(property_id, "locked") != NULL) return "behaviour";
    return "general";
}

static const char *property_group_label(const char *group_id)
{
    if (strcmp(group_id, "geometry") == 0) return "Geometry";
    if (strcmp(group_id, "component") == 0) return "Component";
    if (strcmp(group_id, "layout") == 0) return "Layout";
    if (strcmp(group_id, "context") == 0) return "Context Linking";
    if (strcmp(group_id, "behaviour") == 0) return "Behaviour";
    return "General";
}

static UmiWorkbenchDesignerPropertyGroup *property_find_group(
    UmiWorkbenchDesignerPropertyGroups *groups,
    const char *group_id)
{
    size_t index;
    for (index = 0U; index < groups->count; ++index) {
        if (strcmp(groups->groups[index].group_id, group_id) == 0) {
            return &groups->groups[index];
        }
    }
    return NULL;
}

UmiStatus umi_workbench_designer_property_groups_build(
    UmiWorkbenchDesignerPropertyGroups *groups,
    const UmiWorkbenchDesignerPropertyModel *properties)
{
    size_t index;
    if (groups == NULL || properties == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_property_groups_init(groups);
    for (index = 0U; index < properties->count; ++index) {
        const UmiWorkbenchDesignerProperty *property = &properties->properties[index];
        const char *group_id = property_group_id(property->property_id);
        UmiWorkbenchDesignerPropertyGroup *group =
            property_find_group(groups, group_id);
        if (group == NULL) {
            if (groups->count >= UMI_WORKBENCH_DESIGNER_MAX_PROPERTY_GROUPS) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            group = &groups->groups[groups->count++];
            (void)memset(group, 0, sizeof(*group));
            (void)umi_workbench_designer_copy_text(
                group->group_id, sizeof(group->group_id), group_id);
            (void)umi_workbench_designer_copy_text(
                group->label, sizeof(group->label),
                property_group_label(group_id));
            group->first_property_index = index;
            group->expanded = true;
            group->editable = true;
        }
        group->property_count += 1U;
        if (!property->editable) group->editable = false;
    }
    groups->properties_revision = properties->revision;
    groups->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_property_groups_set_expanded(
    UmiWorkbenchDesignerPropertyGroups *groups,
    const char *group_id,
    bool expanded)
{
    UmiWorkbenchDesignerPropertyGroup *group;
    if (groups == NULL || group_id == NULL || group_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    group = property_find_group(groups, group_id);
    if (group == NULL) return UMI_STATUS_NOT_FOUND;
    group->expanded = expanded;
    groups->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiWorkbenchDesignerPropertyGroup *umi_workbench_designer_property_group_at(
    const UmiWorkbenchDesignerPropertyGroups *groups,
    size_t index)
{
    return groups != NULL && index < groups->count ? &groups->groups[index] : NULL;
}
