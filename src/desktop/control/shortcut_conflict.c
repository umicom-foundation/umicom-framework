/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/shortcut_conflict.c
 * PURPOSE: Implement represent shortcut conflict evidence and resolution priority.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/shortcut_conflict.h"

#include <string.h>

void umi_desktop_shortcut_conflict_init(UmiDesktopShortcutConflictSnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_SHORTCUT_CONFLICT_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_shortcut_conflict_validate(const UmiDesktopShortcutConflictSnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_SHORTCUT_CONFLICT_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_shortcut_conflict_set_subject(UmiDesktopShortcutConflictSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_shortcut_conflict_set_detail(UmiDesktopShortcutConflictSnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_shortcut_conflict_set_state(UmiDesktopShortcutConflictSnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_shortcut_conflict_set_priority(UmiDesktopShortcutConflictSnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_shortcut_conflict_set_enabled(UmiDesktopShortcutConflictSnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_shortcut_conflict_same_identity(const UmiDesktopShortcutConflictSnapshot *left, const UmiDesktopShortcutConflictSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

bool umi_desktop_shortcut_conflict_conflicts(const char *left_chord, const char *right_chord, const char *left_context, const char *right_context)
{
    if (left_chord == NULL || right_chord == NULL || left_context == NULL || right_context == NULL) return false;
    return strcmp(left_chord, right_chord) == 0 &&
           (strcmp(left_context, right_context) == 0 || left_context[0] == '\0' || right_context[0] == '\0');
}
