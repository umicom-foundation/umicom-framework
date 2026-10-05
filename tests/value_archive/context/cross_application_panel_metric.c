/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_metric.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/metric.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/metric.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelMetricTransferEqual(const UmiPanelMetric *a, const UmiPanelMetric *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        a->open_count == b->open_count &&
        a->close_count == b->close_count &&
        a->activation_count == b->activation_count &&
        a->context_count == b->context_count &&
        a->failure_count == b->failure_count &&
        a->last_active_ms == b->last_active_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelMetricTransferTails(UmiPanelMetric *value)
{
    (void)value;
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelMetricTransferMalformed(const UmiPanelMetric *sample)
{
    (void)sample;
    {
        UmiPanelMetric invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_metric_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_metric_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelMetricTransferCases, UmiPanelMetric,
    umi_panel_metric_archive_encode, umi_panel_metric_archive_decode,
    UmiPanelMetricTransferEqual, UmiPanelMetricTransferTails, UmiPanelMetricTransferMalformed)

int main(void)
{
    UmiPanelMetric value;
    umi_panel_metric_init(&value);
    value.panel_id[0] = 's';
    value.open_count = (uint64_t)17U;
    value.close_count = (uint64_t)17U;
    value.activation_count = (uint64_t)17U;
    value.context_count = (uint64_t)17U;
    value.failure_count = (uint64_t)17U;
    value.last_active_ms = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_panel_metric_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelMetricTransferCases(&value) != 0) return 1;

    return 0;
}
