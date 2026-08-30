/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_capabilities/test_developer_capabilities_01.c
 *
 * PURPOSE:
 *   Verify developer capability descriptor partition 01 and its public contract.
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
    descriptor = umi_developer_capability_workspace_discovery();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_WORKSPACE_DISCOVERY) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_profile_selection();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_PROFILE_SELECTION) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_preset_import();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_PRESET_IMPORT) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_environment_probe();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_ENVIRONMENT_PROBE) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_toolchain_detection();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_TOOLCHAIN_DETECTION) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("developer capability partition 01 passed\n");
    return 0;
}
