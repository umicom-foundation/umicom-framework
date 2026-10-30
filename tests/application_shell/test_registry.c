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
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "umicom/application_shell/registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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


    /* Presentation order is independent of registration order and never
     * rewrites the registry. Extensions with equal priorities use stable IDs. */
    const char *ids[] = {"menu.save", "menu.new", "menu.open", "menu.exit"};
    const int orders[] = {20, INT_MIN, 20, INT_MAX};
    for (size_t i = 0U; i < 4U; ++i) {
        umi_application_shell_contribution_init(&item, ids[i], ids[i],
            UMI_APPLICATION_SHELL_ROLE_MENU_ITEM, UMI_APPLICATION_SHELL_REGION_MENU_BAR);
        item.order = orders[i];
        assert(umi_application_shell_registry_upsert(registry, &item) == UMI_STATUS_OK);
    }
    uint64_t revision = umi_application_shell_registry_revision(registry);
    size_t indices[4] = {99U, 99U, 99U, 99U}, count = 0U;
    assert(umi_application_shell_registry_ordered_indices(registry, NULL, 0U, &count)
        == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(count == 4U);
    assert(umi_application_shell_registry_ordered_indices(registry, indices, 3U, &count)
        == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(indices[0] == 99U && indices[3] == 99U);
    assert(umi_application_shell_registry_ordered_indices(registry, indices, 4U, &count) == UMI_STATUS_OK);
    assert(indices[0] == 1U && indices[1] == 2U && indices[2] == 0U && indices[3] == 3U);
    assert(umi_application_shell_registry_revision(registry) == revision);
    assert(umi_application_shell_registry_at(registry, 0U, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.contribution_id, "menu.save") == 0);
    assert(umi_application_shell_registry_ordered_indices(registry, NULL, 4U, &count)
        == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_application_shell_registry_ordered_indices(registry, indices, 4U, NULL)
        == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_application_shell_registry_ordered_indices(NULL, indices, 4U, &count)
        == UMI_STATUS_INVALID_ARGUMENT);
    /* Updates and removals must not leave stale positions in the next view. */
    assert(umi_application_shell_registry_find(registry, "menu.exit", &item) == UMI_STATUS_OK);
    item.order = INT_MIN;
    assert(umi_application_shell_registry_upsert(registry, &item) == UMI_STATUS_OK);
    assert(umi_application_shell_registry_remove(registry, "menu.open") == UMI_STATUS_OK);
    assert(umi_application_shell_registry_ordered_indices(registry, indices, 4U, &count) == UMI_STATUS_OK);
    assert(count == 3U);
    assert(umi_application_shell_registry_at(registry, indices[0], &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.contribution_id, "menu.exit") == 0);
    for (size_t i = 0U; i < 4U; ++i) (void)umi_application_shell_registry_remove(registry, ids[i]);
    assert(umi_application_shell_registry_ordered_indices(registry, NULL, 0U, &count) == UMI_STATUS_OK);
    assert(count == 0U);
    umi_application_shell_registry_destroy(registry);
    return 0;
}
