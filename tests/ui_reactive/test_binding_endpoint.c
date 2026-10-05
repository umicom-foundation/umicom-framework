/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_endpoint.c
 *
 * PURPOSE:
 *   Exercise the binding endpoint reactive UI contract.
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
#include "umicom/ui/reactive/binding_endpoint.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/binding_endpoint.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveBindingEndpointTransferEqual(const UmiUiReactiveBindingEndpoint *a, const UmiUiReactiveBindingEndpoint *b)
{
    return strcmp(a->view_id, b->view_id) == 0 &&
        strcmp(a->property_path, b->property_path) == 0 &&
        a->writable == b->writable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveBindingEndpointTransferTails(UmiUiReactiveBindingEndpoint *value)
{
    (void)value;
    {
        size_t used = strlen(value->view_id) + 1U;
        memset(value->view_id + used, 0xa5, sizeof(value->view_id) - used);
    }
    {
        size_t used = strlen(value->property_path) + 1U;
        memset(value->property_path + used, 0xa5, sizeof(value->property_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveBindingEndpointTransferMalformed(const UmiUiReactiveBindingEndpoint *sample)
{
    (void)sample;
    {
        UmiUiReactiveBindingEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.view_id, 'x', sizeof(invalid.view_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_binding_endpoint_valid(&invalid)) ||
            umi_ui_reactive_binding_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated view_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveBindingEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.property_path, 'x', sizeof(invalid.property_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_binding_endpoint_valid(&invalid)) ||
            umi_ui_reactive_binding_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated property_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveBindingEndpointTransferCases, UmiUiReactiveBindingEndpoint,
    umi_ui_reactive_binding_endpoint_archive_encode, umi_ui_reactive_binding_endpoint_archive_decode,
    UmiUiReactiveBindingEndpointTransferEqual, UmiUiReactiveBindingEndpointTransferTails, UmiUiReactiveBindingEndpointTransferMalformed)

int main(void) { UmiUiReactiveBindingEndpoint item; umi_ui_reactive_binding_endpoint_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveBindingEndpoint populated = item;
    (void)snprintf(populated.view_id, sizeof(populated.view_id), "field-0");
    (void)snprintf(populated.property_path, sizeof(populated.property_path), "field-1");
    populated.writable = true;
    if (UmiUiReactiveBindingEndpointTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_binding_endpoint_valid(&item) ? 0 : 1; }
