/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_json_allocation.c
 * PURPOSE: Fail each staged import allocation and prove complete rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "json_test_support.h"
static size_t calls, failAt;
static int Fail(void) { ++calls; return failAt != 0U && calls == failAt; }
void *JsonFaultMalloc(size_t size) { return Fail() ? NULL : malloc(size); }
void *JsonFaultCalloc(size_t count, size_t size) { return Fail() ? NULL : calloc(count, size); }
void *JsonFaultRealloc(void *pointer, size_t size) { return Fail() ? NULL : realloc(pointer, size); }
int main(void)
{
    char json[20000]; size_t used = 0U;
    int n = snprintf(json, sizeof(json), "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1,\"minor\":0},\"tests\":[");
    REQUIRE(n > 0); used = (size_t)n;
    for (size_t i = 0U; i < 150U; ++i) {
        n = snprintf(json + used, sizeof(json) - used, "%s{\"name\":\"notes.%zu\"}", i == 0U ? "" : ",", i);
        REQUIRE(n > 0 && (size_t)n < sizeof(json) - used); used += (size_t)n;
    }
    REQUIRE(used + 3U < sizeof(json)); memcpy(json + used, "]}", 3U);
    JsonTestState s; UmiTestPlatformCtestImportSummary summary;
    JsonTestCreate(&s); JsonTestSeed(&s); calls = 0U;
    REQUIRE(JsonTestImport(&s, json, &summary) == UMI_STATUS_OK);
    size_t successfulAllocations = calls;
    JsonTestDestroy(&s);
    for (size_t fault = 1U; fault <= successfulAllocations; ++fault) {
        failAt = 0U; JsonTestCreate(&s); JsonTestSeed(&s);
        uint64_t before = JsonTestFingerprint(&s);
        calls = 0U; failAt = fault;
        UmiStatus status = JsonTestImport(&s, json, &summary);
        failAt = 0U;
        REQUIRE(status == UMI_STATUS_OUT_OF_MEMORY && summary.discovered_count == 0U);
        REQUIRE(JsonTestFingerprint(&s) == before);
        JsonTestDestroy(&s);
    }
    printf("Injected failure at every one of %zu staged-import allocations; all records and revisions preserved.\n", successfulAllocations);
    return 0;
}
