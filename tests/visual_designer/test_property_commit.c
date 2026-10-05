/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_property_commit.c
 *
 * PURPOSE:
 *   Validate record before/after property values for review, undo and audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/designer/visual_designer/property_commit.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/property_commit.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadPropertyCommitTransferEqual(const UmiRadPropertyCommit *a, const UmiRadPropertyCommit *b)
{
    return strcmp(a->component_id, b->component_id) == 0 &&
        strcmp(a->property_id, b->property_id) == 0 &&
        strcmp(a->before_value, b->before_value) == 0 &&
        strcmp(a->after_value, b->after_value) == 0 &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadPropertyCommitTransferTails(UmiRadPropertyCommit *value)
{
    (void)value;
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
    {
        size_t used = strlen(value->property_id) + 1U;
        memset(value->property_id + used, 0xa5, sizeof(value->property_id) - used);
    }
    {
        size_t used = strlen(value->before_value) + 1U;
        memset(value->before_value + used, 0xa5, sizeof(value->before_value) - used);
    }
    {
        size_t used = strlen(value->after_value) + 1U;
        memset(value->after_value + used, 0xa5, sizeof(value->after_value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadPropertyCommitTransferMalformed(const UmiRadPropertyCommit *sample)
{
    (void)sample;
    {
        UmiRadPropertyCommit invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_commit_is_valid(&invalid)) ||
            umi_rad_property_commit_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadPropertyCommit invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.property_id, 'x', sizeof(invalid.property_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_commit_is_valid(&invalid)) ||
            umi_rad_property_commit_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated property_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadPropertyCommit invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.before_value, 'x', sizeof(invalid.before_value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_commit_is_valid(&invalid)) ||
            umi_rad_property_commit_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated before_value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadPropertyCommit invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.after_value, 'x', sizeof(invalid.after_value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_commit_is_valid(&invalid)) ||
            umi_rad_property_commit_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated after_value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadPropertyCommitTransferCases, UmiRadPropertyCommit,
    umi_rad_property_commit_archive_encode, umi_rad_property_commit_archive_decode,
    UmiRadPropertyCommitTransferEqual, UmiRadPropertyCommitTransferTails, UmiRadPropertyCommitTransferMalformed)

int main(void){UmiRadPropertyCommit item;CHECK(umi_rad_property_commit_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_property_commit_is_valid(&item));
    if (UmiRadPropertyCommitTransferCases(&item) != 0) return 1;
return 0;}
