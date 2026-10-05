/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/transport_profile.c
 *
 * PURPOSE:
 *   Describe bounded transport limits, heartbeat policy and security requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transport_profile.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric transport profile from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_transport_profile_init(UmiFabricTransportProfile *item, const char *profile_id, uint64_t max_frame_bytes, uint32_t heartbeat_ms, uint32_t idle_timeout_ms, bool compression_allowed, bool tls_required) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->profile_id,sizeof(item->profile_id),profile_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->max_frame_bytes=max_frame_bytes;item->heartbeat_ms=heartbeat_ms;item->idle_timeout_ms=idle_timeout_ms;item->compression_allowed=compression_allowed;item->tls_required=tls_required;
    return umi_fabric_transport_profile_validate(item);
}
/*
 * Check that fabric transport profile satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_transport_profile_validate(const UmiFabricTransportProfile *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->profile_id, '\0', sizeof(item->profile_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->profile_id[0]!='\0' && item->max_frame_bytes>0U && item->heartbeat_ms>0U && item->idle_timeout_ms>=item->heartbeat_ms)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricTransportProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7b49650e76a1b9af);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricTransportProfile *)0)->profile_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricTransportProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricTransportProfile *)0)->profile_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiFabricTransportProfileArchiveWrite(UmiArchiveWriter *writer, const UmiFabricTransportProfile *value)
{
    UmiArchiveWriteText(writer, value->profile_id, sizeof(value->profile_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_frame_bytes);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->heartbeat_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->idle_timeout_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->compression_allowed);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->tls_required);
}
static void UmiFabricTransportProfileArchiveRead(UmiArchiveReader *reader, UmiFabricTransportProfile *value)
{
    UmiArchiveReadText(reader, value->profile_id, sizeof(value->profile_id));
    value->max_frame_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->heartbeat_ms = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->idle_timeout_ms = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->compression_allowed = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->tls_required = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricTransportProfileArchiveValidate(const UmiFabricTransportProfile *value)
{
    return umi_fabric_transport_profile_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_transport_profile_archive_encode, umi_fabric_transport_profile_archive_decode,
    UmiFabricTransportProfile, UmiFabricTransportProfileArchiveSchema, UmiFabricTransportProfileArchiveBound, UmiFabricTransportProfileArchiveWrite, UmiFabricTransportProfileArchiveRead, UmiFabricTransportProfileArchiveValidate)
