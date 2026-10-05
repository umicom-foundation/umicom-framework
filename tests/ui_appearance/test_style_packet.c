/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_style_packet.c
 *
 * PURPOSE:
 *   Verify bundle resolved theme, typography, density, scale and motion identities for one renderer update.
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
#include "umicom/ui/appearance/style_packet.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/style_packet.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceStylePacketTransferEqual(const UmiAppearanceStylePacket *a, const UmiAppearanceStylePacket *b)
{
    return strcmp(a->packet_id, b->packet_id) == 0 &&
        strcmp(a->theme_pack_id, b->theme_pack_id) == 0 &&
        strcmp(a->typography_policy_id, b->typography_policy_id) == 0 &&
        a->density == b->density &&
        a->scale == b->scale &&
        a->reduced_motion == b->reduced_motion &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceStylePacketTransferTails(UmiAppearanceStylePacket *value)
{
    (void)value;
    {
        size_t used = strlen(value->packet_id) + 1U;
        memset(value->packet_id + used, 0xa5, sizeof(value->packet_id) - used);
    }
    {
        size_t used = strlen(value->theme_pack_id) + 1U;
        memset(value->theme_pack_id + used, 0xa5, sizeof(value->theme_pack_id) - used);
    }
    {
        size_t used = strlen(value->typography_policy_id) + 1U;
        memset(value->typography_policy_id + used, 0xa5, sizeof(value->typography_policy_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceStylePacketTransferMalformed(const UmiAppearanceStylePacket *sample)
{
    (void)sample;
    {
        UmiAppearanceStylePacket invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.packet_id, 'x', sizeof(invalid.packet_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_style_packet_is_valid(&invalid)) ||
            umi_appearance_style_packet_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated packet_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceStylePacket invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.theme_pack_id, 'x', sizeof(invalid.theme_pack_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_style_packet_is_valid(&invalid)) ||
            umi_appearance_style_packet_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated theme_pack_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceStylePacket invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.typography_policy_id, 'x', sizeof(invalid.typography_policy_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_style_packet_is_valid(&invalid)) ||
            umi_appearance_style_packet_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated typography_policy_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceStylePacketTransferCases, UmiAppearanceStylePacket,
    umi_appearance_style_packet_archive_encode, umi_appearance_style_packet_archive_decode,
    UmiAppearanceStylePacketTransferEqual, UmiAppearanceStylePacketTransferTails, UmiAppearanceStylePacketTransferMalformed)

int main(void) {
    UmiAppearanceStylePacket item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_style_packet_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_style_packet_is_valid(&item)) return 2;
    if (UmiAppearanceStylePacketTransferCases(&item) != 0) return 1;

    return 0;
}
