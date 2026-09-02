/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/drag_drop.h
 *
 * PURPOSE:
 *   Define portable drag/drop intent for moving, copying or linking reusable
 *   views/panels between shell regions and windows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_DRAG_DROP_H
#define UMICOM_APPLICATION_SHELL_DRAG_DROP_H

#include "umicom/application_shell/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named application shell drop operation values accepted by this public contract.
 */
typedef enum UmiApplicationShellDropOperation {
    UMI_APPLICATION_SHELL_DROP_MOVE = 1,
    UMI_APPLICATION_SHELL_DROP_COPY = 2,
    UMI_APPLICATION_SHELL_DROP_LINK = 3
} UmiApplicationShellDropOperation;

/**
 * Represent the application shell drop transaction data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationShellDropTransaction {
    char transaction_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char source_contribution_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char target_container_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    UmiApplicationShellRegion target_region;
    UmiApplicationShellDropOperation operation;
    size_t target_index;
    int accepted;
    char reason[UMI_APPLICATION_SHELL_TEXT_CAPACITY];
    uint64_t revision;
} UmiApplicationShellDropTransaction;

/**
 * Provide the application shell drop prepare operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_shell_drop_prepare(
    UmiApplicationShellDropTransaction *transaction,
    const char *transaction_id,
    const char *source_contribution_id,
    const char *target_container_id,
    UmiApplicationShellRegion target_region,
    UmiApplicationShellDropOperation operation,
    size_t target_index);

/**
 * Provide the application shell drop accept operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_shell_drop_accept(
    UmiApplicationShellDropTransaction *transaction);

/**
 * Provide the application shell drop reject operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_shell_drop_reject(
    UmiApplicationShellDropTransaction *transaction,
    const char *reason);

#ifdef __cplusplus
}
#endif

#endif
