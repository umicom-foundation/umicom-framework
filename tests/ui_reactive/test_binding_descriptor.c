/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_descriptor.c
 *
 * PURPOSE:
 *   Exercise the binding descriptor reactive UI contract.
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
#include "umicom/ui/reactive/binding_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/binding_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveBindingDescriptorTransferEqual(const UmiUiReactiveBindingDescriptor *a, const UmiUiReactiveBindingDescriptor *b)
{
    return strcmp(a->binding_id, b->binding_id) == 0 &&
        strcmp(a->source_path, b->source_path) == 0 &&
        strcmp(a->target_path, b->target_path) == 0 &&
        a->direction == b->direction &&
        a->trigger == b->trigger &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveBindingDescriptorTransferTails(UmiUiReactiveBindingDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->binding_id) + 1U;
        memset(value->binding_id + used, 0xa5, sizeof(value->binding_id) - used);
    }
    {
        size_t used = strlen(value->source_path) + 1U;
        memset(value->source_path + used, 0xa5, sizeof(value->source_path) - used);
    }
    {
        size_t used = strlen(value->target_path) + 1U;
        memset(value->target_path + used, 0xa5, sizeof(value->target_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveBindingDescriptorTransferMalformed(const UmiUiReactiveBindingDescriptor *sample)
{
    (void)sample;
    {
        UmiUiReactiveBindingDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.binding_id, 'x', sizeof(invalid.binding_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_binding_descriptor_valid(&invalid)) ||
            umi_ui_reactive_binding_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated binding_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveBindingDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_path, 'x', sizeof(invalid.source_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_binding_descriptor_valid(&invalid)) ||
            umi_ui_reactive_binding_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveBindingDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_path, 'x', sizeof(invalid.target_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_binding_descriptor_valid(&invalid)) ||
            umi_ui_reactive_binding_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveBindingDescriptorTransferCases, UmiUiReactiveBindingDescriptor,
    umi_ui_reactive_binding_descriptor_archive_encode, umi_ui_reactive_binding_descriptor_archive_decode,
    UmiUiReactiveBindingDescriptorTransferEqual, UmiUiReactiveBindingDescriptorTransferTails, UmiUiReactiveBindingDescriptorTransferMalformed)

int main(void) { UmiUiReactiveBindingDescriptor item; umi_ui_reactive_binding_descriptor_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveBindingDescriptor populated = item;
    (void)snprintf(populated.binding_id, sizeof(populated.binding_id), "field-0");
    (void)snprintf(populated.source_path, sizeof(populated.source_path), "field-1");
    (void)snprintf(populated.target_path, sizeof(populated.target_path), "field-2");
    populated.enabled = true;
    if (UmiUiReactiveBindingDescriptorTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_binding_descriptor_valid(&item) ? 0 : 1; }
