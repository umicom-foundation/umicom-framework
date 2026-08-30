/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_registry.c
 *
 * PURPOSE:
 *   Verify contribution ownership, updates, runtime state and removal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/application_shell/registry.h"

int main(void)
{
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellContribution item;
    UmiApplicationShellContribution snapshot;

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);

    umi_application_shell_contribution_init(
        &item,
        "test.action",
        "Test Action",
        UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        UMI_APPLICATION_SHELL_REGION_MENU_BAR);
    (void)strcpy(item.command_id, "test.command");

    assert(umi_application_shell_registry_upsert(
        registry, &item) == UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) == 1U);

    assert(umi_application_shell_registry_set_state(
        registry, "test.action", 1, 0, 1, 3U) == UMI_STATUS_OK);
    assert(umi_application_shell_registry_find(
        registry, "test.action", &snapshot) == UMI_STATUS_OK);
    assert((snapshot.flags & UMI_APPLICATION_SHELL_ENABLED) == 0U);
    assert((snapshot.flags & UMI_APPLICATION_SHELL_CHECKED) != 0U);
    assert(snapshot.badge_count == 3U);

    assert(umi_application_shell_registry_remove(
        registry, "test.action") == UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) == 0U);

    umi_application_shell_registry_destroy(registry);
    return 0;
}
