/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/connector_binding.c
 *
 * PURPOSE:
 *   Bind a connector to one endpoint/profile without embedding product-specific connection logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric connector binding from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_connector_binding_init(UmiFabricConnectorBinding *item, const char *binding_id, const char *connector_id, const char *endpoint_id, const char *transport_profile_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->binding_id,sizeof(item->binding_id),binding_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->connector_id,sizeof(item->connector_id),connector_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->endpoint_id,sizeof(item->endpoint_id),endpoint_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->transport_profile_id,sizeof(item->transport_profile_id),transport_profile_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->enabled=true;
    return umi_fabric_connector_binding_validate(item);
}
/*
 * Check that fabric connector binding satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_connector_binding_validate(const UmiFabricConnectorBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->binding_id, '\0', sizeof(item->binding_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->connector_id, '\0', sizeof(item->connector_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->endpoint_id, '\0', sizeof(item->endpoint_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->transport_profile_id, '\0', sizeof(item->transport_profile_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->binding_id[0]!='\0' && item->connector_id[0]!='\0' && item->endpoint_id[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricConnectorBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1ddd18444e7e07e4);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricConnectorBinding *)0)->binding_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricConnectorBinding *)0)->connector_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricConnectorBinding *)0)->endpoint_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricConnectorBinding *)0)->transport_profile_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricConnectorBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricConnectorBinding *)0)->binding_id) - 1U +
        8U + sizeof(((UmiFabricConnectorBinding *)0)->connector_id) - 1U +
        8U + sizeof(((UmiFabricConnectorBinding *)0)->endpoint_id) - 1U +
        8U + sizeof(((UmiFabricConnectorBinding *)0)->transport_profile_id) - 1U +
        8U;
}
static void UmiFabricConnectorBindingArchiveWrite(UmiArchiveWriter *writer, const UmiFabricConnectorBinding *value)
{
    UmiArchiveWriteText(writer, value->binding_id, sizeof(value->binding_id));
    UmiArchiveWriteText(writer, value->connector_id, sizeof(value->connector_id));
    UmiArchiveWriteText(writer, value->endpoint_id, sizeof(value->endpoint_id));
    UmiArchiveWriteText(writer, value->transport_profile_id, sizeof(value->transport_profile_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiFabricConnectorBindingArchiveRead(UmiArchiveReader *reader, UmiFabricConnectorBinding *value)
{
    UmiArchiveReadText(reader, value->binding_id, sizeof(value->binding_id));
    UmiArchiveReadText(reader, value->connector_id, sizeof(value->connector_id));
    UmiArchiveReadText(reader, value->endpoint_id, sizeof(value->endpoint_id));
    UmiArchiveReadText(reader, value->transport_profile_id, sizeof(value->transport_profile_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricConnectorBindingArchiveValidate(const UmiFabricConnectorBinding *value)
{
    return umi_fabric_connector_binding_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_connector_binding_archive_encode, umi_fabric_connector_binding_archive_decode,
    UmiFabricConnectorBinding, UmiFabricConnectorBindingArchiveSchema, UmiFabricConnectorBindingArchiveBound, UmiFabricConnectorBindingArchiveWrite, UmiFabricConnectorBindingArchiveRead, UmiFabricConnectorBindingArchiveValidate)
