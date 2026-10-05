/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/descriptor.c
 *
 * PURPOSE:
 *   Implement provider descriptor construction, routing identity and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/descriptor.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench selection provider descriptor from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_descriptor_init(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *provider_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) return;
    memset(descriptor, 0, sizeof(*descriptor));
    descriptor->structure_size = (uint32_t)sizeof(*descriptor);
    descriptor->kind = UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC;
    descriptor->state = UMI_WORKBENCH_SELECTION_PROVIDER_CREATED;
    descriptor->selection_kind = UMI_WORKBENCH_SELECTION_GENERIC;
    descriptor->context_kind = UMI_CONTEXT_KIND_SELECTION;
    descriptor->enabled = true;
    descriptor->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider_id != NULL) {
        (void)umi_workbench_selection_provider_copy_text(
            descriptor->provider_id,
            sizeof(descriptor->provider_id),
            provider_id);
    }
}

/*
 * Provide the workbench selection provider descriptor set identity operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_descriptor_set_identity(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *application_id,
    const char *panel_id,
    const char *display_name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || application_id == NULL ||
        panel_id == NULL || display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_copy_text(
        descriptor->application_id,
        sizeof(descriptor->application_id),
        application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        descriptor->panel_id,
        sizeof(descriptor->panel_id),
        panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        descriptor->display_name,
        sizeof(descriptor->display_name),
        display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++descriptor->revision;
    return status;
}

/*
 * Provide the workbench selection provider descriptor set routing operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_descriptor_set_routing(
    UmiWorkbenchSelectionProviderDescriptor *descriptor,
    const char *source_id,
    const char *group_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || source_id == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_copy_text(
        descriptor->default_source_id,
        sizeof(descriptor->default_source_id),
        source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_provider_copy_text(
        descriptor->default_group_id,
        sizeof(descriptor->default_group_id),
        group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++descriptor->revision;
    return status;
}

/*
 * Check that workbench selection provider descriptor satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_provider_descriptor_validate(
    const UmiWorkbenchSelectionProviderDescriptor *descriptor)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->provider_id, '\0', sizeof(descriptor->provider_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->application_id, '\0', sizeof(descriptor->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->panel_id, '\0', sizeof(descriptor->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->display_name, '\0', sizeof(descriptor->display_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->default_source_id, '\0', sizeof(descriptor->default_source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->default_group_id, '\0', sizeof(descriptor->default_group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL ||
        descriptor->structure_size != sizeof(*descriptor) ||
        descriptor->provider_id[0] == '\0' ||
        descriptor->application_id[0] == '\0' ||
        descriptor->panel_id[0] == '\0' ||
        descriptor->kind < UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC ||
        descriptor->kind > UMI_WORKBENCH_SELECTION_PROVIDER_MEDIA ||
        descriptor->state < UMI_WORKBENCH_SELECTION_PROVIDER_CREATED ||
        descriptor->state > UMI_WORKBENCH_SELECTION_PROVIDER_STOPPED ||
        descriptor->selection_kind < UMI_WORKBENCH_SELECTION_GENERIC ||
        descriptor->selection_kind > UMI_WORKBENCH_SELECTION_MEDIA ||
        descriptor->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        descriptor->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchSelectionProviderDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xba1dea2551ec0d0c);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->provider_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->display_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->default_source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->default_group_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchSelectionProviderDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->provider_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->application_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->panel_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->display_name) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->default_source_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderDescriptor *)0)->default_group_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchSelectionProviderDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchSelectionProviderDescriptor *value)
{
    UmiArchiveWriteText(writer, value->provider_id, sizeof(value->provider_id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->display_name, sizeof(value->display_name));
    UmiArchiveWriteText(writer, value->default_source_id, sizeof(value->default_source_id));
    UmiArchiveWriteText(writer, value->default_group_id, sizeof(value->default_group_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteSigned(writer, (int64_t)value->selection_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->context_kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->capabilities);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiWorkbenchSelectionProviderDescriptorArchiveRead(UmiArchiveReader *reader, UmiWorkbenchSelectionProviderDescriptor *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->provider_id, sizeof(value->provider_id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->display_name, sizeof(value->display_name));
    UmiArchiveReadText(reader, value->default_source_id, sizeof(value->default_source_id));
    UmiArchiveReadText(reader, value->default_group_id, sizeof(value->default_group_id));
    value->kind = (UmiWorkbenchSelectionProviderKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->state = (UmiWorkbenchSelectionProviderRuntimeState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->selection_kind = (UmiWorkbenchSelectionKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->context_kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->capabilities = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiWorkbenchSelectionProviderDescriptorArchiveValidate(const UmiWorkbenchSelectionProviderDescriptor *value)
{
    return umi_workbench_selection_provider_descriptor_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_selection_provider_descriptor_archive_encode, umi_workbench_selection_provider_descriptor_archive_decode,
    UmiWorkbenchSelectionProviderDescriptor, UmiWorkbenchSelectionProviderDescriptorArchiveSchema, UmiWorkbenchSelectionProviderDescriptorArchiveBound, UmiWorkbenchSelectionProviderDescriptorArchiveWrite, UmiWorkbenchSelectionProviderDescriptorArchiveRead, UmiWorkbenchSelectionProviderDescriptorArchiveValidate)
