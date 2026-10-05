/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_configuration_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for system, user and portable configuration-root policy.
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
#include "umicom/distribution/runtime/configuration_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/configuration_layout.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrConfigurationLayoutTransferEqual(const UmiDrConfigurationLayout *a, const UmiDrConfigurationLayout *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->system_root, b->system_root) == 0 &&
        strcmp(a->user_root, b->user_root) == 0 &&
        a->portable == b->portable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrConfigurationLayoutTransferTails(UmiDrConfigurationLayout *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->system_root) + 1U;
        memset(value->system_root + used, 0xa5, sizeof(value->system_root) - used);
    }
    {
        size_t used = strlen(value->user_root) + 1U;
        memset(value->user_root + used, 0xa5, sizeof(value->user_root) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrConfigurationLayoutTransferMalformed(const UmiDrConfigurationLayout *sample)
{
    (void)sample;
    {
        UmiDrConfigurationLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_configuration_layout_valid(&invalid)) ||
            umi_dr_configuration_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrConfigurationLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.system_root, 'x', sizeof(invalid.system_root));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_configuration_layout_valid(&invalid)) ||
            umi_dr_configuration_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated system_root was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrConfigurationLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.user_root, 'x', sizeof(invalid.user_root));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_configuration_layout_valid(&invalid)) ||
            umi_dr_configuration_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated user_root was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrConfigurationLayoutTransferCases, UmiDrConfigurationLayout,
    umi_dr_configuration_layout_archive_encode, umi_dr_configuration_layout_archive_decode,
    UmiDrConfigurationLayoutTransferEqual, UmiDrConfigurationLayoutTransferTails, UmiDrConfigurationLayoutTransferMalformed)

int main(void) {
    UmiDrConfigurationLayout value; umi_dr_configuration_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"config")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.user_root,sizeof(value.user_root),"config")==UMI_STATUS_OK); value.portable=true; CHECK(umi_dr_configuration_layout_valid(&value));
    if (UmiDrConfigurationLayoutTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_configuration_layout_fingerprint(&value) != 0U);
    return 0;
}
