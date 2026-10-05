/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/connector_descriptor.c
 *
 * PURPOSE:
 *   Describe a connector implementation and supported protocol/capability surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric connector descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_connector_descriptor_init(UmiFabricConnectorDescriptor *item, const char *connector_id, const char *provider, UmiFabricProtocol protocol, uint64_t capability_mask, bool supports_transactions) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->connector_id,sizeof(item->connector_id),connector_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->provider,sizeof(item->provider),provider);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->protocol=protocol;item->capability_mask=capability_mask;item->supports_transactions=supports_transactions;
    return umi_fabric_connector_descriptor_validate(item);
}
/*
 * Check that fabric connector descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_connector_descriptor_validate(const UmiFabricConnectorDescriptor *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->connector_id, '\0', sizeof(item->connector_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->provider, '\0', sizeof(item->provider)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->connector_id[0]!='\0' && item->provider[0]!='\0' && item->protocol>=UMI_FABRIC_PROTOCOL_INPROC && item->protocol<=UMI_FABRIC_PROTOCOL_FILE)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricConnectorDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe51635711fe475f3);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricConnectorDescriptor *)0)->connector_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricConnectorDescriptor *)0)->provider)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricConnectorDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricConnectorDescriptor *)0)->connector_id) - 1U +
        8U + sizeof(((UmiFabricConnectorDescriptor *)0)->provider) - 1U +
        8U +
        8U +
        8U;
}
static void UmiFabricConnectorDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiFabricConnectorDescriptor *value)
{
    UmiArchiveWriteText(writer, value->connector_id, sizeof(value->connector_id));
    UmiArchiveWriteText(writer, value->provider, sizeof(value->provider));
    UmiArchiveWriteSigned(writer, (int64_t)value->protocol);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->capability_mask);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_transactions);
}
static void UmiFabricConnectorDescriptorArchiveRead(UmiArchiveReader *reader, UmiFabricConnectorDescriptor *value)
{
    UmiArchiveReadText(reader, value->connector_id, sizeof(value->connector_id));
    UmiArchiveReadText(reader, value->provider, sizeof(value->provider));
    value->protocol = (UmiFabricProtocol)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->capability_mask = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->supports_transactions = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricConnectorDescriptorArchiveValidate(const UmiFabricConnectorDescriptor *value)
{
    return umi_fabric_connector_descriptor_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_connector_descriptor_archive_encode, umi_fabric_connector_descriptor_archive_decode,
    UmiFabricConnectorDescriptor, UmiFabricConnectorDescriptorArchiveSchema, UmiFabricConnectorDescriptorArchiveBound, UmiFabricConnectorDescriptorArchiveWrite, UmiFabricConnectorDescriptorArchiveRead, UmiFabricConnectorDescriptorArchiveValidate)
