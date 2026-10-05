/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_factory_record.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/factory_record.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/factory_record.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelFactoryRecordTransferEqual(const UmiPanelFactoryRecord *a, const UmiPanelFactoryRecord *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->factory_id, b->factory_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->provider_id, b->provider_id) == 0 &&
        strcmp(a->component_id, b->component_id) == 0 &&
        a->enabled == b->enabled &&
        a->priority == b->priority &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelFactoryRecordTransferTails(UmiPanelFactoryRecord *value)
{
    (void)value;
    {
        size_t used = strlen(value->factory_id) + 1U;
        memset(value->factory_id + used, 0xa5, sizeof(value->factory_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->provider_id) + 1U;
        memset(value->provider_id + used, 0xa5, sizeof(value->provider_id) - used);
    }
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelFactoryRecordTransferMalformed(const UmiPanelFactoryRecord *sample)
{
    (void)sample;
    {
        UmiPanelFactoryRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.factory_id, 'x', sizeof(invalid.factory_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_factory_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_factory_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated factory_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelFactoryRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_factory_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_factory_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelFactoryRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider_id, 'x', sizeof(invalid.provider_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_factory_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_factory_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelFactoryRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_factory_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_factory_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelFactoryRecordTransferCases, UmiPanelFactoryRecord,
    umi_panel_factory_record_archive_encode, umi_panel_factory_record_archive_decode,
    UmiPanelFactoryRecordTransferEqual, UmiPanelFactoryRecordTransferTails, UmiPanelFactoryRecordTransferMalformed)

int main(void)
{
    UmiPanelFactoryRecord value;
    umi_panel_factory_record_init(&value);
    value.factory_id[0] = 's';
    value.panel_id[0] = 's';
    value.provider_id[0] = 's';
    value.component_id[0] = 's';
    value.enabled = true;
    value.priority = (uint32_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_panel_factory_record_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelFactoryRecordTransferCases(&value) != 0) return 1;

    return 0;
}
