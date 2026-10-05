/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_descriptor.c
 *
 * PURPOSE:
 *   Verify provider identity, routing and validation.
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

#include "umicom/workbench_selection_provider/descriptor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_selection_provider/descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchSelectionProviderDescriptorTransferEqual(const UmiWorkbenchSelectionProviderDescriptor *a, const UmiWorkbenchSelectionProviderDescriptor *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->provider_id, b->provider_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->display_name, b->display_name) == 0 &&
        strcmp(a->default_source_id, b->default_source_id) == 0 &&
        strcmp(a->default_group_id, b->default_group_id) == 0 &&
        a->kind == b->kind &&
        a->state == b->state &&
        a->selection_kind == b->selection_kind &&
        a->context_kind == b->context_kind &&
        a->capabilities == b->capabilities &&
        a->revision == b->revision &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchSelectionProviderDescriptorTransferTails(UmiWorkbenchSelectionProviderDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->provider_id) + 1U;
        memset(value->provider_id + used, 0xa5, sizeof(value->provider_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->display_name) + 1U;
        memset(value->display_name + used, 0xa5, sizeof(value->display_name) - used);
    }
    {
        size_t used = strlen(value->default_source_id) + 1U;
        memset(value->default_source_id + used, 0xa5, sizeof(value->default_source_id) - used);
    }
    {
        size_t used = strlen(value->default_group_id) + 1U;
        memset(value->default_group_id + used, 0xa5, sizeof(value->default_group_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchSelectionProviderDescriptorTransferMalformed(const UmiWorkbenchSelectionProviderDescriptor *sample)
{
    (void)sample;
    {
        UmiWorkbenchSelectionProviderDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.provider_id, 'x', sizeof(invalid.provider_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_provider_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_provider_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated provider_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionProviderDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_provider_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_provider_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionProviderDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_provider_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_provider_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionProviderDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_name, 'x', sizeof(invalid.display_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_provider_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_provider_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionProviderDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.default_source_id, 'x', sizeof(invalid.default_source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_provider_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_provider_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated default_source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionProviderDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.default_group_id, 'x', sizeof(invalid.default_group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_provider_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_provider_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated default_group_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchSelectionProviderDescriptorTransferCases, UmiWorkbenchSelectionProviderDescriptor,
    umi_workbench_selection_provider_descriptor_archive_encode, umi_workbench_selection_provider_descriptor_archive_decode,
    UmiWorkbenchSelectionProviderDescriptorTransferEqual, UmiWorkbenchSelectionProviderDescriptorTransferTails, UmiWorkbenchSelectionProviderDescriptorTransferMalformed)

int main(void)
{
    UmiWorkbenchSelectionProviderDescriptor descriptor;
    umi_workbench_selection_provider_descriptor_init(
        &descriptor, "studio.provider.project");
    assert(umi_workbench_selection_provider_descriptor_set_identity(
        &descriptor, "org.umicom.studio", "studio.project-explorer",
        "Project") == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_descriptor_set_routing(
        &descriptor, "studio.project.selection", "") == UMI_STATUS_OK);
    descriptor.kind = UMI_WORKBENCH_SELECTION_PROVIDER_PROJECT;
    descriptor.state = UMI_WORKBENCH_SELECTION_PROVIDER_ACTIVE;
    descriptor.selection_kind = UMI_WORKBENCH_SELECTION_PROJECT;
    descriptor.context_kind = UMI_CONTEXT_KIND_PROJECT;
    assert(umi_workbench_selection_provider_descriptor_validate(
        &descriptor) == UMI_STATUS_OK);
    if (UmiWorkbenchSelectionProviderDescriptorTransferCases(&descriptor) != 0) return 1;


    return 0;
}
