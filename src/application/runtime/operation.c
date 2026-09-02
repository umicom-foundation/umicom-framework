/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/operation.c
 *
 * PURPOSE:
 *   Implement ordered bounded operation evidence without heap allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/operation.h"

#include <string.h>

/*
 * Initialise application operation log from caller-provided values so later operations
 * receive a known state.
 */
void umi_application_operation_log_init(UmiApplicationOperationLog *log)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return;
    memset(log, 0, sizeof(*log));
    log->structure_size = sizeof(*log);
    log->next_sequence = 1U;
}

/*
 * Provide the application operation log record operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_operation_log_record(
    UmiApplicationOperationLog *log,
    UmiApplicationOperationKind kind,
    const char *target_id,
    UmiStatus result)
{
    UmiApplicationOperation *operation;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || target_id == NULL || kind < UMI_APPLICATION_OPERATION_SESSION_START ||
        kind > UMI_APPLICATION_OPERATION_FEATURE_GATE)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (log->operation_count >= UMI_APPLICATION_RUNTIME_MAX_OPERATIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    operation = &log->operations[log->operation_count++];
    operation->sequence = log->next_sequence++;
    operation->kind = kind;
    operation->target_id = target_id;
    operation->result = result;
    return UMI_STATUS_OK;
}

/*
 * Provide the application operation log last operation used by this module and its client
 * applications.
 */
const UmiApplicationOperation *umi_application_operation_log_last(
    const UmiApplicationOperationLog *log)
{
    return log != NULL && log->operation_count > 0U
        ? &log->operations[log->operation_count - 1U] : NULL;
}
