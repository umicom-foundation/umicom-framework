/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/bad_command_evidence.h
 *
 * PURPOSE:
 *   Retain evidence when CTest cannot start a registered test command.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_BAD_COMMAND_EVIDENCE
#define UMICOM_TEST_RUNTIME_BAD_COMMAND_EVIDENCE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeBadCommandEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t missing_dependency_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeBadCommandEvidence;

void umi_test_runtime_bad_command_evidence_init(UmiTestRuntimeBadCommandEvidence *value, const char *id);
UmiStatus umi_test_runtime_bad_command_evidence_validate(const UmiTestRuntimeBadCommandEvidence *value);
UmiStatus umi_test_runtime_bad_command_evidence_set_name(UmiTestRuntimeBadCommandEvidence *value, const char *name);
UmiStatus umi_test_runtime_bad_command_evidence_set_detail(UmiTestRuntimeBadCommandEvidence *value, const char *detail);
UmiStatus umi_test_runtime_bad_command_evidence_set_candidate_count(UmiTestRuntimeBadCommandEvidence *value, uint64_t number);
UmiStatus umi_test_runtime_bad_command_evidence_set_missing_dependency_count(UmiTestRuntimeBadCommandEvidence *value, uint64_t number);
UmiStatus umi_test_runtime_bad_command_evidence_touch(UmiTestRuntimeBadCommandEvidence *value, uint64_t updated_at_ms);
bool umi_test_runtime_bad_command_evidence_same_identity(const UmiTestRuntimeBadCommandEvidence *left, const UmiTestRuntimeBadCommandEvidence *right);

#ifdef __cplusplus
}
#endif
#endif
