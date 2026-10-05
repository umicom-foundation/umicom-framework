/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_group_member.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/group_member.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/group_member.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextGroupMemberTransferEqual(const UmiContextGroupMember *a, const UmiContextGroupMember *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->member_id, b->member_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->panel_instance_id, b->panel_instance_id) == 0 &&
        a->role == b->role &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextGroupMemberTransferTails(UmiContextGroupMember *value)
{
    (void)value;
    {
        size_t used = strlen(value->member_id) + 1U;
        memset(value->member_id + used, 0xa5, sizeof(value->member_id) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->panel_instance_id) + 1U;
        memset(value->panel_instance_id + used, 0xa5, sizeof(value->panel_instance_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextGroupMemberTransferMalformed(const UmiContextGroupMember *sample)
{
    (void)sample;
    {
        UmiContextGroupMember invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.member_id, 'x', sizeof(invalid.member_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_group_member_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_group_member_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated member_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextGroupMember invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_group_member_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_group_member_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextGroupMember invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_group_member_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_group_member_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextGroupMember invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_instance_id, 'x', sizeof(invalid.panel_instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_group_member_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_group_member_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_instance_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextGroupMemberTransferCases, UmiContextGroupMember,
    umi_context_group_member_archive_encode, umi_context_group_member_archive_decode,
    UmiContextGroupMemberTransferEqual, UmiContextGroupMemberTransferTails, UmiContextGroupMemberTransferMalformed)

int main(void)
{
    UmiContextGroupMember value;
    umi_context_group_member_init(&value);
    value.member_id[0] = 's';
    value.group_id[0] = 's';
    value.application_id[0] = 's';
    value.panel_instance_id[0] = 's';
    value.enabled = true;
    value.revision = (uint64_t)17U;
    if (umi_context_group_member_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextGroupMemberTransferCases(&value) != 0) return 1;

    return 0;
}
