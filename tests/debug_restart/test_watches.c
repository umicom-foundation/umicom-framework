/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_restart/test_watches.c
 * PURPOSE: Preserve watch expressions while retiring evaluated evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug/watch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
int main(void)
{
    UmiDebugWatchRegistry *registry = NULL;
    CHECK(UmiDebugWatchRegistryInvalidateValues(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_debug_watch_registry_create(&registry) == UMI_STATUS_OK);
    uint64_t revision = umi_debug_watch_registry_revision(registry);
    CHECK(UmiDebugWatchRegistryInvalidateValues(registry) == UMI_STATUS_OK);
    CHECK(umi_debug_watch_registry_revision(registry) == revision);
    UmiDebugWatchSnapshot row = {0};
    strcpy(row.id, "enabled");
    strcpy(row.expression, "items.size");
    strcpy(row.value, "12");
    strcpy(row.type, "unsigned");
    strcpy(row.session_id, "prior");
    row.enabled = 1;
    row.valid = 1;
    CHECK(umi_debug_watch_registry_upsert(registry, &row) == UMI_STATUS_OK);
    strcpy(row.id, "disabled");
    row.enabled = 0;
    CHECK(umi_debug_watch_registry_upsert(registry, &row) == UMI_STATUS_OK);
    revision = umi_debug_watch_registry_revision(registry);
    CHECK(UmiDebugWatchRegistryInvalidateValues(registry) == UMI_STATUS_OK);
    CHECK(umi_debug_watch_registry_revision(registry) == revision + 1U);
    CHECK(umi_debug_watch_registry_count(registry) == 2U);
    for (size_t index = 0U; index < 2U; ++index)
    {
        CHECK(umi_debug_watch_registry_at(registry, index, &row) == UMI_STATUS_OK);
        CHECK(strcmp(row.id, index == 0U ? "enabled" : "disabled") == 0);
        CHECK(row.enabled == (index == 0U) && !row.valid);
        CHECK(strcmp(row.expression, "items.size") == 0);
        CHECK(row.value[0] == '\0' && row.type[0] == '\0' && row.session_id[0] == '\0');
        CHECK(row.revision == revision + 1U);
    }
    umi_debug_watch_registry_destroy(registry);
    return 0;
}
