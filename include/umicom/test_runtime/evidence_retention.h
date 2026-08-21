/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/evidence_retention.h
 *
 * PURPOSE:
 *   Define how long regression evidence remains queryable.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EVIDENCE_RETENTION
#define UMICOM_TEST_RUNTIME_EVIDENCE_RETENTION
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeEvidenceRetention {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t retention_days;
    uint64_t maximum_runs;
    uint64_t revision;
    bool active;
} UmiTestRuntimeEvidenceRetention;
void umi_test_runtime_evidence_retention_init(UmiTestRuntimeEvidenceRetention *value,const char *id);
UmiStatus umi_test_runtime_evidence_retention_validate(const UmiTestRuntimeEvidenceRetention *value);
UmiStatus umi_test_runtime_evidence_retention_set_category(UmiTestRuntimeEvidenceRetention *value,const char *category);
UmiStatus umi_test_runtime_evidence_retention_set_detail(UmiTestRuntimeEvidenceRetention *value,const char *detail);
UmiStatus umi_test_runtime_evidence_retention_set_retention_days(UmiTestRuntimeEvidenceRetention *value,uint64_t number);
UmiStatus umi_test_runtime_evidence_retention_set_maximum_runs(UmiTestRuntimeEvidenceRetention *value,uint64_t number);
UmiStatus umi_test_runtime_evidence_retention_set_active(UmiTestRuntimeEvidenceRetention *value,bool active);
bool umi_test_runtime_evidence_retention_same_identity(const UmiTestRuntimeEvidenceRetention *left,const UmiTestRuntimeEvidenceRetention *right);
#ifdef __cplusplus
}
#endif
#endif
