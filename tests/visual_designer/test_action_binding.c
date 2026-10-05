/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_action_binding.c
 *
 * PURPOSE:
 *   Validate bind a designer action surface to a Framework command and target.
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
#include "umicom/designer/visual_designer/action_binding.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/action_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadActionBindingTransferEqual(const UmiRadActionBinding *a, const UmiRadActionBinding *b)
{
    return strcmp(a->action_id, b->action_id) == 0 &&
        strcmp(a->command_id, b->command_id) == 0 &&
        strcmp(a->target_id, b->target_id) == 0 &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadActionBindingTransferTails(UmiRadActionBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->action_id) + 1U;
        memset(value->action_id + used, 0xa5, sizeof(value->action_id) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->target_id) + 1U;
        memset(value->target_id + used, 0xa5, sizeof(value->target_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadActionBindingTransferMalformed(const UmiRadActionBinding *sample)
{
    (void)sample;
    {
        UmiRadActionBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.action_id, 'x', sizeof(invalid.action_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_action_binding_is_valid(&invalid)) ||
            umi_rad_action_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated action_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadActionBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.command_id, 'x', sizeof(invalid.command_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_action_binding_is_valid(&invalid)) ||
            umi_rad_action_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated command_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadActionBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_id, 'x', sizeof(invalid.target_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_action_binding_is_valid(&invalid)) ||
            umi_rad_action_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadActionBindingTransferCases, UmiRadActionBinding,
    umi_rad_action_binding_archive_encode, umi_rad_action_binding_archive_decode,
    UmiRadActionBindingTransferEqual, UmiRadActionBindingTransferTails, UmiRadActionBindingTransferMalformed)

int main(void){UmiRadActionBinding item;CHECK(umi_rad_action_binding_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_action_binding_is_valid(&item));
    if (UmiRadActionBindingTransferCases(&item) != 0) return 1;
return 0;}
