/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_fingerprint.h
 *
 * PURPOSE:
 *   Build stable fingerprints for recurring failure grouping.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_FINGERPRINT
#define UMICOM_TEST_RUNTIME_FAILURE_FINGERPRINT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeFailureFingerprint
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t hash;
    uint64_t occurrence_count;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeFailureFingerprint;
void umi_test_runtime_failure_fingerprint_init(UmiTestRuntimeFailureFingerprint *value,const char *id);
UmiStatus umi_test_runtime_failure_fingerprint_validate(const UmiTestRuntimeFailureFingerprint *value);
UmiStatus umi_test_runtime_failure_fingerprint_set_detail(UmiTestRuntimeFailureFingerprint *value,const char *detail);
UmiStatus umi_test_runtime_failure_fingerprint_set_hash(UmiTestRuntimeFailureFingerprint *value,uint64_t number);
UmiStatus umi_test_runtime_failure_fingerprint_set_occurrence_count(UmiTestRuntimeFailureFingerprint *value,uint64_t number);
bool umi_test_runtime_failure_fingerprint_same_identity(const UmiTestRuntimeFailureFingerprint *left,const UmiTestRuntimeFailureFingerprint *right);
#ifdef __cplusplus
}
#endif
#endif
