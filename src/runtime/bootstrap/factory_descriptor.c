/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/factory_descriptor.c
 *
 * PURPOSE:
 *   Implement the factory descriptor behavior for
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
 * File: src/runtime/bootstrap/factory_descriptor.c
 *
 * PURPOSE:
 *   Describe named factories that create services for the canonical service registry.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/factory_descriptor.h"
#include "../../base/value_archive_internal.h"


#include <string.h>
/*
 * Initialise bootstrap factory descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_factory_descriptor_init(
    UmiBootstrapFactoryDescriptor *descriptor,
    const char *factory_id,
    const UmiBootstrapServiceKey *produces,
    int32_t priority) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || produces == NULL || !umi_bootstrap_id_valid(factory_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(descriptor, 0, sizeof(*descriptor));
    status = umi_bootstrap_copy_text(descriptor->factory_id,
        sizeof(descriptor->factory_id), factory_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    descriptor->produces = *produces;
    descriptor->priority = priority;
    descriptor->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Check that bootstrap factory descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_bootstrap_factory_descriptor_valid(
    const UmiBootstrapFactoryDescriptor *descriptor) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (descriptor == NULL) return 0;
    if (memchr(descriptor->factory_id, '\0', sizeof(descriptor->factory_id)) == NULL) return 0;
    if (memchr(descriptor->produces.service_id, '\0', sizeof(descriptor->produces.service_id)) == NULL) return 0;
    if (memchr(descriptor->produces.qualifier, '\0', sizeof(descriptor->produces.qualifier)) == NULL) return 0;

    return descriptor != NULL &&
           umi_bootstrap_id_valid(descriptor->factory_id) &&
           umi_bootstrap_id_valid(descriptor->produces.service_id);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBootstrapFactoryDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x57fb129641176aad);
    schema = (schema ^ (uint64_t)sizeof(((UmiBootstrapFactoryDescriptor *)0)->factory_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBootstrapFactoryDescriptor *)0)->produces.service_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBootstrapFactoryDescriptor *)0)->produces.qualifier)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBootstrapFactoryDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBootstrapFactoryDescriptor *)0)->factory_id) - 1U +
        8U + sizeof(((UmiBootstrapFactoryDescriptor *)0)->produces.service_id) - 1U +
        8U + sizeof(((UmiBootstrapFactoryDescriptor *)0)->produces.qualifier) - 1U +
        8U +
        8U;
}
static void UmiBootstrapFactoryDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiBootstrapFactoryDescriptor *value)
{
    UmiArchiveWriteText(writer, value->factory_id, sizeof(value->factory_id));
    UmiArchiveWriteText(writer, value->produces.service_id, sizeof(value->produces.service_id));
    UmiArchiveWriteText(writer, value->produces.qualifier, sizeof(value->produces.qualifier));
    UmiArchiveWriteSigned(writer, (int64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiBootstrapFactoryDescriptorArchiveRead(UmiArchiveReader *reader, UmiBootstrapFactoryDescriptor *value)
{
    UmiArchiveReadText(reader, value->factory_id, sizeof(value->factory_id));
    UmiArchiveReadText(reader, value->produces.service_id, sizeof(value->produces.service_id));
    UmiArchiveReadText(reader, value->produces.qualifier, sizeof(value->produces.qualifier));
    value->priority = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiBootstrapFactoryDescriptorArchiveValidate(const UmiBootstrapFactoryDescriptor *value)
{
    return umi_bootstrap_factory_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_bootstrap_factory_descriptor_archive_encode, umi_bootstrap_factory_descriptor_archive_decode,
    UmiBootstrapFactoryDescriptor, UmiBootstrapFactoryDescriptorArchiveSchema, UmiBootstrapFactoryDescriptorArchiveBound, UmiBootstrapFactoryDescriptorArchiveWrite, UmiBootstrapFactoryDescriptorArchiveRead, UmiBootstrapFactoryDescriptorArchiveValidate)
