/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/artifact_locator.h
 *
 * PURPOSE:
 *   Resolve test executables, logs and generated evidence from build roots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ARTIFACT_LOCATOR
#define UMICOM_TEST_RUNTIME_ARTIFACT_LOCATOR

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeArtifactLocator {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t resolved;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeArtifactLocator;

void umi_test_runtime_artifact_locator_init(UmiTestRuntimeArtifactLocator *value, const char *id);
UmiStatus umi_test_runtime_artifact_locator_validate(const UmiTestRuntimeArtifactLocator *value);
UmiStatus umi_test_runtime_artifact_locator_set_name(UmiTestRuntimeArtifactLocator *value, const char *name);
UmiStatus umi_test_runtime_artifact_locator_set_detail(UmiTestRuntimeArtifactLocator *value, const char *detail);
UmiStatus umi_test_runtime_artifact_locator_set_candidate_count(UmiTestRuntimeArtifactLocator *value, uint64_t number);
UmiStatus umi_test_runtime_artifact_locator_set_resolved(UmiTestRuntimeArtifactLocator *value, uint64_t number);
UmiStatus umi_test_runtime_artifact_locator_touch(UmiTestRuntimeArtifactLocator *value, uint64_t updated_at_ms);
bool umi_test_runtime_artifact_locator_same_identity(const UmiTestRuntimeArtifactLocator *left, const UmiTestRuntimeArtifactLocator *right);

#ifdef __cplusplus
}
#endif
#endif
