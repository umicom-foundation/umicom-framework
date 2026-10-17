/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/status_sync.c
 *
 * PURPOSE:
 *   Implement status text/badge synchronization into Application Shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/status_sync.h"

#include <string.h>

/*
 * A closed Studio workspace legitimately has no project identity. Likewise,
 * before opening a document, language and encoding are not yet known. Preserve
 * the authoritative empty values in UmiStudioRuntimeStatusModel: only the
 * Application Shell presentation title needs a non-empty human-readable value.
 * The Shell's contribution validator rejects empty titles. Normal status
 * values are passed through without modification.
 */
static const char *status_display_text(const UmiStudioRuntimeStatusItem *item)
{
    if (item->text[0] != '\0') return item->text;

    switch (item->kind) {
        case UMI_STUDIO_STATUS_WORKSPACE: return "No workspace open";
        case UMI_STUDIO_STATUS_LANGUAGE: return "No document language";
        case UMI_STUDIO_STATUS_ENCODING: return "No document encoding";
        default: return "Unavailable";
    }
}

/*
 * Provide the studio status sync operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_status_sync(
    UmiStudioRuntimeBindings *bindings,
    const UmiStudioRuntimeStatusModel *model)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->item_count; ++index) {
        const UmiStudioRuntimeStatusItem *item = &model->items[index];
        /* Keep status state unchanged; Shell presentation requires a title. */
        const char *display_text = status_display_text(item);
        UmiApplicationShellContribution contribution;

        status = umi_application_shell_registry_find(
            bindings->shell_registry,
            item->item_id,
            &contribution);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) {
            /* Superseded: an absent observation produced an invalid empty
             * Shell title and prevented Studio GTK4 from starting. Preserve
             * the original implementation here for engineering comparison. */
#if 0
            umi_application_shell_contribution_init(
                &contribution,
                item->item_id,
                item->text,
                UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
                UMI_APPLICATION_SHELL_REGION_STATUS_BAR);
#endif
            umi_application_shell_contribution_init(
                &contribution,
                item->item_id,
                display_text,
                UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
                UMI_APPLICATION_SHELL_REGION_STATUS_BAR);

            contribution.flags =
                UMI_APPLICATION_SHELL_VISIBLE |
                UMI_APPLICATION_SHELL_ENABLED;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) {
            return status;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        /* Superseded: the capacity guard checked the raw empty observation.
         * The displayed caption is now the value to validate. Keep the old
         * condition disabled for explicit review, not deletion. */
#if 0
        if (strlen(item->text) >= sizeof(contribution.title) ||
            strlen(item->text) >= sizeof(contribution.description) ||
            strlen(item->command_id) >= sizeof(contribution.command_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
#endif
        if (strlen(display_text) >= sizeof(contribution.title) ||
            strlen(display_text) >= sizeof(contribution.description) ||
            strlen(item->command_id) >= sizeof(contribution.command_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        /* Superseded: copying empty status text into the required Shell
         * contribution title caused UMI_STATUS_INVALID_ARGUMENT. The original
         * assignments remain disabled for engineering review. */
#if 0
        (void)strcpy(contribution.title, item->text);
        (void)strcpy(contribution.description, item->text);
#endif
        (void)strcpy(contribution.title, display_text);
        (void)strcpy(contribution.description, display_text);
        (void)strcpy(contribution.command_id, item->command_id);
        contribution.badge_count = item->badge_count;
        contribution.revision += 1U;

        /* Apply this operation only while the related capability or state is available. */
        if (item->visible) {
            contribution.flags |= UMI_APPLICATION_SHELL_VISIBLE;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            contribution.flags &= (UmiApplicationShellFlags)~(UmiApplicationShellFlags)UMI_APPLICATION_SHELL_VISIBLE;
        }

        status = umi_application_shell_registry_upsert(
            bindings->shell_registry,
            &contribution);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
