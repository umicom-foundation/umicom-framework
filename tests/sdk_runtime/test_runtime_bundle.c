/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_runtime_bundle.c
 *
 * PURPOSE:
 *   Verify the runtime bundle SDK/runtime contract.
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
#include "umicom/sdk_runtime/runtime_bundle.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Compare the documented default fields, not struct padding. */
static int RecordDefaultsEqual(const UmiSdkRuntimeRuntimeBundle *left, const UmiSdkRuntimeRuntimeBundle *right)
{
    return left->structure_size == right->structure_size &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->path, right->path, sizeof(left->path)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->file_count == right->file_count &&
        left->dependency_count == right->dependency_count &&
        left->revision == right->revision &&
        left->state == right->state &&
        left->enabled == right->enabled;
}
/* Public records may arrive from a caller's memory. Each fixed text field
 * must contain its own terminator; a later field cannot supply one for it. */
static int RecordRejectsUnterminatedFields(void)
{
    UmiSdkRuntimeRuntimeBundle value;
    umi_sdk_runtime_runtime_bundle_init(&value, "bounded-record");
    memset(value.id, 'x', sizeof(value.id));
    if (umi_sdk_runtime_runtime_bundle_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_sdk_runtime_runtime_bundle_init(&value, "bounded-record");
    memset(value.path, 'x', sizeof(value.path));
    if (umi_sdk_runtime_runtime_bundle_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_sdk_runtime_runtime_bundle_init(&value, "bounded-record");
    memset(value.detail, 'x', sizeof(value.detail));
    if (umi_sdk_runtime_runtime_bundle_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    return 0;
}
#define RECORD_TYPE UmiSdkRuntimeRuntimeBundle
#define RECORD_INIT umi_sdk_runtime_runtime_bundle_init
#define RECORD_VALIDATE umi_sdk_runtime_runtime_bundle_validate
#define RECORD_INIT_CHECKED umi_sdk_runtime_runtime_bundle_init_checked
#include "../record_integrity/record_construction_cases.h"

/* Each mutation refusal must preserve the complete accepted value. These
 * checks remain active in optimized builds where assert may be disabled. */
static int RecordMutationCases(void)
{
    UmiSdkRuntimeRuntimeBundle value;
    umi_sdk_runtime_runtime_bundle_init(&value, "mutation-record");
    unsigned char before[sizeof(value)];
    umi_sdk_runtime_runtime_bundle_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.path) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_sdk_runtime_runtime_bundle_set_path(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_sdk_runtime_runtime_bundle_set_path(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_sdk_runtime_runtime_bundle_set_path(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_sdk_runtime_runtime_bundle_set_path(&value, value.path + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.path, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.path) - 1U] = '\0';
        if (umi_sdk_runtime_runtime_bundle_set_path(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.path, oversized) != 0) return 1;
    }
    umi_sdk_runtime_runtime_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_runtime_bundle_set_path(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_sdk_runtime_runtime_bundle_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_sdk_runtime_runtime_bundle_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_sdk_runtime_runtime_bundle_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_sdk_runtime_runtime_bundle_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_sdk_runtime_runtime_bundle_set_detail(&value, value.detail + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.detail) - 1U] = '\0';
        if (umi_sdk_runtime_runtime_bundle_set_detail(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, oversized) != 0) return 1;
    }
    umi_sdk_runtime_runtime_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_runtime_bundle_set_detail(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_sdk_runtime_runtime_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_runtime_bundle_set_file_count(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_sdk_runtime_runtime_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_runtime_bundle_set_dependency_count(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_sdk_runtime_runtime_bundle_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_sdk_runtime_runtime_bundle_set_state(&value, (UmiSdkRuntimeState)0) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    UmiSdkRuntimeRuntimeBundle other;
    umi_sdk_runtime_runtime_bundle_init(&value, "identity-record");
    umi_sdk_runtime_runtime_bundle_init(&other, "identity-record");
    if (!umi_sdk_runtime_runtime_bundle_same_identity(&value, &other)) return 1;
    memcpy(before, &value, sizeof(value));
    memset(other.id, 'x', sizeof(other.id));
    if (umi_sdk_runtime_runtime_bundle_same_identity(&value, &other) || umi_sdk_runtime_runtime_bundle_same_identity(&other, &value)) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

/* Use nonzero domain fields to expose a codec that accidentally drops
 * values. The existing initializer supplies required compatibility metadata. */
static UmiSdkRuntimeRuntimeBundle ArchiveSample(void)
{
    UmiSdkRuntimeRuntimeBundle value;
    umi_sdk_runtime_runtime_bundle_init(&value, "archive-record");
    value.path[0] = 'a';
    value.detail[0] = 'a';
    value.file_count = (uint64_t)6U;
    value.dependency_count = (uint64_t)7U;
    value.revision = (uint64_t)8U;
    value.state = UMI_SDK_RUNTIME_STATE_READY;
    value.enabled = true;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiSdkRuntimeRuntimeBundle *value)
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
#define ARCHIVE_TYPE UmiSdkRuntimeRuntimeBundle
#define ARCHIVE_ENCODE umi_sdk_runtime_runtime_bundle_archive_encode
#define ARCHIVE_DECODE umi_sdk_runtime_runtime_bundle_archive_decode
#define ARCHIVE_EQUAL RecordDefaultsEqual
#include "../value_archive/record_cases.h"

int main(void)
{
    if (ArchiveRecordCases() != 0) return 1;
    if (RecordMutationCases() != 0) return 1;
    if (RecordConstructionCases() != 0) return 1;
    UmiSdkRuntimeRuntimeBundle value,same;
    umi_sdk_runtime_runtime_bundle_init(&value,"sdk-runtime.runtime_bundle");
    assert(umi_sdk_runtime_runtime_bundle_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_bundle_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_bundle_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_bundle_set_file_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_bundle_set_dependency_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_bundle_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_runtime_bundle_init(&same,"sdk-runtime.runtime_bundle");
    assert(umi_sdk_runtime_runtime_bundle_same_identity(&value,&same));
    return 0;
    }
