/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_capabilities/test_developer_capabilities_06.c
 *
 * PURPOSE:
 *   Verify developer capability descriptor partition 06 and its public contract.
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
    descriptor = umi_developer_capability_test_retry();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_TEST_RETRY) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_test_fixtures();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_TEST_FIXTURES) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_coverage_capture();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_COVERAGE_CAPTURE) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_machine_reports();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_MACHINE_REPORTS) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_continuous_testing();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_CONTINUOUS_TESTING) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("developer capability partition 06 passed\n");
    return 0;
}
