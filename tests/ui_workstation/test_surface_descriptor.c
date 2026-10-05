/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_surface_descriptor.c
 *
 * PURPOSE:
 *   Implement the test surface descriptor behavior for
 *   Umicom Framework.
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
#include <stdio.h>
#include "umicom/ui/workstation/surface_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/workstation/surface_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWsSurfaceDescriptorTransferEqual(const UmiWsSurfaceDescriptor *a, const UmiWsSurfaceDescriptor *b)
{
    return a->api_version == b->api_version &&
        strcmp(a->surface_id, b->surface_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->category, b->category) == 0 &&
        strcmp(a->icon_name, b->icon_name) == 0 &&
        a->domain == b->domain &&
        a->kind == b->kind &&
        a->default_region == b->default_region &&
        a->minimum_width == b->minimum_width &&
        a->minimum_height == b->minimum_height &&
        a->closable == b->closable &&
        a->movable == b->movable &&
        a->detachable == b->detachable &&
        a->multi_instance == b->multi_instance;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWsSurfaceDescriptorTransferTails(UmiWsSurfaceDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->surface_id) + 1U;
        memset(value->surface_id + used, 0xa5, sizeof(value->surface_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->category) + 1U;
        memset(value->category + used, 0xa5, sizeof(value->category) - used);
    }
    {
        size_t used = strlen(value->icon_name) + 1U;
        memset(value->icon_name + used, 0xa5, sizeof(value->icon_name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWsSurfaceDescriptorTransferMalformed(const UmiWsSurfaceDescriptor *sample)
{
    (void)sample;
    {
        UmiWsSurfaceDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.surface_id, 'x', sizeof(invalid.surface_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ws_surface_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ws_surface_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated surface_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWsSurfaceDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ws_surface_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ws_surface_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWsSurfaceDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.category, 'x', sizeof(invalid.category));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ws_surface_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ws_surface_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated category was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWsSurfaceDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.icon_name, 'x', sizeof(invalid.icon_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ws_surface_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ws_surface_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated icon_name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWsSurfaceDescriptorTransferCases, UmiWsSurfaceDescriptor,
    umi_ws_surface_descriptor_archive_encode, umi_ws_surface_descriptor_archive_decode,
    UmiWsSurfaceDescriptorTransferEqual, UmiWsSurfaceDescriptorTransferTails, UmiWsSurfaceDescriptorTransferMalformed)

int main(void) {
    UmiWsSurfaceDescriptor d;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_surface_descriptor_init(&d, "studio.editor", "Editor", UMI_WS_DOMAIN_STUDIO, UMI_WS_SURFACE_EDITOR) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_surface_descriptor_validate(&d) != UMI_STATUS_OK) return 2;
    if (UmiWsSurfaceDescriptorTransferCases(&d) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!d.movable || d.minimum_width != 160) return 3;
    puts("surface descriptor: ok");
    return 0;
}
