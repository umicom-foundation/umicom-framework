/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_result_parser.c
 *
 * PURPOSE:
 *   Verify the result parser runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/result_parser.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Compare the documented default fields, not struct padding. */
static int RecordDefaultsEqual(const UmiTestRuntimeResultParser *left, const UmiTestRuntimeResultParser *right)
{
    return left->structure_size == right->structure_size &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->parsed_count == right->parsed_count &&
        left->rejected_count == right->rejected_count &&
        left->revision == right->revision &&
        left->active == right->active;
}
/* Public records may arrive from a caller's memory. Each fixed text field
 * must contain its own terminator; a later field cannot supply one for it. */
static int RecordRejectsUnterminatedFields(void)
{
    UmiTestRuntimeResultParser value;
    umi_test_runtime_result_parser_init(&value, "bounded-record");
    memset(value.id, 'x', sizeof(value.id));
    if (umi_test_runtime_result_parser_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_test_runtime_result_parser_init(&value, "bounded-record");
    memset(value.category, 'x', sizeof(value.category));
    if (umi_test_runtime_result_parser_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_test_runtime_result_parser_init(&value, "bounded-record");
    memset(value.detail, 'x', sizeof(value.detail));
    if (umi_test_runtime_result_parser_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    return 0;
}
#define RECORD_TYPE UmiTestRuntimeResultParser
#define RECORD_INIT umi_test_runtime_result_parser_init
#define RECORD_VALIDATE umi_test_runtime_result_parser_validate
#define RECORD_INIT_CHECKED umi_test_runtime_result_parser_init_checked
#include "../record_integrity/record_construction_cases.h"

/* Each mutation refusal must preserve the complete accepted value. These
 * checks remain active in optimized builds where assert may be disabled. */
static int RecordMutationCases(void)
{
    UmiTestRuntimeResultParser value;
    umi_test_runtime_result_parser_init(&value, "mutation-record");
    unsigned char before[sizeof(value)];
    umi_test_runtime_result_parser_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.category) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_result_parser_set_category(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_result_parser_set_category(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_result_parser_set_category(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_test_runtime_result_parser_set_category(&value, value.category + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.category, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.category) - 1U] = '\0';
        if (umi_test_runtime_result_parser_set_category(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.category, oversized) != 0) return 1;
    }
    umi_test_runtime_result_parser_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_result_parser_set_category(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_result_parser_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_result_parser_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_result_parser_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_result_parser_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_test_runtime_result_parser_set_detail(&value, value.detail + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.detail) - 1U] = '\0';
        if (umi_test_runtime_result_parser_set_detail(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, oversized) != 0) return 1;
    }
    umi_test_runtime_result_parser_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_result_parser_set_detail(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_result_parser_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_result_parser_set_parsed_count(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_result_parser_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_result_parser_set_rejected_count(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_test_runtime_result_parser_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_result_parser_set_active(&value, true) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    UmiTestRuntimeResultParser other;
    umi_test_runtime_result_parser_init(&value, "identity-record");
    umi_test_runtime_result_parser_init(&other, "identity-record");
    if (!umi_test_runtime_result_parser_same_identity(&value, &other)) return 1;
    memcpy(before, &value, sizeof(value));
    memset(other.id, 'x', sizeof(other.id));
    if (umi_test_runtime_result_parser_same_identity(&value, &other) || umi_test_runtime_result_parser_same_identity(&other, &value)) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

/* Use nonzero domain fields to expose a codec that accidentally drops
 * values. The existing initializer supplies required compatibility metadata. */
static UmiTestRuntimeResultParser ArchiveSample(void)
{
    UmiTestRuntimeResultParser value;
    umi_test_runtime_result_parser_init(&value, "archive-record");
    value.category[0] = 'a';
    value.detail[0] = 'a';
    value.parsed_count = (uint64_t)6U;
    value.rejected_count = (uint64_t)7U;
    value.revision = (uint64_t)8U;
    value.active = true;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiTestRuntimeResultParser *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->category) + 1U;
        memset(value->category + used, 0xa5, sizeof(value->category) - used);
    }
    {
        size_t used = strlen(value->detail) + 1U;
        memset(value->detail + used, 0xa5, sizeof(value->detail) - used);
    }
}
#define ARCHIVE_TYPE UmiTestRuntimeResultParser
#define ARCHIVE_ENCODE umi_test_runtime_result_parser_archive_encode
#define ARCHIVE_DECODE umi_test_runtime_result_parser_archive_decode
#define ARCHIVE_EQUAL RecordDefaultsEqual
#include "../value_archive/record_cases.h"

int main(void)
{
    if (ArchiveRecordCases() != 0) return 1;
    if (RecordMutationCases() != 0) return 1;
    if (RecordConstructionCases() != 0) return 1;
    UmiTestRuntimeResultParser value,same;
    uint64_t r;
    umi_test_runtime_result_parser_init(&value,"test-runtime.result_parser");
    assert(umi_test_runtime_result_parser_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_result_parser_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_result_parser_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_result_parser_set_parsed_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_result_parser_set_rejected_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_result_parser_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.parsed_count==13U&&value.rejected_count==21U);
    umi_test_runtime_result_parser_init(&same,"test-runtime.result_parser");
    assert(umi_test_runtime_result_parser_same_identity(&value,&same));
    return 0;
    }
