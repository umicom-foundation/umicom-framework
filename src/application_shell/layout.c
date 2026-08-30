/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/layout.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral placement and drag/drop movement for reusable
 *   application-shell contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/layout.h"
#include "umicom/application_shell/drag_drop.h"

#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";

    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static size_t find_index(const UmiApplicationShellLayout *layout,
                         const char *contribution_id)
{
    size_t index;

    if (layout == NULL || contribution_id == NULL) return (size_t)-1;

    for (index = 0U; index < layout->placement_count; ++index) {
        if (strcmp(layout->placements[index].contribution_id,
                   contribution_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

void umi_application_shell_layout_init(
    UmiApplicationShellLayout *layout,
    const char *layout_id,
    const char *title)
{
    if (layout == NULL) return;

    (void)memset(layout, 0, sizeof(*layout));
    copy_text(layout->layout_id, sizeof(layout->layout_id), layout_id);
    copy_text(layout->title, sizeof(layout->title), title);
    layout->revision = 1U;
}

UmiStatus umi_application_shell_layout_place(
    UmiApplicationShellLayout *layout,
    const char *contribution_id,
    const char *container_id,
    UmiApplicationShellRegion region,
    size_t order,
    int visible)
{
    UmiApplicationShellPlacement *placement;
    size_t index;

    if (layout == NULL || contribution_id == NULL ||
        contribution_id[0] == '\0' ||
        region < UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR ||
        region > UMI_APPLICATION_SHELL_REGION_WINDOW) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(layout, contribution_id);
    if (index == (size_t)-1) {
        if (layout->placement_count >=
            UMI_APPLICATION_SHELL_LAYOUT_MAX_ITEMS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = layout->placement_count++;
    }

    placement = &layout->placements[index];
    (void)memset(placement, 0, sizeof(*placement));
    copy_text(placement->contribution_id,
              sizeof(placement->contribution_id),
              contribution_id);
    copy_text(placement->container_id,
              sizeof(placement->container_id),
              container_id);
    placement->region = region;
    placement->order = order;
    placement->visible = visible != 0;

    layout->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_shell_layout_move(
    UmiApplicationShellLayout *layout,
    const UmiApplicationShellDropTransaction *transaction)
{
    UmiApplicationShellPlacement placement;
    UmiStatus status;

    if (layout == NULL || transaction == NULL ||
        !transaction->accepted) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_application_shell_layout_find(
        layout,
        transaction->source_contribution_id,
        &placement);
    if (status != UMI_STATUS_OK) return status;

    return umi_application_shell_layout_place(
        layout,
        transaction->source_contribution_id,
        transaction->target_container_id,
        transaction->target_region,
        transaction->target_index,
        placement.visible);
}

UmiStatus umi_application_shell_layout_find(
    const UmiApplicationShellLayout *layout,
    const char *contribution_id,
    UmiApplicationShellPlacement *out_placement)
{
    size_t index;

    if (layout == NULL || contribution_id == NULL ||
        out_placement == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(layout, contribution_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_placement = layout->placements[index];
    return UMI_STATUS_OK;
}
