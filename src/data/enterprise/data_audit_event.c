/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_audit_event.c
 *
 * PURPOSE:
 *   Record immutable data-operation audit evidence without storing application payloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_audit_event.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_data_audit_event_init(UmiDataAuditEvent *item, const char *event_id, const char *operation_id, const char *principal_id, const char *action, uint64_t timestamp, UmiStatus outcome) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->event_id,sizeof(item->event_id),event_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->operation_id,sizeof(item->operation_id),operation_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->principal_id,sizeof(item->principal_id),principal_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->action,sizeof(item->action),action);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->timestamp=timestamp;item->outcome=outcome;
    return umi_data_data_audit_event_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_data_audit_event_validate(const UmiDataAuditEvent *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->event_id, '\0', sizeof(item->event_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->operation_id, '\0', sizeof(item->operation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->principal_id, '\0', sizeof(item->principal_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->action, '\0', sizeof(item->action)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->event_id[0] != '\0' && item->operation_id[0] != '\0' && item->principal_id[0] != '\0' && item->action[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataAuditEventArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfd27bfd1e586b56e);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataAuditEvent *)0)->event_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataAuditEvent *)0)->operation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataAuditEvent *)0)->principal_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataAuditEvent *)0)->action)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataAuditEventArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataAuditEvent *)0)->event_id) - 1U +
        8U + sizeof(((UmiDataAuditEvent *)0)->operation_id) - 1U +
        8U + sizeof(((UmiDataAuditEvent *)0)->principal_id) - 1U +
        8U + sizeof(((UmiDataAuditEvent *)0)->action) - 1U +
        8U +
        8U;
}
static void UmiDataAuditEventArchiveWrite(UmiArchiveWriter *writer, const UmiDataAuditEvent *value)
{
    UmiArchiveWriteText(writer, value->event_id, sizeof(value->event_id));
    UmiArchiveWriteText(writer, value->operation_id, sizeof(value->operation_id));
    UmiArchiveWriteText(writer, value->principal_id, sizeof(value->principal_id));
    UmiArchiveWriteText(writer, value->action, sizeof(value->action));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp);
    UmiArchiveWriteSigned(writer, (int64_t)value->outcome);
}
static void UmiDataAuditEventArchiveRead(UmiArchiveReader *reader, UmiDataAuditEvent *value)
{
    UmiArchiveReadText(reader, value->event_id, sizeof(value->event_id));
    UmiArchiveReadText(reader, value->operation_id, sizeof(value->operation_id));
    UmiArchiveReadText(reader, value->principal_id, sizeof(value->principal_id));
    UmiArchiveReadText(reader, value->action, sizeof(value->action));
    value->timestamp = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->outcome = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDataAuditEventArchiveValidate(const UmiDataAuditEvent *value)
{
    return umi_data_data_audit_event_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_data_audit_event_archive_encode, umi_data_data_audit_event_archive_decode,
    UmiDataAuditEvent, UmiDataAuditEventArchiveSchema, UmiDataAuditEventArchiveBound, UmiDataAuditEventArchiveWrite, UmiDataAuditEventArchiveRead, UmiDataAuditEventArchiveValidate)
