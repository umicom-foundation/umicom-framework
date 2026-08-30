/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_capabilities/test_developer_capabilities_05.c
 *
 * PURPOSE:
 *   Verify developer capability descriptor partition 05 and its public contract.
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
    descriptor = umi_developer_capability_test_discovery();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_TEST_DISCOVERY) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_test_filtering();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_TEST_FILTERING) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_test_sharding();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_TEST_SHARDING) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_parallel_testing();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_PARALLEL_TESTING) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_test_timeout();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_TEST_TIMEOUT) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("developer capability partition 05 passed\n");
    return 0;
}
