/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_channel.c
 *
 * PURPOSE:
 *   Focused regression coverage for release channel descriptors and stability ordering.
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
#include "umicom/distribution/runtime/channel.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/channel.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrChannelTransferEqual(const UmiDrChannel *a, const UmiDrChannel *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->kind == b->kind &&
        a->stability_rank == b->stability_rank &&
        a->signed_only == b->signed_only &&
        a->automatic_updates == b->automatic_updates;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrChannelTransferTails(UmiDrChannel *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrChannelTransferMalformed(const UmiDrChannel *sample)
{
    (void)sample;
    {
        UmiDrChannel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_channel_valid(&invalid)) ||
            umi_dr_channel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrChannelTransferCases, UmiDrChannel,
    umi_dr_channel_archive_encode, umi_dr_channel_archive_decode,
    UmiDrChannelTransferEqual, UmiDrChannelTransferTails, UmiDrChannelTransferMalformed)

int main(void) {
    UmiDrChannel value; umi_dr_channel_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"stable")==UMI_STATUS_OK); CHECK(umi_dr_channel_valid(&value));
    if (UmiDrChannelTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_channel_fingerprint(&value) != 0U);
    return 0;
}
