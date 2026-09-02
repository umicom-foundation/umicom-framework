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
        UmiApplicationShellContribution contribution;

        status = umi_application_shell_registry_find(
            bindings->shell_registry,
            item->item_id,
            &contribution);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) {
            umi_application_shell_contribution_init(
                &contribution,
                item->item_id,
                item->text,
                UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
                UMI_APPLICATION_SHELL_REGION_STATUS_BAR);

            contribution.flags =
                UMI_APPLICATION_SHELL_VISIBLE |
                UMI_APPLICATION_SHELL_ENABLED;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) {
            return status;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strlen(item->text) >= sizeof(contribution.title) ||
            strlen(item->text) >= sizeof(contribution.description) ||
            strlen(item->command_id) >= sizeof(contribution.command_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)strcpy(contribution.title, item->text);
        (void)strcpy(contribution.description, item->text);
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
