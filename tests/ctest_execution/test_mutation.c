/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_mutation.c
 *
 * PURPOSE:
 *   Exercise bounded report parsing with deterministic mutations and truncations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This is repeatable mutation coverage, not proof of correctness or a general
 * XML conformance test. Sanitizers make invalid memory accesses fail the run. */
int main(void)
{
    static const char seed[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<testsuite tests=\"1\"><testcase name=\"notes.save\" status=\"run\" time=\"0.001\">"
        "<system-out>Umicom &amp; Notes</system-out></testcase></testsuite>";
    unsigned checks = 0U, failures = 0U;
    UmiCtestReportSnapshot snapshot, before;
    memset(&before, 0x5A, sizeof before);
    /* Every strict prefix is incomplete and must leave caller state unchanged. */
    for (size_t size = 0U; size < sizeof seed - 1U; ++size) {
        snapshot = before;
        UmiStatus status = UmiCtestParseReport(seed, size, "notes.save", &snapshot);
        ++checks;
        if (status == UMI_STATUS_OK || memcmp(&snapshot, &before, sizeof before) != 0) ++failures;
    }
    uint32_t random = UINT32_C(0xC7E57021);
    for (unsigned iteration = 0U; iteration < 20000U; ++iteration) {
        char input[sizeof seed]; memcpy(input, seed, sizeof seed);
        random = random * UINT32_C(1664525) + UINT32_C(1013904223);
        size_t position = (size_t)random % (sizeof seed - 1U);
        random = random * UINT32_C(1664525) + UINT32_C(1013904223);
        input[position] = (char)(random >> 24U);
        snapshot = before;
        UmiStatus status = UmiCtestParseReport(input, sizeof seed - 1U, "notes.save", &snapshot);
        ++checks;
        if (status != UMI_STATUS_OK && memcmp(&snapshot, &before, sizeof before) != 0) ++failures;
        if (status == UMI_STATUS_OK && (snapshot.state != UMI_TEST_STATE_PASSED || snapshot.testcase_count != 1U)) ++failures;
    }
    printf("Mutation: %u checks, %u failures (seed 0xC7E57021)\n", checks, failures);
    return failures == 0U ? 0 : 1;
}
