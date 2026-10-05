/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_consumer_evidence.c
 *
 * PURPOSE:
 *   Verify the consumer evidence SDK/runtime contract.
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
#include "umicom/sdk_runtime/consumer_evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Compare the documented default fields, not struct padding. */
static int RecordDefaultsEqual(const UmiSdkRuntimeConsumerEvidence *left, const UmiSdkRuntimeConsumerEvidence *right)
{
    return left->structure_size == right->structure_size &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->path, right->path, sizeof(left->path)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->target_count == right->target_count &&
        left->runtime_file_count == right->runtime_file_count &&
        left->revision == right->revision &&
        left->state == right->state &&
        left->enabled == right->enabled;
}
/* Public records may arrive from a caller's memory. Each fixed text field
 * must contain its own terminator; a later field cannot supply one for it. */
static int RecordRejectsUnterminatedFields(void)
{
    UmiSdkRuntimeConsumerEvidence value;
    umi_sdk_runtime_consumer_evidence_init(&value, "bounded-record");
    memset(value.id, 'x', sizeof(value.id));
    if (umi_sdk_runtime_consumer_evidence_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_sdk_runtime_consumer_evidence_init(&value, "bounded-record");
    memset(value.path, 'x', sizeof(value.path));
    if (umi_sdk_runtime_consumer_evidence_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_sdk_runtime_consumer_evidence_init(&value, "bounded-record");
    memset(value.detail, 'x', sizeof(value.detail));
    if (umi_sdk_runtime_consumer_evidence_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    return 0;
}
#define RECORD_TYPE UmiSdkRuntimeConsumerEvidence
#define RECORD_INIT umi_sdk_runtime_consumer_evidence_init
#define RECORD_VALIDATE umi_sdk_runtime_consumer_evidence_validate
#define RECORD_INIT_CHECKED umi_sdk_runtime_consumer_evidence_init_checked
#include "../record_integrity/record_construction_cases.h"

/* Each mutation refusal must preserve the complete accepted value. These
 * checks remain active in optimized builds where assert may be disabled. */
static int RecordMutationCases(void)
{
    UmiSdkRuntimeConsumerEvidence value;
    umi_sdk_runtime_consumer_evidence_init(&value, "mutation-record");
    unsigned char before[sizeof(value)];
    umi_sdk_runtime_consumer_evidence_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.path) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_sdk_runtime_consumer_evidence_set_path(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_sdk_runtime_consumer_evidence_set_path(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_sdk_runtime_consumer_evidence_set_path(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_sdk_runtime_consumer_evidence_set_path(&value, value.path + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.path, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.path) - 1U] = '\0';
        if (umi_sdk_runtime_consumer_evidence_set_path(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.path, oversized) != 0) return 1;
    }
    umi_sdk_runtime_consumer_evidence_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_consumer_evidence_set_path(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_sdk_runtime_consumer_evidence_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_sdk_runtime_consumer_evidence_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_sdk_runtime_consumer_evidence_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_sdk_runtime_consumer_evidence_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_sdk_runtime_consumer_evidence_set_detail(&value, value.detail + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.detail) - 1U] = '\0';
        if (umi_sdk_runtime_consumer_evidence_set_detail(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, oversized) != 0) return 1;
    }
    umi_sdk_runtime_consumer_evidence_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_consumer_evidence_set_detail(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_sdk_runtime_consumer_evidence_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_consumer_evidence_set_target_count(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_sdk_runtime_consumer_evidence_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_consumer_evidence_set_runtime_file_count(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_sdk_runtime_consumer_evidence_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_consumer_evidence_set_state(&value, (UmiSdkRuntimeState)0) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    UmiSdkRuntimeConsumerEvidence other;
    umi_sdk_runtime_consumer_evidence_init(&value, "identity-record");
    umi_sdk_runtime_consumer_evidence_init(&other, "identity-record");
    if (!umi_sdk_runtime_consumer_evidence_same_identity(&value, &other)) return 1;
    memcpy(before, &value, sizeof(value));
    memset(other.id, 'x', sizeof(other.id));
    if (umi_sdk_runtime_consumer_evidence_same_identity(&value, &other) || umi_sdk_runtime_consumer_evidence_same_identity(&other, &value)) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

/* Use nonzero domain fields to expose a codec that accidentally drops
 * values. The existing initializer supplies required compatibility metadata. */
static UmiSdkRuntimeConsumerEvidence ArchiveSample(void)
{
    UmiSdkRuntimeConsumerEvidence value;
    umi_sdk_runtime_consumer_evidence_init(&value, "archive-record");
    value.path[0] = 'a';
    value.detail[0] = 'a';
    value.target_count = (uint64_t)6U;
    value.runtime_file_count = (uint64_t)7U;
    value.revision = (uint64_t)8U;
    value.state = UMI_SDK_RUNTIME_STATE_READY;
    value.enabled = true;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiSdkRuntimeConsumerEvidence *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
    {
        size_t used = strlen(value->detail) + 1U;
        memset(value->detail + used, 0xa5, sizeof(value->detail) - used);
    }
}
#define ARCHIVE_TYPE UmiSdkRuntimeConsumerEvidence
#define ARCHIVE_ENCODE umi_sdk_runtime_consumer_evidence_archive_encode
#define ARCHIVE_DECODE umi_sdk_runtime_consumer_evidence_archive_decode
#define ARCHIVE_EQUAL RecordDefaultsEqual
#include "../value_archive/record_cases.h"

int main(void)
{
    if (ArchiveRecordCases() != 0) return 1;
    if (RecordMutationCases() != 0) return 1;
    if (RecordConstructionCases() != 0) return 1;
    UmiSdkRuntimeConsumerEvidence value,same;
    umi_sdk_runtime_consumer_evidence_init(&value,"sdk-runtime.consumer_evidence");
    assert(umi_sdk_runtime_consumer_evidence_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_target_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_runtime_file_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_evidence_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_consumer_evidence_init(&same,"sdk-runtime.consumer_evidence");
    assert(umi_sdk_runtime_consumer_evidence_same_identity(&value,&same));
    return 0;
    }
