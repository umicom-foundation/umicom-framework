/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_capabilities/test_developer_capabilities_20.c
 *
 * PURPOSE:
 *   Verify developer capability descriptor partition 20 and its public contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/developer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const UmiDeveloperCapabilityDescriptor *descriptor;
    char message[UMI_DEVELOPER_CAPABILITY_MESSAGE_CAPACITY];
    descriptor = umi_developer_capability_msbuild_provider();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_MSBUILD_PROVIDER) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_clang_toolchain();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_CLANG_TOOLCHAIN) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_gcc_toolchain();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_GCC_TOOLCHAIN) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_msvc_toolchain();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_MSVC_TOOLCHAIN) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_container_runner();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_CONTAINER_RUNNER) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("developer capability partition 20 passed\n");
    return 0;
}
