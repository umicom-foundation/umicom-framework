/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/drag_drop.c
 *
 * PURPOSE:
 *   Implement portable drag/drop transaction state. Toolkit adapters translate
 *   native pointer gestures into this Framework-owned intent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/drag_drop.h"

#include <string.h>

static int copy_text(char *destination,
                     size_t capacity,
                     const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U ||
        source == NULL || source[0] == '\0') {
        return 0;
    }

    length = strlen(source);
    if (length + 1U > capacity) return 0;

    (void)memcpy(destination, source, length + 1U);
    return 1;
}

UmiStatus umi_application_shell_drop_prepare(
    UmiApplicationShellDropTransaction *transaction,
    const char *transaction_id,
    const char *source_contribution_id,
    const char *target_container_id,
    UmiApplicationShellRegion target_region,
    UmiApplicationShellDropOperation operation,
    size_t target_index)
{
    if (transaction == NULL ||
        target_region < UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR ||
        target_region > UMI_APPLICATION_SHELL_REGION_WINDOW ||
        operation < UMI_APPLICATION_SHELL_DROP_MOVE ||
        operation > UMI_APPLICATION_SHELL_DROP_LINK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(transaction, 0, sizeof(*transaction));

    if (!copy_text(transaction->transaction_id,
                   sizeof(transaction->transaction_id),
                   transaction_id) ||
        !copy_text(transaction->source_contribution_id,
                   sizeof(transaction->source_contribution_id),
                   source_contribution_id) ||
        !copy_text(transaction->target_container_id,
                   sizeof(transaction->target_container_id),
                   target_container_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    transaction->target_region = target_region;
    transaction->operation = operation;
    transaction->target_index = target_index;
    transaction->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_shell_drop_accept(
    UmiApplicationShellDropTransaction *transaction)
{
    if (transaction == NULL || transaction->transaction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    transaction->accepted = 1;
    transaction->reason[0] = '\0';
    transaction->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_shell_drop_reject(
    UmiApplicationShellDropTransaction *transaction,
    const char *reason)
{
    size_t length;

    if (transaction == NULL || transaction->transaction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    transaction->accepted = 0;
    if (reason == NULL) reason = "Drop rejected.";

    length = strlen(reason);
    if (length >= sizeof(transaction->reason)) {
        length = sizeof(transaction->reason) - 1U;
    }
    (void)memcpy(transaction->reason, reason, length);
    transaction->reason[length] = '\0';
    transaction->revision += 1U;
    return UMI_STATUS_OK;
}
