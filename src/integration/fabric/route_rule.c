/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/route_rule.c
 *
 * PURPOSE:
 *   Describe ordered route matching by source, destination and message pattern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/route_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric route rule from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_fabric_route_rule_init(UmiFabricRouteRule *item, const char *route_id, const char *source_pattern, const char *message_pattern, const char *destination_id, uint32_t priority) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->route_id,sizeof(item->route_id),route_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->source_pattern,sizeof(item->source_pattern),source_pattern);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->message_pattern,sizeof(item->message_pattern),message_pattern);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->destination_id,sizeof(item->destination_id),destination_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->priority=priority;item->enabled=true;
    return umi_fabric_route_rule_validate(item);
}
/* Check that fabric route rule satisfies its contract before another service relies on it. */
UmiStatus umi_fabric_route_rule_validate(const UmiFabricRouteRule *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->route_id, '\0', sizeof(item->route_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->source_pattern, '\0', sizeof(item->source_pattern)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->message_pattern, '\0', sizeof(item->message_pattern)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->destination_id, '\0', sizeof(item->destination_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->route_id[0]!='\0' && item->message_pattern[0]!='\0' && item->destination_id[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricRouteRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3dbed74bd7f78184);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricRouteRule *)0)->route_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricRouteRule *)0)->source_pattern)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricRouteRule *)0)->message_pattern)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricRouteRule *)0)->destination_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricRouteRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricRouteRule *)0)->route_id) - 1U +
        8U + sizeof(((UmiFabricRouteRule *)0)->source_pattern) - 1U +
        8U + sizeof(((UmiFabricRouteRule *)0)->message_pattern) - 1U +
        8U + sizeof(((UmiFabricRouteRule *)0)->destination_id) - 1U +
        8U +
        8U;
}
static void UmiFabricRouteRuleArchiveWrite(UmiArchiveWriter *writer, const UmiFabricRouteRule *value)
{
    UmiArchiveWriteText(writer, value->route_id, sizeof(value->route_id));
    UmiArchiveWriteText(writer, value->source_pattern, sizeof(value->source_pattern));
    UmiArchiveWriteText(writer, value->message_pattern, sizeof(value->message_pattern));
    UmiArchiveWriteText(writer, value->destination_id, sizeof(value->destination_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiFabricRouteRuleArchiveRead(UmiArchiveReader *reader, UmiFabricRouteRule *value)
{
    UmiArchiveReadText(reader, value->route_id, sizeof(value->route_id));
    UmiArchiveReadText(reader, value->source_pattern, sizeof(value->source_pattern));
    UmiArchiveReadText(reader, value->message_pattern, sizeof(value->message_pattern));
    UmiArchiveReadText(reader, value->destination_id, sizeof(value->destination_id));
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricRouteRuleArchiveValidate(const UmiFabricRouteRule *value)
{
    return umi_fabric_route_rule_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_route_rule_archive_encode, umi_fabric_route_rule_archive_decode,
    UmiFabricRouteRule, UmiFabricRouteRuleArchiveSchema, UmiFabricRouteRuleArchiveBound, UmiFabricRouteRuleArchiveWrite, UmiFabricRouteRuleArchiveRead, UmiFabricRouteRuleArchiveValidate)
