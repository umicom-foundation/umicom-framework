/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/selection.c
 *
 * PURPOSE:
 *   Implement canonical structured selection identity, fields, validation and content hashing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/selection.h"

#include <string.h>

/*
 * Initialise workbench selection from caller-provided values so later operations receive a
 * known state.
 */
void umi_workbench_selection_init(
    UmiWorkbenchSelection *selection,
    UmiWorkbenchSelectionKind kind,
    UmiContextKind context_kind,
    const char *selection_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return;
    memset(selection, 0, sizeof(*selection));
    selection->structure_size = (uint32_t)sizeof(*selection);
    selection->kind = kind;
    selection->context_kind = context_kind;
    selection->activation = UMI_WORKBENCH_SELECTION_ACTIVATION_SELECT;
    selection->state = UMI_WORKBENCH_SELECTION_STATE_CREATED;
    selection->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection_id != NULL) {
        (void)umi_workbench_selection_copy_text(
            selection->selection_id,
            sizeof(selection->selection_id),
            selection_id);
    }
}

/*
 * Provide the workbench selection set origin operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_selection_set_origin(
    UmiWorkbenchSelection *selection,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || application_id == NULL || panel_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_copy_text(
        selection->application_id, sizeof(selection->application_id),
        application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_copy_text(
        selection->panel_id, sizeof(selection->panel_id), panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace_id != NULL) {
        status = umi_workbench_selection_copy_text(
            selection->workspace_id, sizeof(selection->workspace_id),
            workspace_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    ++selection->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection set subject operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_selection_set_subject(
    UmiWorkbenchSelection *selection,
    const char *subject_id,
    const char *secondary_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || subject_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_copy_text(
        selection->subject_id, sizeof(selection->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (secondary_id != NULL) {
        status = umi_workbench_selection_copy_text(
            selection->secondary_id, sizeof(selection->secondary_id),
            secondary_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    ++selection->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection set location operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_set_location(
    UmiWorkbenchSelection *selection,
    const char *path,
    uint32_t line,
    uint32_t column,
    uint32_t selection_length)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_selection_copy_text(
        selection->path, sizeof(selection->path), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    selection->line = line;
    selection->column = column;
    selection->selection_length = selection_length;
    ++selection->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection set display text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_set_display_text(
    UmiWorkbenchSelection *selection,
    const char *display_text)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || display_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_copy_text(
        selection->display_text, sizeof(selection->display_text),
        display_text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++selection->revision;
    return status;
}

/*
 * Provide the workbench selection set group hint operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_set_group_hint(
    UmiWorkbenchSelection *selection,
    const char *group_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_copy_text(
        selection->group_hint, sizeof(selection->group_hint), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++selection->revision;
    return status;
}

/*
 * Provide the workbench selection add field operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_selection_add_field(
    UmiWorkbenchSelection *selection,
    const UmiWorkbenchSelectionField *field)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_selection_field_validate(field);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->field_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(selection->fields[index].name, field->name) == 0) {
            selection->fields[index] = *field;
            ++selection->revision;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (selection->field_count >= UMI_WORKBENCH_SELECTION_MAX_FIELDS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    selection->fields[selection->field_count++] = *field;
    ++selection->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection find field operation used by this module and its client
 * applications.
 */
const UmiWorkbenchSelectionField *umi_workbench_selection_find_field(
    const UmiWorkbenchSelection *selection,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || name == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->field_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(selection->fields[index].name, name) == 0) {
            return &selection->fields[index];
        }
    }
    return NULL;
}

/*
 * Check that workbench selection satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_workbench_selection_validate(
    const UmiWorkbenchSelection *selection)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL ||
        selection->structure_size != sizeof(*selection) ||
        selection->selection_id[0] == '\0' ||
        selection->application_id[0] == '\0' ||
        selection->panel_id[0] == '\0' ||
        selection->kind < UMI_WORKBENCH_SELECTION_GENERIC ||
        selection->kind > UMI_WORKBENCH_SELECTION_MEDIA ||
        selection->activation < UMI_WORKBENCH_SELECTION_ACTIVATION_SELECT ||
        selection->activation > UMI_WORKBENCH_SELECTION_ACTIVATION_PREVIEW ||
        selection->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        selection->context_kind > UMI_CONTEXT_KIND_SELECTION ||
        selection->field_count > UMI_WORKBENCH_SELECTION_MAX_FIELDS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->field_count; ++index) {
        /* Apply this operation only while the related capability or state is available. */
        if (umi_workbench_selection_field_validate(
                &selection->fields[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection refresh hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_refresh_hash(
    UmiWorkbenchSelection *selection)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return 0U;

    hash = umi_workbench_selection_hash_text(
        hash, selection->application_id, sizeof(selection->application_id));
    hash = umi_workbench_selection_hash_text(
        hash, selection->panel_id, sizeof(selection->panel_id));
    hash = umi_workbench_selection_hash_text(
        hash, selection->workspace_id, sizeof(selection->workspace_id));
    hash = umi_workbench_selection_hash_text(
        hash, selection->subject_id, sizeof(selection->subject_id));
    hash = umi_workbench_selection_hash_text(
        hash, selection->secondary_id, sizeof(selection->secondary_id));
    hash = umi_workbench_selection_hash_text(
        hash, selection->path, sizeof(selection->path));
    hash ^= (uint64_t)selection->kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)selection->context_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)selection->line;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)selection->column;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)selection->selection_length;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)selection->signed_value;
    hash *= UINT64_C(1099511628211);
    hash ^= selection->unsigned_value;
    hash *= UINT64_C(1099511628211);
    hash ^= selection->boolean_value ? UINT64_C(1) : UINT64_C(0);
    hash *= UINT64_C(1099511628211);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->field_count; ++index) {
        const UmiWorkbenchSelectionField *field = &selection->fields[index];
        hash = umi_workbench_selection_hash_text(
            hash, field->name, sizeof(field->name));
        hash ^= (uint64_t)field->kind;
        hash *= UINT64_C(1099511628211);
        /* Apply this branch only when its contract condition is satisfied. */
        if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_TEXT) {
            hash = umi_workbench_selection_hash_text(
                hash, field->text, sizeof(field->text));
        } else /* Apply this branch only when its contract condition is satisfied. */ if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_INTEGER) {
            hash ^= (uint64_t)field->integer_value;
            hash *= UINT64_C(1099511628211);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_UNSIGNED) {
            hash ^= field->unsigned_value;
            hash *= UINT64_C(1099511628211);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_DECIMAL) {
            uint64_t bits = 0U;
            memcpy(&bits, &field->decimal_value, sizeof(bits));
            hash ^= bits;
            hash *= UINT64_C(1099511628211);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_BOOLEAN) {
            hash ^= field->boolean_value ? UINT64_C(1) : UINT64_C(0);
            hash *= UINT64_C(1099511628211);
        }
    }

    selection->content_hash = hash;
    return hash;
}
