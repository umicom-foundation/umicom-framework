/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_workspace_folder.c
 * PURPOSE: Focused regression test for workspace folder.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/workspace_folder.h"
#include <string.h>


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Compare the documented default fields, not struct padding. */
static int RecordDefaultsEqual(const UmiLanguageIntelligenceWorkspaceFolder *left, const UmiLanguageIntelligenceWorkspaceFolder *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->subject_id, right->subject_id, sizeof(left->subject_id)) == 0 &&
        memcmp(left->detail, right->detail, sizeof(left->detail)) == 0 &&
        left->range.start.line == right->range.start.line &&
        left->range.start.character == right->range.start.character &&
        left->range.end.line == right->range.end.line &&
        left->range.end.character == right->range.end.character &&
        left->revision == right->revision &&
        left->priority == right->priority &&
        left->flags == right->flags &&
        left->enabled == right->enabled;
}
/* Public records may arrive from a caller's memory. Each fixed text field
 * must contain its own terminator; a later field cannot supply one for it. */
static int RecordRejectsUnterminatedFields(void)
{
    UmiLanguageIntelligenceWorkspaceFolder value;
    umi_language_intelligence_workspace_folder_init(&value, "bounded-record");
    memset(value.id, 'x', sizeof(value.id));
    if (umi_language_intelligence_workspace_folder_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_language_intelligence_workspace_folder_init(&value, "bounded-record");
    memset(value.subject_id, 'x', sizeof(value.subject_id));
    if (umi_language_intelligence_workspace_folder_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    umi_language_intelligence_workspace_folder_init(&value, "bounded-record");
    memset(value.detail, 'x', sizeof(value.detail));
    if (umi_language_intelligence_workspace_folder_validate(&value) != UMI_STATUS_INVALID_ARGUMENT) return 1;
    return 0;
}
#define RECORD_TYPE UmiLanguageIntelligenceWorkspaceFolder
#define RECORD_INIT umi_language_intelligence_workspace_folder_init
#define RECORD_VALIDATE umi_language_intelligence_workspace_folder_validate
#define RECORD_INIT_CHECKED umi_language_intelligence_workspace_folder_init_checked
#include "../record_integrity/record_construction_cases.h"

/* Each mutation refusal must preserve the complete accepted value. These
 * checks remain active in optimized builds where assert may be disabled. */
static int RecordMutationCases(void)
{
    UmiLanguageIntelligenceWorkspaceFolder value;
    umi_language_intelligence_workspace_folder_init(&value, "mutation-record");
    unsigned char before[sizeof(value)];
    umi_language_intelligence_workspace_folder_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.subject_id) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_language_intelligence_workspace_folder_set_subject(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_language_intelligence_workspace_folder_set_subject(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_language_intelligence_workspace_folder_set_subject(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_language_intelligence_workspace_folder_set_subject(&value, value.subject_id + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.subject_id, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.subject_id) - 1U] = '\0';
        if (umi_language_intelligence_workspace_folder_set_subject(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.subject_id, oversized) != 0) return 1;
    }
    umi_language_intelligence_workspace_folder_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_language_intelligence_workspace_folder_set_subject(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    umi_language_intelligence_workspace_folder_init(&value, "mutation-record");
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_language_intelligence_workspace_folder_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_language_intelligence_workspace_folder_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_language_intelligence_workspace_folder_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        const uint64_t revision = value.revision;
        if (umi_language_intelligence_workspace_folder_set_detail(&value, value.detail + 1U) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != revision + 1U) return 1;
        oversized[sizeof(value.detail) - 1U] = '\0';
        if (umi_language_intelligence_workspace_folder_set_detail(&value, oversized) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, oversized) != 0) return 1;
    }
    umi_language_intelligence_workspace_folder_init(&value, "exhausted-record");
    value.revision = UINT64_MAX;
    memcpy(before, &value, sizeof(value));
    if (umi_language_intelligence_workspace_folder_set_detail(&value, "retained") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    UmiLanguageIntelligenceWorkspaceFolder other;
    umi_language_intelligence_workspace_folder_init(&value, "identity-record");
    umi_language_intelligence_workspace_folder_init(&other, "identity-record");
    if (!umi_language_intelligence_workspace_folder_same_identity(&value, &other)) return 1;
    memcpy(before, &value, sizeof(value));
    memset(other.id, 'x', sizeof(other.id));
    if (umi_language_intelligence_workspace_folder_same_identity(&value, &other) || umi_language_intelligence_workspace_folder_same_identity(&other, &value)) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

/* Use nonzero domain fields to expose a codec that accidentally drops
 * values. The existing initializer supplies required compatibility metadata. */
static UmiLanguageIntelligenceWorkspaceFolder ArchiveSample(void)
{
    UmiLanguageIntelligenceWorkspaceFolder value;
    umi_language_intelligence_workspace_folder_init(&value, "archive-record");
    value.subject_id[0] = 'a';
    value.detail[0] = 'a';
    value.range = (UmiLanguageIntelligenceRange){{2U, 3U}, {4U, 5U}};
    value.revision = (uint64_t)8U;
    value.priority = (uint32_t)9U;
    value.flags = (uint32_t)10U;
    value.enabled = (int)11U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiLanguageIntelligenceWorkspaceFolder *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->subject_id) + 1U;
        memset(value->subject_id + used, 0xa5, sizeof(value->subject_id) - used);
    }
    {
        size_t used = strlen(value->detail) + 1U;
        memset(value->detail + used, 0xa5, sizeof(value->detail) - used);
    }
}
#define ARCHIVE_TYPE UmiLanguageIntelligenceWorkspaceFolder
#define ARCHIVE_ENCODE umi_language_intelligence_workspace_folder_archive_encode
#define ARCHIVE_DECODE umi_language_intelligence_workspace_folder_archive_decode
#define ARCHIVE_EQUAL RecordDefaultsEqual
#include "../value_archive/record_cases.h"

int main(void)
{
    if (ArchiveRecordCases() != 0) return 1;
    if (RecordMutationCases() != 0) return 1;
    if (RecordConstructionCases() != 0) return 1;
    UmiLanguageIntelligenceWorkspaceFolder value;
    UmiLanguageIntelligenceWorkspaceFolder other;
    umi_language_intelligence_workspace_folder_init(&value, "workspace_folder.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_workspace_folder_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_workspace_folder_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_workspace_folder_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_workspace_folder_init(&other, "workspace_folder.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_workspace_folder_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
