/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_server_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data server profile enterprise data capability.
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
#include "umicom/data/enterprise/data_server_profile.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/data_server_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataServerProfileTransferEqual(const UmiDataServerProfile *a, const UmiDataServerProfile *b)
{
    return strcmp(a->profile_id, b->profile_id) == 0 &&
        a->minimum_pool_size == b->minimum_pool_size &&
        a->maximum_pool_size == b->maximum_pool_size &&
        a->query_row_limit == b->query_row_limit &&
        a->default_consistency == b->default_consistency &&
        a->migrations_enabled == b->migrations_enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataServerProfileTransferTails(UmiDataServerProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->profile_id) + 1U;
        memset(value->profile_id + used, 0xa5, sizeof(value->profile_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataServerProfileTransferMalformed(const UmiDataServerProfile *sample)
{
    (void)sample;
    {
        UmiDataServerProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.profile_id, 'x', sizeof(invalid.profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_server_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_server_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated profile_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataServerProfileTransferCases, UmiDataServerProfile,
    umi_data_data_server_profile_archive_encode, umi_data_data_server_profile_archive_decode,
    UmiDataServerProfileTransferEqual, UmiDataServerProfileTransferTails, UmiDataServerProfileTransferMalformed)

int main(void) {
    UmiDataServerProfile item;
    CHECK(umi_data_data_server_profile_init(&item,"production",2U,16U,10000U) == UMI_STATUS_OK);
    if (UmiDataServerProfileTransferCases(&item) != 0) return 1;

    CHECK(item.migrations_enabled);
    return 0;
}
