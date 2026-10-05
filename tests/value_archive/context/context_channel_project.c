/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_project.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/project.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/project.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiProjectContextTransferEqual(const UmiProjectContext *a, const UmiProjectContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->project_id, b->project_id) == 0 &&
        strcmp(a->repository_id, b->repository_id) == 0 &&
        strcmp(a->root_path, b->root_path) == 0 &&
        strcmp(a->target_id, b->target_id) == 0 &&
        strcmp(a->configuration_id, b->configuration_id) == 0 &&
        strcmp(a->language_id, b->language_id) == 0 &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiProjectContextTransferTails(UmiProjectContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->project_id) + 1U;
        memset(value->project_id + used, 0xa5, sizeof(value->project_id) - used);
    }
    {
        size_t used = strlen(value->repository_id) + 1U;
        memset(value->repository_id + used, 0xa5, sizeof(value->repository_id) - used);
    }
    {
        size_t used = strlen(value->root_path) + 1U;
        memset(value->root_path + used, 0xa5, sizeof(value->root_path) - used);
    }
    {
        size_t used = strlen(value->target_id) + 1U;
        memset(value->target_id + used, 0xa5, sizeof(value->target_id) - used);
    }
    {
        size_t used = strlen(value->configuration_id) + 1U;
        memset(value->configuration_id + used, 0xa5, sizeof(value->configuration_id) - used);
    }
    {
        size_t used = strlen(value->language_id) + 1U;
        memset(value->language_id + used, 0xa5, sizeof(value->language_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiProjectContextTransferMalformed(const UmiProjectContext *sample)
{
    (void)sample;
    {
        UmiProjectContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.project_id, 'x', sizeof(invalid.project_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated project_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.repository_id, 'x', sizeof(invalid.repository_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated repository_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.root_path, 'x', sizeof(invalid.root_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated root_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_id, 'x', sizeof(invalid.target_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.configuration_id, 'x', sizeof(invalid.configuration_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated configuration_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.language_id, 'x', sizeof(invalid.language_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated language_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiProjectContextTransferCases, UmiProjectContext,
    umi_project_context_archive_encode, umi_project_context_archive_decode,
    UmiProjectContextTransferEqual, UmiProjectContextTransferTails, UmiProjectContextTransferMalformed)

int main(void)
{
    UmiProjectContext value;
    umi_project_context_init(&value);
    value.project_id[0] = 's';
    value.repository_id[0] = 's';
    value.root_path[0] = 's';
    value.target_id[0] = 's';
    value.configuration_id[0] = 's';
    value.language_id[0] = 's';
    value.revision = (uint64_t)17U;
    if (umi_project_context_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiProjectContextTransferCases(&value) != 0) return 1;

    return 0;
}
