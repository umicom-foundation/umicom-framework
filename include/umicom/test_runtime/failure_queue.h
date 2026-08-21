/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_queue.h
 *
 * PURPOSE:
 *   Maintain ordered unresolved regression failures.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_QUEUE
#define UMICOM_TEST_RUNTIME_FAILURE_QUEUE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeFailureQueue
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t pending_count;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeFailureQueue;
void umi_test_runtime_failure_queue_init(UmiTestRuntimeFailureQueue *value,const char *id);
UmiStatus umi_test_runtime_failure_queue_validate(const UmiTestRuntimeFailureQueue *value);
UmiStatus umi_test_runtime_failure_queue_set_detail(UmiTestRuntimeFailureQueue *value,const char *detail);
UmiStatus umi_test_runtime_failure_queue_set_pending_count(UmiTestRuntimeFailureQueue *value,uint64_t number);
UmiStatus umi_test_runtime_failure_queue_set_generation(UmiTestRuntimeFailureQueue *value,uint64_t number);
bool umi_test_runtime_failure_queue_same_identity(const UmiTestRuntimeFailureQueue *left,const UmiTestRuntimeFailureQueue *right);
#ifdef __cplusplus
}
#endif
#endif
