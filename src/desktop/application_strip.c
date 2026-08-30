/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/application_strip.c
 *
 * PURPOSE:
 *   Implement deterministic Umicom Desk application-taskbar projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/application_strip.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/application/resource_catalogue.h"

struct UmiDesktopApplicationStrip {
    UmiApplicationRuntimeCatalogue *catalogue;
    UmiDesktopApplicationStripItem items[
        UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS];
    size_t count;
    uint64_t revision;
};

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source,
                           bool allow_empty)
{
    size_t length;
    if (destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (source == NULL || source[0] == '\0') {
        if (!allow_empty) return UMI_STATUS_INVALID_ARGUMENT;
        destination[0] = '\0';
        return UMI_STATUS_OK;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static int compare_items(const void *left_value, const void *right_value)
{
    const UmiDesktopApplicationStripItem *left =
        (const UmiDesktopApplicationStripItem *)left_value;
    const UmiDesktopApplicationStripItem *right =
        (const UmiDesktopApplicationStripItem *)right_value;
    int group_comparison;
    if (left->pinned != right->pinned) return left->pinned ? -1 : 1;
    if (left->active != right->active) return left->active ? -1 : 1;
    group_comparison = strcmp(left->taskbar_group, right->taskbar_group);
    if (group_comparison != 0) return group_comparison;
    return strcmp(left->display_name, right->display_name);
}

static UmiStatus project_record(
    const UmiApplicationRuntimeRecord *record,
    UmiDesktopApplicationStripItem *item)
{
    const UmiApplicationResourceDescriptor *icon;
    UmiStatus status;
    if (record == NULL || item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    status = copy_text(item->application_id,
                       sizeof(item->application_id),
                       record->application_id, false);
    if (status == UMI_STATUS_OK) {
        status = copy_text(item->display_name,
                           sizeof(item->display_name),
                           record->display_name, false);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(item->icon_resource_id,
                           sizeof(item->icon_resource_id),
                           record->icon_resource_id, true);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(item->default_layout_id,
                           sizeof(item->default_layout_id),
                           record->default_layout_id, true);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(item->taskbar_group,
                           sizeof(item->taskbar_group),
                           record->taskbar_group, true);
    }
    if (status != UMI_STATUS_OK) return status;

    icon = record->icon_resource_id[0] != '\0'
        ? umi_application_resource_catalogue_find(
            record->icon_resource_id)
        : NULL;
    if (icon != NULL &&
        icon->kind == UMI_APPLICATION_RESOURCE_THEME_ICON) {
        status = copy_text(item->icon_name, sizeof(item->icon_name),
                           icon->locator, true);
    } else {
        status = copy_text(item->icon_name, sizeof(item->icon_name),
                           "application-x-executable-symbolic", false);
    }
    if (status != UMI_STATUS_OK) return status;

    item->state = record->state;
    item->entry_kind = record->entry_kind;
    item->pinned = record->pinned;
    item->running = record->running;
    item->active = record->active;
    item->attention = record->attention;
    item->enabled = record->enabled;
    item->revision = record->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_strip_create(
    UmiApplicationRuntimeCatalogue *catalogue,
    UmiDesktopApplicationStrip **out_strip)
{
    UmiDesktopApplicationStrip *strip;
    UmiStatus status;
    if (catalogue == NULL || out_strip == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_strip = NULL;
    strip = (UmiDesktopApplicationStrip *)calloc(1U, sizeof(*strip));
    if (strip == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    strip->catalogue = catalogue;
    strip->revision = 1U;
    status = umi_desktop_application_strip_refresh(strip);
    if (status != UMI_STATUS_OK) {
        free(strip);
        return status;
    }
    *out_strip = strip;
    return UMI_STATUS_OK;
}

void umi_desktop_application_strip_destroy(
    UmiDesktopApplicationStrip *strip)
{
    free(strip);
}

UmiStatus umi_desktop_application_strip_refresh(
    UmiDesktopApplicationStrip *strip)
{
    const size_t count =
        strip != NULL
            ? umi_application_runtime_catalogue_count(strip->catalogue)
            : 0U;
    size_t index;
    if (strip == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    strip->count = 0U;
    for (index = 0U; index < count; ++index) {
        UmiApplicationRuntimeRecord record;
        UmiStatus status = umi_application_runtime_catalogue_at(
            strip->catalogue, index, &record);
        if (status != UMI_STATUS_OK) return status;
        if (!record.visible && !record.pinned) continue;
        if (strip->count >= UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = project_record(
            &record, &strip->items[strip->count]);
        if (status != UMI_STATUS_OK) return status;
        strip->count += 1U;
    }
    qsort(strip->items, strip->count, sizeof(strip->items[0]),
          compare_items);
    strip->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_strip_at(
    const UmiDesktopApplicationStrip *strip,
    size_t index,
    UmiDesktopApplicationStripItem *out_item)
{
    if (strip == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= strip->count) return UMI_STATUS_NOT_FOUND;
    *out_item = strip->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_strip_find(
    const UmiDesktopApplicationStrip *strip,
    const char *application_id,
    UmiDesktopApplicationStripItem *out_item)
{
    size_t index;
    if (strip == NULL || application_id == NULL ||
        application_id[0] == '\0' || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < strip->count; ++index) {
        if (strcmp(strip->items[index].application_id,
                   application_id) == 0) {
            *out_item = strip->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_desktop_application_strip_request(
    UmiDesktopApplicationStrip *strip,
    UmiApplicationLauncher *launcher,
    const char *application_id,
    UmiDesktopApplicationStripAction action)
{
    UmiDesktopApplicationStripItem item;
    UmiStatus status;
    if (strip == NULL || launcher == NULL ||
        application_id == NULL || application_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_desktop_application_strip_find(
        strip, application_id, &item);
    if (status != UMI_STATUS_OK) return status;

    switch (action) {
    case UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE:
        status = umi_application_launcher_request(
            launcher, application_id,
            item.running
                ? UMI_APPLICATION_LAUNCH_ACTIVATE
                : UMI_APPLICATION_LAUNCH_START);
        break;
    case UMI_DESKTOP_APPLICATION_STRIP_TOGGLE_PIN:
        status = umi_application_runtime_catalogue_pin(
            strip->catalogue, application_id, !item.pinned);
        break;
    case UMI_DESKTOP_APPLICATION_STRIP_STOP:
        status = umi_application_launcher_request(
            launcher, application_id, UMI_APPLICATION_LAUNCH_STOP);
        break;
    case UMI_DESKTOP_APPLICATION_STRIP_RESTART:
        status = umi_application_launcher_request(
            launcher, application_id, UMI_APPLICATION_LAUNCH_RESTART);
        break;
    default:
        status = UMI_STATUS_INVALID_ARGUMENT;
        break;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_desktop_application_strip_refresh(strip);
    }
    return status;
}

UmiStatus umi_desktop_application_strip_snapshot(
    const UmiDesktopApplicationStrip *strip,
    UmiDesktopApplicationStripSnapshot *out_snapshot)
{
    size_t index;
    if (strip == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->item_count = strip->count;
    out_snapshot->revision = strip->revision;
    for (index = 0U; index < strip->count; ++index) {
        const UmiDesktopApplicationStripItem *item =
            &strip->items[index];
        if (item->pinned) out_snapshot->pinned_count += 1U;
        if (item->running) out_snapshot->running_count += 1U;
        if (item->attention) out_snapshot->attention_count += 1U;
        if (item->active) {
            (void)copy_text(out_snapshot->active_application_id,
                            sizeof(out_snapshot->active_application_id),
                            item->application_id, true);
        }
    }
    return UMI_STATUS_OK;
}

const char *umi_desktop_application_strip_action_text(
    UmiDesktopApplicationStripAction action)
{
    switch (action) {
    case UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE:
        return "launch-or-activate";
    case UMI_DESKTOP_APPLICATION_STRIP_TOGGLE_PIN:
        return "toggle-pin";
    case UMI_DESKTOP_APPLICATION_STRIP_STOP:
        return "stop";
    case UMI_DESKTOP_APPLICATION_STRIP_RESTART:
        return "restart";
    default:
        return "invalid";
    }
}
