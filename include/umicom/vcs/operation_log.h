/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/operation_log.h
 *
 * PURPOSE:
 *   Publish the public operation log contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_OPERATION_LOG_H
#define UMICOM_VCS_OPERATION_LOG_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs operation log data shared with callers of this public contract.
 */
typedef struct UmiVcsOperationLog UmiVcsOperationLog;
/**
 * Initialise vcs operation log from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_operation_log_create(UmiVcsOperationLog **out_log);
/**
 * Release or reset state held by vcs operation log so the same storage can be reused
 * safely.
 */
void umi_vcs_operation_log_destroy(UmiVcsOperationLog *log);
/**
 * Release or reset state held by vcs operation log so the same storage can be reused
 * safely.
 */
void umi_vcs_operation_log_clear(UmiVcsOperationLog *log);
/**
 * Provide the vcs operation log begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_operation_log_begin(UmiVcsOperationLog *log, UmiVcsOperationKind kind, const char *subject, uint64_t *out_operation_id);
/**
 * Provide the vcs operation log finish operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_operation_log_finish(UmiVcsOperationLog *log, uint64_t operation_id, UmiStatus status, const char *summary);
/**
 * Return the number of records represented by vcs operation log without changing their
 * state.
 */
size_t umi_vcs_operation_log_count(const UmiVcsOperationLog *log);
/**
 * Find vcs operation log while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsOperation *umi_vcs_operation_log_at(const UmiVcsOperationLog *log, size_t index);
/**
 * Find vcs operation log while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsOperation *umi_vcs_operation_log_find(const UmiVcsOperationLog *log, uint64_t operation_id);
#ifdef __cplusplus
}
#endif
#endif
