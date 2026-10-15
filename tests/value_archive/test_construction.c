/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_construction.c
 * PURPOSE: Exercise candidate ownership, refusal and allocation failure in the shared constructor.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "construction_cases.h"
#include "../../src/base/value_construction_internal.h"

typedef struct ConstructionSample {
    char name[16];
    unsigned marker;
} ConstructionSample;
static bool refuse_allocation;
static unsigned allocations;
static unsigned releases;
static unsigned builds;

/* The allocator substitution is confined to this translation unit. Production
 * services retain the C allocator and expose no mutable test hook. */
static void *ConstructionAllocate(size_t size)
{
    ++allocations;
    return refuse_allocation ? NULL : malloc(size);
}
static void ConstructionRelease(void *memory)
{
    if (memory != NULL) ++releases;
    free(memory);
}
static UmiStatus ConstructionBuild(ConstructionSample *value,
    const char *name, bool refuse_value)
{
    ++builds;
    memset(value, 0, sizeof(*value));
    value->marker = 42U;
    if (name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(name) >= sizeof(value->name)) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(value->name, name, strlen(name) + 1U);
    return refuse_value ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}
#define malloc ConstructionAllocate
#define free ConstructionRelease
UMI_DEFINE_STAGED_VALUE_INIT(ConstructionInit, ConstructionSample, destination,
    (ConstructionSample *destination, const char *name, bool refuse_value),
    ConstructionBuild(candidate, name, refuse_value))
#undef free
#undef malloc

int main(void)
{
    ConstructionSample value = { "accepted", 7U };
    unsigned char before[sizeof(value)];
    int result = 1;
    memcpy(before, &value, sizeof(value));

    /* Missing destinations must not allocate or invoke the domain builder. */
    UMI_CONSTRUCTION_REQUIRE(ConstructionInit(NULL, "unused", false) ==
        UMI_STATUS_INVALID_ARGUMENT);
    UMI_CONSTRUCTION_REQUIRE(allocations == 0U && builds == 0U && releases == 0U);
    refuse_allocation = true;
    UMI_CONSTRUCTION_REQUIRE(ConstructionInit(&value, "replacement", false) ==
        UMI_STATUS_OUT_OF_MEMORY);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, &value, sizeof(value)) == 0);
    UMI_CONSTRUCTION_REQUIRE(allocations == 1U && builds == 0U && releases == 0U);

    /* Even a builder that writes before rejecting must not publish those bytes. */
    refuse_allocation = false;
    UMI_CONSTRUCTION_REQUIRE(ConstructionInit(&value, "replacement", true) ==
        UMI_STATUS_INVALID_STATE);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, &value, sizeof(value)) == 0);
    UMI_CONSTRUCTION_REQUIRE(allocations == 2U && builds == 1U && releases == 1U);
    UMI_CONSTRUCTION_REQUIRE(ConstructionInit(&value, value.name + 2U, false) ==
        UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value.name, "cepted") == 0 && value.marker == 42U);
    UMI_CONSTRUCTION_REQUIRE(allocations == 3U && builds == 2U && releases == 2U);
    result = 0;
construction_cleanup:
    return result;
}
