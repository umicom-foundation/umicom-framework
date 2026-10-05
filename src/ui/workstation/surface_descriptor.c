/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/surface_descriptor.c
 *
 * PURPOSE:
 *   Implement reusable semantic metadata for dockable workstation surfaces across every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/surface_descriptor.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise ws surface descriptor from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ws_surface_descriptor_init(UmiWsSurfaceDescriptor *descriptor,
                                         const char *surface_id,
                                         const char *label,
                                         UmiWsApplicationDomain domain,
                                         UmiWsSurfaceKind kind) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || !umi_ws_id_valid(surface_id) || label == NULL || label[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *descriptor = (UmiWsSurfaceDescriptor){0};
    descriptor->api_version = UMI_WS_API_VERSION;
    status = umi_ws_copy_text(descriptor->surface_id, sizeof(descriptor->surface_id), surface_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ws_copy_text(descriptor->label, sizeof(descriptor->label), label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ws_copy_text(descriptor->category, sizeof(descriptor->category), "General");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ws_copy_text(descriptor->icon_name, sizeof(descriptor->icon_name), "view-grid-symbolic");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    descriptor->domain = domain;
    descriptor->kind = kind;
    descriptor->default_region = UMI_WS_DOCK_CENTRE;
    descriptor->minimum_width = 160;
    descriptor->minimum_height = 120;
    descriptor->closable = true;
    descriptor->movable = true;
    descriptor->detachable = true;
    descriptor->multi_instance = false;
    return UMI_STATUS_OK;
}

/*
 * Check that ws surface descriptor satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ws_surface_descriptor_validate(const UmiWsSurfaceDescriptor *descriptor) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->surface_id, '\0', sizeof(descriptor->surface_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->label, '\0', sizeof(descriptor->label)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->category, '\0', sizeof(descriptor->category)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(descriptor->icon_name, '\0', sizeof(descriptor->icon_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || descriptor->api_version != UMI_WS_API_VERSION) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_ws_id_valid(descriptor->surface_id) || descriptor->label[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (descriptor->kind < UMI_WS_SURFACE_PANEL || descriptor->kind > UMI_WS_SURFACE_TOOLBAR) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (descriptor->default_region < UMI_WS_DOCK_LEFT || descriptor->default_region > UMI_WS_DOCK_FLOATING) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (descriptor->minimum_width < 0 || descriptor->minimum_height < 0) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWsSurfaceDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x90fd2dfff86a0936);
    schema = (schema ^ (uint64_t)sizeof(((UmiWsSurfaceDescriptor *)0)->surface_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWsSurfaceDescriptor *)0)->label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWsSurfaceDescriptor *)0)->category)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWsSurfaceDescriptor *)0)->icon_name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWsSurfaceDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiWsSurfaceDescriptor *)0)->surface_id) - 1U +
        8U + sizeof(((UmiWsSurfaceDescriptor *)0)->label) - 1U +
        8U + sizeof(((UmiWsSurfaceDescriptor *)0)->category) - 1U +
        8U + sizeof(((UmiWsSurfaceDescriptor *)0)->icon_name) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWsSurfaceDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiWsSurfaceDescriptor *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->surface_id, sizeof(value->surface_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteText(writer, value->category, sizeof(value->category));
    UmiArchiveWriteText(writer, value->icon_name, sizeof(value->icon_name));
    UmiArchiveWriteSigned(writer, (int64_t)value->domain);
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->default_region);
    UmiArchiveWriteSigned(writer, (int64_t)value->minimum_width);
    UmiArchiveWriteSigned(writer, (int64_t)value->minimum_height);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->closable);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->movable);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->detachable);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->multi_instance);
}
static void UmiWsSurfaceDescriptorArchiveRead(UmiArchiveReader *reader, UmiWsSurfaceDescriptor *value)
{
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->surface_id, sizeof(value->surface_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    UmiArchiveReadText(reader, value->category, sizeof(value->category));
    UmiArchiveReadText(reader, value->icon_name, sizeof(value->icon_name));
    value->domain = (UmiWsApplicationDomain)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->kind = (UmiWsSurfaceKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->default_region = (UmiWsDockRegion)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->minimum_width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->minimum_height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->closable = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->movable = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->detachable = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->multi_instance = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiWsSurfaceDescriptorArchiveValidate(const UmiWsSurfaceDescriptor *value)
{
    return umi_ws_surface_descriptor_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ws_surface_descriptor_archive_encode, umi_ws_surface_descriptor_archive_decode,
    UmiWsSurfaceDescriptor, UmiWsSurfaceDescriptorArchiveSchema, UmiWsSurfaceDescriptorArchiveBound, UmiWsSurfaceDescriptorArchiveWrite, UmiWsSurfaceDescriptorArchiveRead, UmiWsSurfaceDescriptorArchiveValidate)
