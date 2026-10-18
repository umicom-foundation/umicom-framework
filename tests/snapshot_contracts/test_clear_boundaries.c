/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * PURPOSE: Exercise unreachable-by-counting revision boundaries in the shared
 * value-only clear implementation, without exposing private product layouts.
 *---------------------------------------------------------------------------*/
#include "../../src/base/snapshot_registry_internal.h"
#include <stdio.h>

typedef struct ClearBoundaryRegistry {
    unsigned char items[2][8];
    size_t count;
    uint64_t revision;
} ClearBoundaryRegistry;
UmiStatus ClearBoundary(ClearBoundaryRegistry *registry, uint64_t expected_revision);
UMI_DEFINE_SNAPSHOT_REGISTRY_CLEAR(ClearBoundary, ClearBoundaryRegistry, 2U)
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; } } while (0)

int main(void)
{
    ClearBoundaryRegistry owner = {0}, before;
    memset(owner.items, 0xa5, sizeof(owner.items));
    owner.count = 2U;
    owner.revision = UINT64_MAX;
    memcpy(&before, &owner, sizeof(owner));
    CHECK(ClearBoundary(&owner, UINT64_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&owner, &before, sizeof(owner)) == 0);
    CHECK(ClearBoundary(&owner, UINT64_MAX - 1U) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&owner, &before, sizeof(owner)) == 0);
    /* The last available revision is usable exactly once for a real clear. */
    owner.revision = UINT64_MAX - 1U;
    CHECK(ClearBoundary(&owner, owner.revision) == UMI_STATUS_OK);
    CHECK(owner.count == 0U && owner.revision == UINT64_MAX);
    for (size_t i = 0U; i < sizeof(owner.items); ++i)
        CHECK(((const unsigned char *)owner.items)[i] == 0U);
    CHECK(ClearBoundary(&owner, UINT64_MAX) == UMI_STATUS_OK);
    CHECK(owner.revision == UINT64_MAX);
    owner.count = 3U;
    memcpy(&before, &owner, sizeof(owner));
    CHECK(ClearBoundary(&owner, UINT64_MAX) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&owner, &before, sizeof(owner)) == 0);
    CHECK(ClearBoundary(NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
