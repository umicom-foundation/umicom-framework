/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gpt_inspection/test_allocation.c
 *
 * PURPOSE:
 *   Verify GPT array allocation refusal preserves output and releases partial ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
static unsigned allocation, refusal, live;
static void *ControlledAllocate(size_t size)
{
    ++allocation;
    if (allocation == refusal) return NULL;
    void *value = malloc(size);
    if (value != NULL) ++live;
    return value;
}
static void ControlledFree(void *value)
{
    if (value != NULL) --live;
    free(value);
}
/* Substitute only this object's allocations. The production parser is compiled
 * directly, so refusal coverage does not introduce a runtime failure switch. */
#define malloc ControlledAllocate
#define free ControlledFree
#include "../../src/image_inspection/gpt.c"
#undef free
#undef malloc

int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "first") && strcmp(argv[1], "second"))) return 2;
    GptFixture fixture;
    if (!FixtureCreate(&fixture, 512U, 128U, 128U)) return 1;
    refusal = strcmp(argv[1], "first") == 0 ? 1U : 2U;
    UmiGptImageInfo result, before;
    memset(&result, 0xa5, sizeof(result)); memcpy(&before, &result, sizeof(result));
    UmiStatus status = UmiGptImageInspect(FixtureRead, &fixture, GPT_FIXTURE_BYTES, 512U, &result);
    int failed = status != UMI_STATUS_OUT_OF_MEMORY || allocation != 2U || live != 0U ||
        fixture.calls != 2U || memcmp(&result, &before, sizeof(result)) != 0;
    /* A later inspection uses independent ownership after either refusal. */
    refusal = 0U; fixture.calls = 0U;
    status = UmiGptImageInspect(FixtureRead, &fixture, GPT_FIXTURE_BYTES, 512U, &result);
    failed |= status != UMI_STATUS_OK || result.partitions != 1U || live != 0U || fixture.calls != 4U;
    free(fixture.data);
    return failed;
}
