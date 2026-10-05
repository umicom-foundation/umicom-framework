/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_endpoint.c
 *
 * PURPOSE:
 *   Verify endpoint identity, group assignment and kind capabilities.
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
#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_host/endpoint.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextHostEndpointTransferEqual(const UmiWorkbenchContextHostEndpoint *a, const UmiWorkbenchContextHostEndpoint *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->endpoint_id, b->endpoint_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->display_name, b->display_name) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        a->role == b->role &&
        a->state == b->state &&
        a->mode == b->mode &&
        a->accepted_kinds_mask == b->accepted_kinds_mask &&
        a->published_kinds_mask == b->published_kinds_mask &&
        a->delivery_count == b->delivery_count &&
        a->publish_count == b->publish_count &&
        a->revision == b->revision &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchContextHostEndpointTransferTails(UmiWorkbenchContextHostEndpoint *value)
{
    (void)value;
    {
        size_t used = strlen(value->endpoint_id) + 1U;
        memset(value->endpoint_id + used, 0xa5, sizeof(value->endpoint_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->display_name) + 1U;
        memset(value->display_name + used, 0xa5, sizeof(value->display_name) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextHostEndpointTransferMalformed(const UmiWorkbenchContextHostEndpoint *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextHostEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.endpoint_id, 'x', sizeof(invalid.endpoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_host_endpoint_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_host_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated endpoint_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextHostEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_host_endpoint_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_host_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextHostEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_host_endpoint_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_host_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextHostEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_name, 'x', sizeof(invalid.display_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_host_endpoint_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_host_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextHostEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_host_endpoint_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_host_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextHostEndpointTransferCases, UmiWorkbenchContextHostEndpoint,
    umi_workbench_context_host_endpoint_archive_encode, umi_workbench_context_host_endpoint_archive_decode,
    UmiWorkbenchContextHostEndpointTransferEqual, UmiWorkbenchContextHostEndpointTransferTails, UmiWorkbenchContextHostEndpointTransferMalformed)

int main(void)
{

    UmiWorkbenchContextHostEndpoint endpoint;
    umi_workbench_context_host_endpoint_init(&endpoint, "endpoint");
    assert(umi_workbench_context_host_endpoint_set_identity(
        &endpoint, "panel", "application", "Panel") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_endpoint_set_group(
        &endpoint, "blue",
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) == UMI_STATUS_OK);
    endpoint.role = UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR;
    endpoint.accepted_kinds_mask =
        umi_workbench_context_host_kind_mask(UMI_CONTEXT_KIND_SOURCE_LOCATION);
    endpoint.published_kinds_mask =
        umi_workbench_context_host_kind_mask(UMI_CONTEXT_KIND_SELECTION);
    assert(umi_workbench_context_host_endpoint_validate(&endpoint) == UMI_STATUS_OK);
    if (UmiWorkbenchContextHostEndpointTransferCases(&endpoint) != 0) return 1;

    assert(umi_workbench_context_host_endpoint_accepts(
        &endpoint, UMI_CONTEXT_KIND_SOURCE_LOCATION));
    assert(!umi_workbench_context_host_endpoint_accepts(
        &endpoint, UMI_CONTEXT_KIND_TRADE));
    assert(umi_workbench_context_host_endpoint_publishes(
        &endpoint, UMI_CONTEXT_KIND_SELECTION));
    return 0;
}
