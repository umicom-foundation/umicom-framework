/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_inventory_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository inventory contract semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "../value_archive/construction_cases.h"
#include <stdlib.h>
#include <stdio.h>
#include "umicom/repository/inventory.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted submodule value. */
static int UmiRepositorySubmoduleConstructionCases(void)
{
    UmiRepositorySubmodule *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_repository_submodule_init(value, "framework", "framework", "", "main", 1) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_repository_submodule_init(value, oversized, "framework", "", "main", 1) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_repository_submodule_init(value, value->name, "framework", "", "main", 1) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "framework") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->url) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_repository_submodule_init(value, "framework", "framework", oversized, "main", 1) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_repository_submodule_init(value, "framework", "framework", value->url, "main", 1) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->url, "") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiRepositorySubmoduleConstructionCases() != 0) return 1;

    UmiRepositoryInventory *i = calloc(1U, sizeof(*i));
    UmiRepositorySubmodule s;
    assert(i != NULL);
    umi_repository_inventory_init(i);
    assert(umi_repository_submodule_init(&s, "framework", "framework", "", "main", 1) == UMI_STATUS_OK);
    assert(umi_repository_inventory_add(i, &s) == UMI_STATUS_OK);
    assert(umi_repository_inventory_find_path(i, "framework") != NULL);
    free(i);
    return 0;
}
