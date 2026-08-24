/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/status_sync.c
 *
 * PURPOSE:
 *   Implement status text/badge synchronization into Application Shell.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/status_sync.h"

#include <string.h>

UmiStatus umi_studio_status_sync(
    UmiStudioRuntimeBindings *bindings,
    const UmiStudioRuntimeStatusModel *model)
{
    size_t index;
    UmiStatus status;

    if (bindings == NULL || model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < model->item_count; ++index) {
        const UmiStudioRuntimeStatusItem *item = &model->items[index];
        UmiApplicationShellContribution contribution;

        status = umi_application_shell_registry_find(
            bindings->shell_registry,
            item->item_id,
            &contribution);

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
        } else if (status != UMI_STATUS_OK) {
            return status;
        }

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

        if (item->visible) {
            contribution.flags |= UMI_APPLICATION_SHELL_VISIBLE;
        } else {
            contribution.flags &= (UmiApplicationShellFlags)~(UmiApplicationShellFlags)UMI_APPLICATION_SHELL_VISIBLE;
        }

        status = umi_application_shell_registry_upsert(
            bindings->shell_registry,
            &contribution);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
