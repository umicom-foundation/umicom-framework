/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/record_integrity/record_construction_cases.h
 * PURPOSE: Check typed record construction without erasing an accepted value.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RECORD_CONSTRUCTION_CASES_H
#define UMICOM_TEST_RECORD_CONSTRUCTION_CASES_H
#include <stdio.h>
#include <string.h>

/* These tests call each domain's real initializer and validator. The typed
 * fixture supplies a member-by-member comparison so padding never becomes
 * part of the public value contract. Failure checks compare bytes from the
 * same object because a refused operation promises not to write any of them. */
#define RECORD_REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)
static int RecordConstructionCases(void)
{
    RECORD_TYPE value, defaults;
    RECORD_INIT(&value, "retained-record");
    unsigned char before[sizeof(value)];
    memcpy(before, &value, sizeof(value));
    RECORD_REQUIRE(RECORD_INIT_CHECKED(NULL, "new-record") == UMI_STATUS_INVALID_ARGUMENT);
    RECORD_REQUIRE(RECORD_INIT_CHECKED(&value, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    RECORD_REQUIRE(memcmp(before, &value, sizeof(value)) == 0);
    RECORD_REQUIRE(RECORD_INIT_CHECKED(&value, "") == UMI_STATUS_INVALID_ARGUMENT);
    RECORD_REQUIRE(memcmp(before, &value, sizeof(value)) == 0);
    char oversized[sizeof(value.id) + 1U];
    memset(oversized, 'x', sizeof(oversized));
    oversized[sizeof(oversized) - 1U] = '\0';
    RECORD_REQUIRE(RECORD_INIT_CHECKED(&value, oversized) == UMI_STATUS_CAPACITY_EXCEEDED);
    RECORD_REQUIRE(memcmp(before, &value, sizeof(value)) == 0);
    /* Only this fixed array is readable. A bounded scan must reject it. */
    char unterminated[sizeof(value.id)];
    memset(unterminated, 'x', sizeof(unterminated));
    RECORD_REQUIRE(RECORD_INIT_CHECKED(&value, unterminated) == UMI_STATUS_CAPACITY_EXCEEDED);
    RECORD_REQUIRE(memcmp(before, &value, sizeof(value)) == 0);
    oversized[sizeof(value.id) - 1U] = '\0';
    RECORD_INIT(&defaults, oversized);
    RECORD_REQUIRE(RECORD_INIT_CHECKED(&value, oversized) == UMI_STATUS_OK);
    RECORD_REQUIRE(RECORD_VALIDATE(&value) == UMI_STATUS_OK);
    RECORD_REQUIRE(RecordDefaultsEqual(&value, &defaults));
    RECORD_REQUIRE(RECORD_INIT_CHECKED(&value, "aliased-record") == UMI_STATUS_OK);
    RECORD_INIT(&defaults, "liased-record");
    RECORD_REQUIRE(RECORD_INIT_CHECKED(&value, value.id + 1U) == UMI_STATUS_OK);
    RECORD_REQUIRE(RecordDefaultsEqual(&value, &defaults));
    RECORD_REQUIRE(RECORD_INIT_CHECKED(&value, value.id) == UMI_STATUS_OK);
    RECORD_REQUIRE(RecordDefaultsEqual(&value, &defaults));
    RECORD_REQUIRE(RecordRejectsUnterminatedFields() == 0);
    return 0;
}
#endif
