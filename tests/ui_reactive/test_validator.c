/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validator.c
 *
 * PURPOSE:
 *   Exercise the validator reactive UI contract.
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
#include "umicom/ui/reactive/validator.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/validator.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveValidatorTransferEqual(const UmiUiReactiveValidator *a, const UmiUiReactiveValidator *b)
{
    return strcmp(a->validator_id, b->validator_id) == 0 &&
        a->value_kind == b->value_kind &&
        a->severity == b->severity &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveValidatorTransferTails(UmiUiReactiveValidator *value)
{
    (void)value;
    {
        size_t used = strlen(value->validator_id) + 1U;
        memset(value->validator_id + used, 0xa5, sizeof(value->validator_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveValidatorTransferMalformed(const UmiUiReactiveValidator *sample)
{
    (void)sample;
    {
        UmiUiReactiveValidator invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.validator_id, 'x', sizeof(invalid.validator_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_validator_valid(&invalid)) ||
            umi_ui_reactive_validator_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated validator_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveValidatorTransferCases, UmiUiReactiveValidator,
    umi_ui_reactive_validator_archive_encode, umi_ui_reactive_validator_archive_decode,
    UmiUiReactiveValidatorTransferEqual, UmiUiReactiveValidatorTransferTails, UmiUiReactiveValidatorTransferMalformed)

int main(void) { UmiUiReactiveValidator item; umi_ui_reactive_validator_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveValidator populated = item;
    (void)snprintf(populated.validator_id, sizeof(populated.validator_id), "field-0");
    populated.enabled = true;
    if (UmiUiReactiveValidatorTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_validator_valid(&item) ? 0 : 1; }
