/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_capabilities/test_developer_capabilities_22.c
 *
 * PURPOSE:
 *   Verify developer capability descriptor partition 22 and its public contract.
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
    descriptor = umi_developer_capability_test_explorer();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_TEST_EXPLORER) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_terminal_panel();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_TERMINAL_PANEL) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_run_configurations();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_RUN_CONFIGURATIONS) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_deployment_centre();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_DEPLOYMENT_CENTRE) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    descriptor = umi_developer_capability_keyboard_navigation();
    assert(descriptor != NULL);
    assert(strcmp(descriptor->capability_id, UMI_DEVELOPER_CAPABILITY_KEYBOARD_NAVIGATION) == 0);
    assert(descriptor->category == UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE);
    assert(umi_developer_capability_descriptor_validate(
        descriptor, message, sizeof(message)) == UMI_STATUS_OK);
    (void)printf("developer capability partition 22 passed\n");
    return 0;
}
