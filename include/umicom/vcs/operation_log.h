/* Umicom Framework VCS operation journal. Created by Sammy Hegab, Umicom Foundation. MIT. */
#ifndef UMICOM_VCS_OPERATION_LOG_H
#define UMICOM_VCS_OPERATION_LOG_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsOperationLog UmiVcsOperationLog;
UmiStatus umi_vcs_operation_log_create(UmiVcsOperationLog **out_log);
void umi_vcs_operation_log_destroy(UmiVcsOperationLog *log);
void umi_vcs_operation_log_clear(UmiVcsOperationLog *log);
UmiStatus umi_vcs_operation_log_begin(UmiVcsOperationLog *log, UmiVcsOperationKind kind, const char *subject, uint64_t *out_operation_id);
UmiStatus umi_vcs_operation_log_finish(UmiVcsOperationLog *log, uint64_t operation_id, UmiStatus status, const char *summary);
size_t umi_vcs_operation_log_count(const UmiVcsOperationLog *log);
const UmiVcsOperation *umi_vcs_operation_log_at(const UmiVcsOperationLog *log, size_t index);
const UmiVcsOperation *umi_vcs_operation_log_find(const UmiVcsOperationLog *log, uint64_t operation_id);
#ifdef __cplusplus
}
#endif
#endif
