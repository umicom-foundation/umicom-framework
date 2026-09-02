/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/operation.h
 *
 * PURPOSE:
 *   Record bounded application-runtime operations for deterministic diagnostics and future durable audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_OPERATION_H
#define UMICOM_APPLICATION_RUNTIME_OPERATION_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application operation data shared with callers of this public contract.
 */
typedef struct UmiApplicationOperation {
    uint64_t sequence;
    UmiApplicationOperationKind kind;
    const char *target_id;
    UmiStatus result;
} UmiApplicationOperation;

/**
 * Represent the application operation log data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationOperationLog {
    uint32_t structure_size;
    UmiApplicationOperation operations[UMI_APPLICATION_RUNTIME_MAX_OPERATIONS];
    size_t operation_count;
    uint64_t next_sequence;
} UmiApplicationOperationLog;

/**
 * Initialise application operation log from caller-provided values so later operations
 * receive a known state.
 */
void umi_application_operation_log_init(UmiApplicationOperationLog *log);
/**
 * Provide the application operation log record operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_operation_log_record(
    UmiApplicationOperationLog *log,
    UmiApplicationOperationKind kind,
    const char *target_id,
    UmiStatus result);
/**
 * Provide the application operation log last operation used by this module and its client
 * applications.
 */
const UmiApplicationOperation *umi_application_operation_log_last(
    const UmiApplicationOperationLog *log);

#ifdef __cplusplus
}
#endif

#endif
