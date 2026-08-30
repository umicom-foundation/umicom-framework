/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_capabilities/test_developer_capabilities_04.c
 *
 * PURPOSE:
 *   Verify developer capability descriptor partition 04 and its public contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/developer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const UmiDeveloperCapabilityDescriptor *descriptor;
    char message[UMI_DEVELOPER_CAPABILITY_MESSAGE_CAPACITY];
    descriptor = umi_developer_capability_artifact_indexing();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_ARTIFACT_INDEXING) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_diagnostic_parsing();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_DIAGNOSTIC_PARSING) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_warning_policy();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_WARNING_POLICY) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_sanitizer_profile();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_SANITIZER_PROFILE) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_build_cancellation();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_BUILD_CANCELLATION) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("developer capability partition 04 passed\n");
    return 0;
}
