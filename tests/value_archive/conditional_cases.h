/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/conditional_cases.h
 * PURPOSE: Exercise guarded restoration through each actual typed codec and domain fixture.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_CONDITIONAL_ARCHIVE_CASES_H
#define UMICOM_TEST_CONDITIONAL_ARCHIVE_CASES_H
#include "construction_cases.h"

/* Tests use public encode/decode functions and semantic equality from the
 * existing domain fixture. A whole-object byte comparison is used only when
 * refusal promises that the exact destination memory remains untouched. */
/* The former fixture assumed the malformed-input checker mutated its sample.
 * It only checks private copies. Retain that fixture for review; the active
 * definition below checks its result and verifies successful restoration. */
#if 0
#define UMI_TEST_CONDITIONAL_ARCHIVE(Name, Type, Encode, Restore, Equal, Tails, Malformed) \
static int Name(const Type *sample) \
{ \
    Type *value = malloc(sizeof(*value)), *expected = malloc(sizeof(*expected)); \
    unsigned char *before = malloc(sizeof(*value)), *bytes = NULL; \
    size_t size = 0U, written = 0U; \
    int result = 1; \
    if (value == NULL || expected == NULL || before == NULL) goto construction_cleanup; \
    memcpy(value, sample, sizeof(*value)); memcpy(expected, sample, sizeof(*expected)); \
    UMI_CONSTRUCTION_REQUIRE(Encode(sample, NULL, 0U, &size) == UMI_STATUS_OK); \
    bytes = malloc(size); \
    if (bytes == NULL) goto construction_cleanup; \
    UMI_CONSTRUCTION_REQUIRE(Encode(sample, bytes, size, &written) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(written == size); \
    memcpy(before, value, sizeof(*value)); \
    UMI_CONSTRUCTION_REQUIRE(Restore(NULL, size, expected, value) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, NULL, value) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, expected, NULL) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size - 1U, expected, value) != UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    bytes[size - 1U] ^= 1U; \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, expected, value) != UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    bytes[size - 1U] ^= 1U; \
    Malformed(expected); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, expected, value) != UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    memcpy(expected, sample, sizeof(*expected)); \
    /* Unused string tails are not encoded state and must not make a review stale. */ \
    Tails(expected); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, expected, value) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(Equal(sample, value)); \
    result = 0; \
construction_cleanup: \
    free(bytes); free(before); free(expected); free(value); \
    return result; \
}
#endif

#define UMI_TEST_CONDITIONAL_ARCHIVE(Name, Type, Encode, Restore, Equal, Tails, Malformed) \
static int Name(const Type *sample) \
{ \
    Type *value = malloc(sizeof(*value)), *expected = malloc(sizeof(*expected)); \
    unsigned char *before = malloc(sizeof(*value)), *bytes = NULL; \
    size_t size = 0U, written = 0U; \
    int result = 1; \
    if (value == NULL || expected == NULL || before == NULL) goto construction_cleanup; \
    memcpy(value, sample, sizeof(*value)); memcpy(expected, sample, sizeof(*expected)); \
    UMI_CONSTRUCTION_REQUIRE(Encode(sample, NULL, 0U, &size) == UMI_STATUS_OK); \
    bytes = malloc(size); \
    if (bytes == NULL) goto construction_cleanup; \
    UMI_CONSTRUCTION_REQUIRE(Encode(sample, bytes, size, &written) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(written == size); \
    memcpy(before, value, sizeof(*value)); \
    UMI_CONSTRUCTION_REQUIRE(Restore(NULL, size, expected, value) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, NULL, value) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, expected, NULL) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size - 1U, expected, value) != UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    bytes[size - 1U] ^= 1U; \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, expected, value) != UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0); \
    bytes[size - 1U] ^= 1U; \
    /* Malformed checks invalid private copies and returns zero on success. \
     * It does not mutate expected: a valid unchanged review must still restore. */ \
    UMI_CONSTRUCTION_REQUIRE(Malformed(expected) == 0); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, expected, value) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(Equal(sample, value)); \
    memcpy(expected, sample, sizeof(*expected)); \
    /* Unused string tails are not encoded state and must not make a review stale. */ \
    Tails(expected); \
    UMI_CONSTRUCTION_REQUIRE(Restore(bytes, size, expected, value) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(Equal(sample, value)); \
    result = 0; \
construction_cleanup: \
    free(bytes); free(before); free(expected); free(value); \
    return result; \
}
#endif
