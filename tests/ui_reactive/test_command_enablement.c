/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_command_enablement.c
 *
 * PURPOSE:
 *   Exercise the command enablement reactive UI contract.
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
#include "umicom/ui/reactive/command_enablement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/command_enablement.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveCommandEnablementTransferEqual(const UmiUiReactiveCommandEnablement *a, const UmiUiReactiveCommandEnablement *b)
{
    return strcmp(a->command_id, b->command_id) == 0 &&
        a->enabled == b->enabled &&
        a->evaluation_revision == b->evaluation_revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveCommandEnablementTransferTails(UmiUiReactiveCommandEnablement *value)
{
    (void)value;
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveCommandEnablementTransferMalformed(const UmiUiReactiveCommandEnablement *sample)
{
    (void)sample;
    {
        UmiUiReactiveCommandEnablement invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.command_id, 'x', sizeof(invalid.command_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_command_enablement_valid(&invalid)) ||
            umi_ui_reactive_command_enablement_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated command_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveCommandEnablementTransferCases, UmiUiReactiveCommandEnablement,
    umi_ui_reactive_command_enablement_archive_encode, umi_ui_reactive_command_enablement_archive_decode,
    UmiUiReactiveCommandEnablementTransferEqual, UmiUiReactiveCommandEnablementTransferTails, UmiUiReactiveCommandEnablementTransferMalformed)

int main(void) { UmiUiReactiveCommandEnablement item; umi_ui_reactive_command_enablement_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveCommandEnablement populated = item;
    (void)snprintf(populated.command_id, sizeof(populated.command_id), "field-0");
    populated.enabled = true;
    populated.evaluation_revision = (uint64_t)4;
    if (UmiUiReactiveCommandEnablementTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_command_enablement_valid(&item) ? 0 : 1; }
