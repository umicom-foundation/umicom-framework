/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_capability.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/capability.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/capability.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextCapabilityTransferEqual(const UmiContextCapability *a, const UmiContextCapability *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->capability_id, b->capability_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        a->can_publish == b->can_publish &&
        a->can_observe == b->can_observe &&
        a->can_share_cross_application == b->can_share_cross_application &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextCapabilityTransferTails(UmiContextCapability *value)
{
    (void)value;
    {
        size_t used = strlen(value->capability_id) + 1U;
        memset(value->capability_id + used, 0xa5, sizeof(value->capability_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextCapabilityTransferMalformed(const UmiContextCapability *sample)
{
    (void)sample;
    {
        UmiContextCapability invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.capability_id, 'x', sizeof(invalid.capability_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_capability_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_capability_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated capability_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextCapability invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_capability_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_capability_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextCapability invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_capability_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_capability_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextCapabilityTransferCases, UmiContextCapability,
    umi_context_capability_archive_encode, umi_context_capability_archive_decode,
    UmiContextCapabilityTransferEqual, UmiContextCapabilityTransferTails, UmiContextCapabilityTransferMalformed)

int main(void)
{
    UmiContextCapability value;
    umi_context_capability_init(&value);
    value.capability_id[0] = 's';
    value.application_id[0] = 's';
    value.schema_id[0] = 's';
    value.can_publish = true;
    value.can_observe = true;
    value.can_share_cross_application = true;
    value.revision = (uint64_t)17U;
    if (umi_context_capability_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextCapabilityTransferCases(&value) != 0) return 1;

    return 0;
}
