/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_capabilities/test_developer_capabilities_08.c
 *
 * PURPOSE:
 *   Verify developer capability descriptor partition 08 and its public contract.
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
    descriptor = umi_developer_capability_attach_process();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_ATTACH_PROCESS) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_elevation_policy();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_ELEVATION_POLICY) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_environment_overlay();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_ENVIRONMENT_OVERLAY) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_argument_quoting();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_ARGUMENT_QUOTING) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_run_readiness();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_RUN_READINESS) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("developer capability partition 08 passed\n");
    return 0;
}
