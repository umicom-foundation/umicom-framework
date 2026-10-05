/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/endpoint_descriptor.c
 *
 * PURPOSE:
 *   Describe a protocol endpoint independently from any transport implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/endpoint_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric endpoint descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_endpoint_descriptor_init(UmiFabricEndpointDescriptor *item, const char *endpoint_id, const char *uri, UmiFabricProtocol protocol, bool secure, uint32_t weight) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->endpoint_id,sizeof(item->endpoint_id),endpoint_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_fabric_copy_text(item->uri,sizeof(item->uri),uri);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; item->protocol=protocol;item->secure=secure;item->weight=weight;
    return umi_fabric_endpoint_descriptor_validate(item);
}
/*
 * Check that fabric endpoint descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_endpoint_descriptor_validate(const UmiFabricEndpointDescriptor *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->endpoint_id, '\0', sizeof(item->endpoint_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->uri, '\0', sizeof(item->uri)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->endpoint_id[0]!='\0' && item->uri[0]!='\0' && item->protocol>=UMI_FABRIC_PROTOCOL_INPROC && item->protocol<=UMI_FABRIC_PROTOCOL_FILE && item->weight>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricEndpointDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6e7a021f6efb9ebc);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricEndpointDescriptor *)0)->endpoint_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricEndpointDescriptor *)0)->uri)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricEndpointDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricEndpointDescriptor *)0)->endpoint_id) - 1U +
        8U + sizeof(((UmiFabricEndpointDescriptor *)0)->uri) - 1U +
        8U +
        8U +
        8U;
}
static void UmiFabricEndpointDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiFabricEndpointDescriptor *value)
{
    UmiArchiveWriteText(writer, value->endpoint_id, sizeof(value->endpoint_id));
    UmiArchiveWriteText(writer, value->uri, sizeof(value->uri));
    UmiArchiveWriteSigned(writer, (int64_t)value->protocol);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->secure);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->weight);
}
static void UmiFabricEndpointDescriptorArchiveRead(UmiArchiveReader *reader, UmiFabricEndpointDescriptor *value)
{
    UmiArchiveReadText(reader, value->endpoint_id, sizeof(value->endpoint_id));
    UmiArchiveReadText(reader, value->uri, sizeof(value->uri));
    value->protocol = (UmiFabricProtocol)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->secure = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->weight = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiFabricEndpointDescriptorArchiveValidate(const UmiFabricEndpointDescriptor *value)
{
    return umi_fabric_endpoint_descriptor_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_endpoint_descriptor_archive_encode, umi_fabric_endpoint_descriptor_archive_decode,
    UmiFabricEndpointDescriptor, UmiFabricEndpointDescriptorArchiveSchema, UmiFabricEndpointDescriptorArchiveBound, UmiFabricEndpointDescriptorArchiveWrite, UmiFabricEndpointDescriptorArchiveRead, UmiFabricEndpointDescriptorArchiveValidate)
