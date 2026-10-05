/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_capability.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/capability.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/capability.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelCapabilityTransferEqual(const UmiPanelCapability *a, const UmiPanelCapability *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->capability_id, b->capability_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->required_capability, b->required_capability) == 0 &&
        strcmp(a->optional_capability, b->optional_capability) == 0 &&
        a->available == b->available &&
        a->degraded == b->degraded &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelCapabilityTransferTails(UmiPanelCapability *value)
{
    (void)value;
    {
        size_t used = strlen(value->capability_id) + 1U;
        memset(value->capability_id + used, 0xa5, sizeof(value->capability_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->required_capability) + 1U;
        memset(value->required_capability + used, 0xa5, sizeof(value->required_capability) - used);
    }
    {
        size_t used = strlen(value->optional_capability) + 1U;
        memset(value->optional_capability + used, 0xa5, sizeof(value->optional_capability) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelCapabilityTransferMalformed(const UmiPanelCapability *sample)
{
    (void)sample;
    {
        UmiPanelCapability invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.capability_id, 'x', sizeof(invalid.capability_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_capability_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_capability_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated capability_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelCapability invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_capability_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_capability_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelCapability invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.required_capability, 'x', sizeof(invalid.required_capability));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_capability_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_capability_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated required_capability was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelCapability invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.optional_capability, 'x', sizeof(invalid.optional_capability));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_capability_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_capability_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated optional_capability was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelCapabilityTransferCases, UmiPanelCapability,
    umi_panel_capability_archive_encode, umi_panel_capability_archive_decode,
    UmiPanelCapabilityTransferEqual, UmiPanelCapabilityTransferTails, UmiPanelCapabilityTransferMalformed)

int main(void)
{
    UmiPanelCapability value;
    umi_panel_capability_init(&value);
    value.capability_id[0] = 's';
    value.panel_id[0] = 's';
    value.required_capability[0] = 's';
    value.optional_capability[0] = 's';
    value.available = true;
    value.degraded = true;
    value.revision = (uint64_t)17U;
    if (umi_panel_capability_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelCapabilityTransferCases(&value) != 0) return 1;

    return 0;
}
