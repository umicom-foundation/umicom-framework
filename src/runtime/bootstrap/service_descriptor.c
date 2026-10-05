/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/service_descriptor.c
 *
 * PURPOSE:
 *   Implement the service descriptor behavior for
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
 * File: src/runtime/bootstrap/service_descriptor.c
 *
 * PURPOSE:
 *   Describe Framework services, ownership scope and lifetime without global variables.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/service_descriptor.h"
#include "../../base/value_archive_internal.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>

/*
 * Initialise bootstrap service descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_service_descriptor_init(
    UmiBootstrapServiceDescriptor *descriptor,
    const char *service_id,
    const char *qualifier,
    const char *provider_id,
    UmiBootstrapScopeKind scope,
    UmiBootstrapLifetimeKind lifetime,
    int32_t priority) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || !umi_bootstrap_id_valid(provider_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(descriptor, 0, sizeof(*descriptor));
    status = umi_bootstrap_service_key_init(&descriptor->key, service_id, qualifier);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(descriptor->provider_id,
                                     sizeof(descriptor->provider_id), provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    descriptor->scope = scope;
    descriptor->lifetime = lifetime;
    descriptor->priority = priority;
    descriptor->enabled = true;
    return umi_bootstrap_service_descriptor_valid(descriptor)
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Check that bootstrap service descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_bootstrap_service_descriptor_valid(
    const UmiBootstrapServiceDescriptor *descriptor) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (descriptor == NULL) return 0;
    if (memchr(descriptor->key.service_id, '\0', sizeof(descriptor->key.service_id)) == NULL) return 0;
    if (memchr(descriptor->key.qualifier, '\0', sizeof(descriptor->key.qualifier)) == NULL) return 0;
    if (memchr(descriptor->provider_id, '\0', sizeof(descriptor->provider_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) return false;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_bootstrap_id_valid(descriptor->key.service_id) ||
        !umi_bootstrap_id_valid(descriptor->provider_id)) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->scope < UMI_BOOTSTRAP_SCOPE_SINGLETON ||
        descriptor->scope > UMI_BOOTSTRAP_SCOPE_TRANSIENT) return false;
    return descriptor->lifetime >= UMI_BOOTSTRAP_LIFETIME_EAGER &&
           descriptor->lifetime <= UMI_BOOTSTRAP_LIFETIME_EXTERNAL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBootstrapServiceDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x12fd5761378c51d8);
    schema = (schema ^ (uint64_t)sizeof(((UmiBootstrapServiceDescriptor *)0)->key.service_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBootstrapServiceDescriptor *)0)->key.qualifier)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBootstrapServiceDescriptor *)0)->provider_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBootstrapServiceDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBootstrapServiceDescriptor *)0)->key.service_id) - 1U +
        8U + sizeof(((UmiBootstrapServiceDescriptor *)0)->key.qualifier) - 1U +
        8U + sizeof(((UmiBootstrapServiceDescriptor *)0)->provider_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiBootstrapServiceDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiBootstrapServiceDescriptor *value)
{
    UmiArchiveWriteText(writer, value->key.service_id, sizeof(value->key.service_id));
    UmiArchiveWriteText(writer, value->key.qualifier, sizeof(value->key.qualifier));
    UmiArchiveWriteText(writer, value->provider_id, sizeof(value->provider_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->scope);
    UmiArchiveWriteSigned(writer, (int64_t)value->lifetime);
    UmiArchiveWriteSigned(writer, (int64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiBootstrapServiceDescriptorArchiveRead(UmiArchiveReader *reader, UmiBootstrapServiceDescriptor *value)
{
    UmiArchiveReadText(reader, value->key.service_id, sizeof(value->key.service_id));
    UmiArchiveReadText(reader, value->key.qualifier, sizeof(value->key.qualifier));
    UmiArchiveReadText(reader, value->provider_id, sizeof(value->provider_id));
    value->scope = (UmiBootstrapScopeKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->lifetime = (UmiBootstrapLifetimeKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->priority = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->flags = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiBootstrapServiceDescriptorArchiveValidate(const UmiBootstrapServiceDescriptor *value)
{
    return umi_bootstrap_service_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_bootstrap_service_descriptor_archive_encode, umi_bootstrap_service_descriptor_archive_decode,
    UmiBootstrapServiceDescriptor, UmiBootstrapServiceDescriptorArchiveSchema, UmiBootstrapServiceDescriptorArchiveBound, UmiBootstrapServiceDescriptorArchiveWrite, UmiBootstrapServiceDescriptorArchiveRead, UmiBootstrapServiceDescriptorArchiveValidate)
