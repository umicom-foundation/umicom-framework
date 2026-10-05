/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_activation.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/activation.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/activation.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelActivationTransferEqual(const UmiPanelActivation *a, const UmiPanelActivation *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->activation_id, b->activation_id) == 0 &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        a->reason == b->reason &&
        strcmp(a->source_id, b->source_id) == 0 &&
        a->timestamp_ms == b->timestamp_ms &&
        a->successful == b->successful &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelActivationTransferTails(UmiPanelActivation *value)
{
    (void)value;
    {
        size_t used = strlen(value->activation_id) + 1U;
        memset(value->activation_id + used, 0xa5, sizeof(value->activation_id) - used);
    }
    {
        size_t used = strlen(value->instance_id) + 1U;
        memset(value->instance_id + used, 0xa5, sizeof(value->instance_id) - used);
    }
    {
        size_t used = strlen(value->source_id) + 1U;
        memset(value->source_id + used, 0xa5, sizeof(value->source_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelActivationTransferMalformed(const UmiPanelActivation *sample)
{
    (void)sample;
    {
        UmiPanelActivation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.activation_id, 'x', sizeof(invalid.activation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_activation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_activation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated activation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelActivation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_activation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_activation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelActivation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_activation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_activation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelActivationTransferCases, UmiPanelActivation,
    umi_panel_activation_archive_encode, umi_panel_activation_archive_decode,
    UmiPanelActivationTransferEqual, UmiPanelActivationTransferTails, UmiPanelActivationTransferMalformed)

int main(void)
{
    UmiPanelActivation value;
    umi_panel_activation_init(&value);
    value.activation_id[0] = 's';
    value.instance_id[0] = 's';
    value.reason = (uint32_t)17U;
    value.source_id[0] = 's';
    value.timestamp_ms = (uint64_t)17U;
    value.successful = true;
    value.revision = (uint64_t)17U;
    if (umi_panel_activation_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelActivationTransferCases(&value) != 0) return 1;

    return 0;
}
