/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_identifier.c
 *
 * PURPOSE:
 *   Implement the test identifier behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | identifier | Sammy Hegab | Umicom Foundation | MIT */
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "umicom/application/production/identifier.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/application/production/identifier.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiApplicationProductionIdentifierTransferEqual(const UmiApplicationProductionIdentifier *a, const UmiApplicationProductionIdentifier *b)
{
    return strcmp(a->value, b->value) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiApplicationProductionIdentifierTransferTails(UmiApplicationProductionIdentifier *value)
{
    (void)value;
    {
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiApplicationProductionIdentifierTransferMalformed(const UmiApplicationProductionIdentifier *sample)
{
    (void)sample;
    {
        UmiApplicationProductionIdentifier invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value, 'x', sizeof(invalid.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_application_production_identifier_valid(&invalid)) ||
            umi_application_production_identifier_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiApplicationProductionIdentifierTransferCases, UmiApplicationProductionIdentifier,
    umi_application_production_identifier_archive_encode, umi_application_production_identifier_archive_decode,
    UmiApplicationProductionIdentifierTransferEqual, UmiApplicationProductionIdentifierTransferTails, UmiApplicationProductionIdentifierTransferMalformed)

int main(void) {
    UmiApplicationProductionIdentifier a = {{0}}, b = {{0}};
    assert(umi_application_production_identifier_set(&a, "org.umicom.studio") == UMI_STATUS_OK);
    assert(umi_application_production_identifier_set(&b, "org.umicom.studio") == UMI_STATUS_OK);
    assert(umi_application_production_identifier_valid(&a));
    if (UmiApplicationProductionIdentifierTransferCases(&a) != 0) return 1;

    assert(umi_application_production_identifier_equal(&a, &b));
    return 0;
}

