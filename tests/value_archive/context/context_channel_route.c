/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_route.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/route.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/route.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextRouteTransferEqual(const UmiContextRoute *a, const UmiContextRoute *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->route_id, b->route_id) == 0 &&
        strcmp(a->source_channel_id, b->source_channel_id) == 0 &&
        strcmp(a->target_channel_id, b->target_channel_id) == 0 &&
        strcmp(a->required_schema_id, b->required_schema_id) == 0 &&
        a->enabled == b->enabled &&
        a->allow_self_route == b->allow_self_route &&
        a->priority == b->priority &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextRouteTransferTails(UmiContextRoute *value)
{
    (void)value;
    {
        size_t used = strlen(value->route_id) + 1U;
        memset(value->route_id + used, 0xa5, sizeof(value->route_id) - used);
    }
    {
        size_t used = strlen(value->source_channel_id) + 1U;
        memset(value->source_channel_id + used, 0xa5, sizeof(value->source_channel_id) - used);
    }
    {
        size_t used = strlen(value->target_channel_id) + 1U;
        memset(value->target_channel_id + used, 0xa5, sizeof(value->target_channel_id) - used);
    }
    {
        size_t used = strlen(value->required_schema_id) + 1U;
        memset(value->required_schema_id + used, 0xa5, sizeof(value->required_schema_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextRouteTransferMalformed(const UmiContextRoute *sample)
{
    (void)sample;
    {
        UmiContextRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.route_id, 'x', sizeof(invalid.route_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_route_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated route_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_channel_id, 'x', sizeof(invalid.source_channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_route_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_channel_id, 'x', sizeof(invalid.target_channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_route_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.required_schema_id, 'x', sizeof(invalid.required_schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_route_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated required_schema_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextRouteTransferCases, UmiContextRoute,
    umi_context_route_archive_encode, umi_context_route_archive_decode,
    UmiContextRouteTransferEqual, UmiContextRouteTransferTails, UmiContextRouteTransferMalformed)

int main(void)
{
    UmiContextRoute value;
    umi_context_route_init(&value);
    value.route_id[0] = 's';
    value.source_channel_id[0] = 's';
    value.target_channel_id[0] = 's';
    value.required_schema_id[0] = 's';
    value.enabled = true;
    value.allow_self_route = true;
    value.priority = (uint32_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_route_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextRouteTransferCases(&value) != 0) return 1;

    return 0;
}
