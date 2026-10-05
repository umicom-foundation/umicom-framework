/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_delivery_target.c
 *
 * PURPOSE:
 *   Verify the context delivery target contract, mutation, validation and stable hashing.
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
#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_link/delivery_target.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/delivery_target.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkDeliveryTargetTransferEqual(const UmiWorkbenchContextLinkDeliveryTarget *a, const UmiWorkbenchContextLinkDeliveryTarget *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->target_id, b->target_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        a->context_kind == b->context_kind &&
        a->colour == b->colour &&
        a->mode == b->mode &&
        a->state == b->state &&
        a->origin == b->origin &&
        a->priority == b->priority &&
        a->flags == b->flags &&
        a->sequence == b->sequence &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchContextLinkDeliveryTargetTransferTails(UmiWorkbenchContextLinkDeliveryTarget *value)
{
    (void)value;
    {
        size_t used = strlen(value->target_id) + 1U;
        memset(value->target_id + used, 0xa5, sizeof(value->target_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkDeliveryTargetTransferMalformed(const UmiWorkbenchContextLinkDeliveryTarget *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkDeliveryTarget invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_id, 'x', sizeof(invalid.target_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_delivery_target_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_delivery_target_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkDeliveryTarget invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_delivery_target_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_delivery_target_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkDeliveryTarget invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_delivery_target_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_delivery_target_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkDeliveryTargetTransferCases, UmiWorkbenchContextLinkDeliveryTarget,
    umi_workbench_context_link_delivery_target_archive_encode, umi_workbench_context_link_delivery_target_archive_decode,
    UmiWorkbenchContextLinkDeliveryTargetTransferEqual, UmiWorkbenchContextLinkDeliveryTargetTransferTails, UmiWorkbenchContextLinkDeliveryTargetTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkDeliveryTarget record;
    UmiWorkbenchContextLinkDeliveryTarget copy;
    uint64_t first_hash;
    umi_workbench_context_link_delivery_target_init(&record, "delivery_target-id");
    assert(umi_workbench_context_link_delivery_target_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkDeliveryTargetTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_delivery_target_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_delivery_target_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_delivery_target_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_delivery_target_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_delivery_target_hash(&copy) == first_hash);
    umi_workbench_context_link_delivery_target_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.target_id, record.target_id) == 0);
    return 0;
}
