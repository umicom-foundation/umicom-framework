/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/comparison_cases.h
 *
 * PURPOSE:
 *   Exercise caller-owned canonical comparison through each registered native record fixture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_RECORD_COMPARISON_CASES_H
#define UMICOM_TEST_RECORD_COMPARISON_CASES_H
#include "umicom/base/value_archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_COMPARE_REQUIRE(Expression) do { if (!(Expression)) { \
    (void)fprintf(stderr, "%s:%d: record comparison: %s\n", __FILE__, __LINE__, #Expression); \
    goto comparison_done; \
} } while (0)

/* This fixture family already has bounded id and revision fields. Use its
 * actual initialized value and tail-filler, rather than inventing a second
 * schema. Mutations distinguish encoded state from padding and unused bytes. */
#define UMI_TEST_RECORD_COMPARISON(Name, Type, Encode, Compare, Tails) \
static int Name(const Type *sample) \
{ \
    int result = 1; \
    Type *left = malloc(sizeof(*left)), *right = malloc(sizeof(*right)); \
    unsigned char *before_left = malloc(sizeof(*left)); \
    unsigned char *before_right = malloc(sizeof(*right)); \
    unsigned char *scratch = NULL, *before_scratch = NULL; \
    UmiValueArchiveComparison observation = {0}, before, measured; \
    size_t encoded_size = 0U, scratch_size = 0U; \
    UMI_COMPARE_REQUIRE(left != NULL && right != NULL && before_left != NULL && before_right != NULL); \
    memcpy(left, sample, sizeof(*left)); memcpy(right, sample, sizeof(*right)); \
    UMI_COMPARE_REQUIRE(Encode(sample, NULL, 0U, &encoded_size) == UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(Compare(left, right, NULL, 0U, &measured) == UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(measured.relation == UMI_VALUE_ARCHIVE_NOT_COMPARED); \
    UMI_COMPARE_REQUIRE(measured.left_archive_size == encoded_size && measured.right_archive_size == encoded_size); \
    UMI_COMPARE_REQUIRE(measured.required_workspace == encoded_size * 2U); \
    scratch_size = measured.required_workspace; \
    scratch = malloc(scratch_size + 1U); before_scratch = malloc(scratch_size + 1U); \
    UMI_COMPARE_REQUIRE(scratch != NULL && before_scratch != NULL); \
    memset(scratch, 0xa5, scratch_size + 1U); memcpy(before_scratch, scratch, scratch_size + 1U); \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size - 1U, &observation) == UMI_STATUS_CAPACITY_EXCEEDED); \
    UMI_COMPARE_REQUIRE(observation.required_workspace == scratch_size && observation.relation == UMI_VALUE_ARCHIVE_NOT_COMPARED); \
    UMI_COMPARE_REQUIRE(memcmp(scratch, before_scratch, scratch_size + 1U) == 0); \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, 0U, &observation) == UMI_STATUS_CAPACITY_EXCEEDED); \
    UMI_COMPARE_REQUIRE(memcmp(scratch, before_scratch, scratch_size + 1U) == 0); \
    memset(&observation, 0x5a, sizeof(observation)); memcpy(&before, &observation, sizeof(before)); \
    UMI_COMPARE_REQUIRE(Compare(NULL, right, scratch, scratch_size, &observation) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_COMPARE_REQUIRE(Compare(left, NULL, scratch, scratch_size, &observation) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size, NULL) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_COMPARE_REQUIRE(Compare(left, right, NULL, 1U, &observation) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_COMPARE_REQUIRE(Compare(left, right, left, 1U, &observation) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_COMPARE_REQUIRE(Compare(left, right, right, 1U, &observation) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size, (UmiValueArchiveComparison *)left) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size, (UmiValueArchiveComparison *)right) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size, (UmiValueArchiveComparison *)scratch) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_COMPARE_REQUIRE(memcmp(&observation, &before, sizeof(before)) == 0); \
    UMI_COMPARE_REQUIRE(memcmp(scratch, before_scratch, scratch_size + 1U) == 0); \
    memcpy(before_left, left, sizeof(*left)); memcpy(before_right, right, sizeof(*right)); \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size, &observation) == UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(observation.relation == UMI_VALUE_ARCHIVE_EQUAL && scratch[scratch_size] == 0xa5); \
    UMI_COMPARE_REQUIRE(Compare(left, left, scratch, scratch_size, &observation) == UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(observation.relation == UMI_VALUE_ARCHIVE_EQUAL); \
    UMI_COMPARE_REQUIRE(memcmp(left, before_left, sizeof(*left)) == 0 && memcmp(right, before_right, sizeof(*right)) == 0); \
    Tails(right); \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size, &observation) == UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(observation.relation == UMI_VALUE_ARCHIVE_EQUAL); \
    UMI_COMPARE_REQUIRE(right->revision < UINT64_MAX); \
    ++right->revision; \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size, &observation) == UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(observation.relation == UMI_VALUE_ARCHIVE_DIFFERENT); \
    UMI_COMPARE_REQUIRE(Compare(right, left, scratch, scratch_size, &observation) == UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(observation.relation == UMI_VALUE_ARCHIVE_DIFFERENT); \
    memcpy(right, sample, sizeof(*right)); \
    size_t id_length = strlen(right->id); \
    UMI_COMPARE_REQUIRE(id_length + 1U < sizeof(right->id)); \
    right->id[id_length] = 'x'; right->id[id_length + 1U] = '\0'; \
    UMI_COMPARE_REQUIRE(Compare(left, right, NULL, 0U, &measured) == UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(measured.relation == UMI_VALUE_ARCHIVE_NOT_COMPARED); \
    UMI_COMPARE_REQUIRE(measured.right_archive_size == measured.left_archive_size + 1U); \
    unsigned char *larger = realloc(scratch, measured.required_workspace + 1U); \
    UMI_COMPARE_REQUIRE(larger != NULL); scratch = larger; scratch_size = measured.required_workspace; \
    scratch[scratch_size] = 0xa5; \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size, &observation) == UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(observation.relation == UMI_VALUE_ARCHIVE_DIFFERENT && scratch[scratch_size] == 0xa5); \
    memset(right->id, 'x', sizeof(right->id)); \
    memcpy(before_right, right, sizeof(*right)); \
    memcpy(&before, &observation, sizeof(before)); \
    UMI_COMPARE_REQUIRE(Compare(left, right, scratch, scratch_size, &observation) != UMI_STATUS_OK); \
    UMI_COMPARE_REQUIRE(memcmp(&observation, &before, sizeof(before)) == 0); \
    UMI_COMPARE_REQUIRE(memcmp(right, before_right, sizeof(*right)) == 0); \
    UMI_COMPARE_REQUIRE(memcmp(left, before_left, sizeof(*left)) == 0); \
    result = 0; \
comparison_done: \
    free(before_scratch); free(scratch); free(before_right); free(before_left); free(right); free(left); \
    return result; \
}
#endif
