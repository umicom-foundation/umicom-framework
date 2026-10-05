/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_instance.c
 *
 * PURPOSE:
 *   Represent a live service instance advertised to the Fabric registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_instance.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric service instance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_service_instance_init(UmiFabricServiceInstance *item, const char *instance_id, const char *service_id, const char *endpoint_id, uint32_t priority, uint32_t weight) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->instance_id,sizeof(item->instance_id),instance_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_fabric_copy_text(item->service_id,sizeof(item->service_id),service_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_fabric_copy_text(item->endpoint_id,sizeof(item->endpoint_id),endpoint_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    item->priority=priority; item->weight=weight; item->healthy=true;
    return umi_fabric_service_instance_validate(item);
}
/*
 * Check that fabric service instance satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_service_instance_validate(const UmiFabricServiceInstance *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->instance_id, '\0', sizeof(item->instance_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->service_id, '\0', sizeof(item->service_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->endpoint_id, '\0', sizeof(item->endpoint_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->instance_id[0]!='\0' && item->service_id[0]!='\0' && item->endpoint_id[0]!='\0' && item->weight>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricServiceInstanceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf007e1f86313e5dc);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricServiceInstance *)0)->instance_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricServiceInstance *)0)->service_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricServiceInstance *)0)->endpoint_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricServiceInstanceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricServiceInstance *)0)->instance_id) - 1U +
        8U + sizeof(((UmiFabricServiceInstance *)0)->service_id) - 1U +
        8U + sizeof(((UmiFabricServiceInstance *)0)->endpoint_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiFabricServiceInstanceArchiveWrite(UmiArchiveWriter *writer, const UmiFabricServiceInstance *value)
{
    UmiArchiveWriteText(writer, value->instance_id, sizeof(value->instance_id));
    UmiArchiveWriteText(writer, value->service_id, sizeof(value->service_id));
    UmiArchiveWriteText(writer, value->endpoint_id, sizeof(value->endpoint_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->weight);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->healthy);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_seen_ms);
}
static void UmiFabricServiceInstanceArchiveRead(UmiArchiveReader *reader, UmiFabricServiceInstance *value)
{
    UmiArchiveReadText(reader, value->instance_id, sizeof(value->instance_id));
    UmiArchiveReadText(reader, value->service_id, sizeof(value->service_id));
    UmiArchiveReadText(reader, value->endpoint_id, sizeof(value->endpoint_id));
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->weight = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->healthy = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->last_seen_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiFabricServiceInstanceArchiveValidate(const UmiFabricServiceInstance *value)
{
    return umi_fabric_service_instance_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_service_instance_archive_encode, umi_fabric_service_instance_archive_decode,
    UmiFabricServiceInstance, UmiFabricServiceInstanceArchiveSchema, UmiFabricServiceInstanceArchiveBound, UmiFabricServiceInstanceArchiveWrite, UmiFabricServiceInstanceArchiveRead, UmiFabricServiceInstanceArchiveValidate)
