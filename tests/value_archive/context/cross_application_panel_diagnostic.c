/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_diagnostic.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/diagnostic.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/diagnostic.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelDiagnosticTransferEqual(const UmiPanelDiagnostic *a, const UmiPanelDiagnostic *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->diagnostic_id, b->diagnostic_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->message, b->message) == 0 &&
        a->severity == b->severity &&
        a->status == b->status &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelDiagnosticTransferTails(UmiPanelDiagnostic *value)
{
    (void)value;
    {
        size_t used = strlen(value->diagnostic_id) + 1U;
        memset(value->diagnostic_id + used, 0xa5, sizeof(value->diagnostic_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->instance_id) + 1U;
        memset(value->instance_id + used, 0xa5, sizeof(value->instance_id) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelDiagnosticTransferMalformed(const UmiPanelDiagnostic *sample)
{
    (void)sample;
    {
        UmiPanelDiagnostic invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.diagnostic_id, 'x', sizeof(invalid.diagnostic_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_diagnostic_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_diagnostic_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated diagnostic_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelDiagnostic invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_diagnostic_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_diagnostic_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelDiagnostic invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_diagnostic_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_diagnostic_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelDiagnostic invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_diagnostic_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_diagnostic_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelDiagnosticTransferCases, UmiPanelDiagnostic,
    umi_panel_diagnostic_archive_encode, umi_panel_diagnostic_archive_decode,
    UmiPanelDiagnosticTransferEqual, UmiPanelDiagnosticTransferTails, UmiPanelDiagnosticTransferMalformed)

int main(void)
{
    UmiPanelDiagnostic value;
    umi_panel_diagnostic_init(&value);
    value.diagnostic_id[0] = 's';
    value.panel_id[0] = 's';
    value.instance_id[0] = 's';
    value.message[0] = 's';
    value.severity = (uint32_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_panel_diagnostic_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelDiagnosticTransferCases(&value) != 0) return 1;

    return 0;
}
