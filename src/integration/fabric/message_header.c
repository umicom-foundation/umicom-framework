/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/message_header.c
 *
 * PURPOSE:
 *   Represent immutable message identity, correlation and tenant metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/message_header.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric message header from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_message_header_init(UmiFabricMessageHeader *item, const char *message_id, const char *correlation_id, const char *tenant_id, const char *content_type, uint64_t created_ms) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->message_id,sizeof(item->message_id),message_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->correlation_id,sizeof(item->correlation_id),correlation_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->tenant_id,sizeof(item->tenant_id),tenant_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->content_type,sizeof(item->content_type),content_type);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->created_ms=created_ms;
    return umi_fabric_message_header_validate(item);
}
/*
 * Check that fabric message header satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_message_header_validate(const UmiFabricMessageHeader *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->message_id, '\0', sizeof(item->message_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->correlation_id, '\0', sizeof(item->correlation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->causation_id, '\0', sizeof(item->causation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->tenant_id, '\0', sizeof(item->tenant_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->content_type, '\0', sizeof(item->content_type)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->message_id[0]!='\0' && item->correlation_id[0]!='\0' && item->content_type[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricMessageHeaderArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x570fce8292409732);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricMessageHeader *)0)->message_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricMessageHeader *)0)->correlation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricMessageHeader *)0)->causation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricMessageHeader *)0)->tenant_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricMessageHeader *)0)->content_type)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricMessageHeaderArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricMessageHeader *)0)->message_id) - 1U +
        8U + sizeof(((UmiFabricMessageHeader *)0)->correlation_id) - 1U +
        8U + sizeof(((UmiFabricMessageHeader *)0)->causation_id) - 1U +
        8U + sizeof(((UmiFabricMessageHeader *)0)->tenant_id) - 1U +
        8U + sizeof(((UmiFabricMessageHeader *)0)->content_type) - 1U +
        8U;
}
static void UmiFabricMessageHeaderArchiveWrite(UmiArchiveWriter *writer, const UmiFabricMessageHeader *value)
{
    UmiArchiveWriteText(writer, value->message_id, sizeof(value->message_id));
    UmiArchiveWriteText(writer, value->correlation_id, sizeof(value->correlation_id));
    UmiArchiveWriteText(writer, value->causation_id, sizeof(value->causation_id));
    UmiArchiveWriteText(writer, value->tenant_id, sizeof(value->tenant_id));
    UmiArchiveWriteText(writer, value->content_type, sizeof(value->content_type));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->created_ms);
}
static void UmiFabricMessageHeaderArchiveRead(UmiArchiveReader *reader, UmiFabricMessageHeader *value)
{
    UmiArchiveReadText(reader, value->message_id, sizeof(value->message_id));
    UmiArchiveReadText(reader, value->correlation_id, sizeof(value->correlation_id));
    UmiArchiveReadText(reader, value->causation_id, sizeof(value->causation_id));
    UmiArchiveReadText(reader, value->tenant_id, sizeof(value->tenant_id));
    UmiArchiveReadText(reader, value->content_type, sizeof(value->content_type));
    value->created_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiFabricMessageHeaderArchiveValidate(const UmiFabricMessageHeader *value)
{
    return umi_fabric_message_header_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_message_header_archive_encode, umi_fabric_message_header_archive_decode,
    UmiFabricMessageHeader, UmiFabricMessageHeaderArchiveSchema, UmiFabricMessageHeaderArchiveBound, UmiFabricMessageHeaderArchiveWrite, UmiFabricMessageHeaderArchiveRead, UmiFabricMessageHeaderArchiveValidate)
