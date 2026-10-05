/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_host_slot.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/host_slot.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/host_slot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelHostSlotTransferEqual(const UmiPanelHostSlot *a, const UmiPanelHostSlot *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->slot_id, b->slot_id) == 0 &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->container_id, b->container_id) == 0 &&
        a->placement == b->placement &&
        a->order == b->order &&
        a->visible == b->visible &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelHostSlotTransferTails(UmiPanelHostSlot *value)
{
    (void)value;
    {
        size_t used = strlen(value->slot_id) + 1U;
        memset(value->slot_id + used, 0xa5, sizeof(value->slot_id) - used);
    }
    {
        size_t used = strlen(value->instance_id) + 1U;
        memset(value->instance_id + used, 0xa5, sizeof(value->instance_id) - used);
    }
    {
        size_t used = strlen(value->container_id) + 1U;
        memset(value->container_id + used, 0xa5, sizeof(value->container_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelHostSlotTransferMalformed(const UmiPanelHostSlot *sample)
{
    (void)sample;
    {
        UmiPanelHostSlot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.slot_id, 'x', sizeof(invalid.slot_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_host_slot_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_host_slot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated slot_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelHostSlot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_host_slot_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_host_slot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelHostSlot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.container_id, 'x', sizeof(invalid.container_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_host_slot_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_host_slot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated container_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelHostSlotTransferCases, UmiPanelHostSlot,
    umi_panel_host_slot_archive_encode, umi_panel_host_slot_archive_decode,
    UmiPanelHostSlotTransferEqual, UmiPanelHostSlotTransferTails, UmiPanelHostSlotTransferMalformed)

int main(void)
{
    UmiPanelHostSlot value;
    umi_panel_host_slot_init(&value);
    value.slot_id[0] = 's';
    value.instance_id[0] = 's';
    value.container_id[0] = 's';
    value.order = (uint32_t)17U;
    value.visible = true;
    value.revision = (uint64_t)17U;
    if (umi_panel_host_slot_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelHostSlotTransferCases(&value) != 0) return 1;

    return 0;
}
