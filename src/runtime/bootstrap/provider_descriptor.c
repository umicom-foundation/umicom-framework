/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/provider_descriptor.c
 *
 * PURPOSE:
 *   Implement the provider descriptor behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/provider_descriptor.c
 *
 * PURPOSE:
 *   Describe modules that contribute replaceable service implementations.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/provider_descriptor.h"
#include "../../base/value_archive_internal.h"


#include <string.h>
/*
 * Initialise bootstrap provider descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_provider_descriptor_init(
    UmiBootstrapProviderDescriptor *descriptor,
    const char *provider_id,
    const char *module_id,
    int32_t priority) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || !umi_bootstrap_id_valid(provider_id) ||
        !umi_bootstrap_id_valid(module_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(descriptor, 0, sizeof(*descriptor));
    status = umi_bootstrap_copy_text(descriptor->provider_id,
        sizeof(descriptor->provider_id), provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(descriptor->module_id,
        sizeof(descriptor->module_id), module_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    descriptor->priority = priority;
    descriptor->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Check that bootstrap provider descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_bootstrap_provider_descriptor_valid(
    const UmiBootstrapProviderDescriptor *descriptor) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (descriptor == NULL) return 0;
    if (memchr(descriptor->provider_id, '\0', sizeof(descriptor->provider_id)) == NULL) return 0;
    if (memchr(descriptor->module_id, '\0', sizeof(descriptor->module_id)) == NULL) return 0;

    return descriptor != NULL &&
           umi_bootstrap_id_valid(descriptor->provider_id) &&
           umi_bootstrap_id_valid(descriptor->module_id);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBootstrapProviderDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd84709aafe1a3fad);
    schema = (schema ^ (uint64_t)sizeof(((UmiBootstrapProviderDescriptor *)0)->provider_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBootstrapProviderDescriptor *)0)->module_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBootstrapProviderDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBootstrapProviderDescriptor *)0)->provider_id) - 1U +
        8U + sizeof(((UmiBootstrapProviderDescriptor *)0)->module_id) - 1U +
        8U +
        8U;
}
static void UmiBootstrapProviderDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiBootstrapProviderDescriptor *value)
{
    UmiArchiveWriteText(writer, value->provider_id, sizeof(value->provider_id));
    UmiArchiveWriteText(writer, value->module_id, sizeof(value->module_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiBootstrapProviderDescriptorArchiveRead(UmiArchiveReader *reader, UmiBootstrapProviderDescriptor *value)
{
    UmiArchiveReadText(reader, value->provider_id, sizeof(value->provider_id));
    UmiArchiveReadText(reader, value->module_id, sizeof(value->module_id));
    value->priority = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiBootstrapProviderDescriptorArchiveValidate(const UmiBootstrapProviderDescriptor *value)
{
    return umi_bootstrap_provider_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_bootstrap_provider_descriptor_archive_encode, umi_bootstrap_provider_descriptor_archive_decode,
    UmiBootstrapProviderDescriptor, UmiBootstrapProviderDescriptorArchiveSchema, UmiBootstrapProviderDescriptorArchiveBound, UmiBootstrapProviderDescriptorArchiveWrite, UmiBootstrapProviderDescriptorArchiveRead, UmiBootstrapProviderDescriptorArchiveValidate)
