/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_audit.c
 *
 * PURPOSE:
 *   Implement record treasury audit evidence with actor and monotonically increasing sequence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_audit.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury treasury audit from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_audit_init(UmiTreasuryTreasuryAudit *value,
    const char *id,
    const char *actor_id,
    uint64_t sequence) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->actor_id,sizeof value->actor_id,actor_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    value->sequence=sequence;
    return umi_treasury_treasury_audit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury treasury audit satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_treasury_audit_valid(const UmiTreasuryTreasuryAudit *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->actor_id, '\0', sizeof(value->actor_id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->actor_id) && value->sequence > 0U);
}

/*
 * Provide the treasury treasury audit sequenced operation used by this module and its
 * client applications.
 */
bool umi_treasury_treasury_audit_sequenced(const UmiTreasuryTreasuryAudit *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->sequence > 0U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryTreasuryAuditArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x95aed5f05a1b1da4);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryTreasuryAudit *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryTreasuryAudit *)0)->actor_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryTreasuryAuditArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryTreasuryAudit *)0)->id) - 1U +
        8U + sizeof(((UmiTreasuryTreasuryAudit *)0)->actor_id) - 1U +
        8U;
}
static void UmiTreasuryTreasuryAuditArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryTreasuryAudit *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->actor_id, sizeof(value->actor_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
}
static void UmiTreasuryTreasuryAuditArchiveRead(UmiArchiveReader *reader, UmiTreasuryTreasuryAudit *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->actor_id, sizeof(value->actor_id));
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiTreasuryTreasuryAuditArchiveValidate(const UmiTreasuryTreasuryAudit *value)
{
    return umi_treasury_treasury_audit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_treasury_audit_archive_encode, umi_treasury_treasury_audit_archive_decode,
    UmiTreasuryTreasuryAudit, UmiTreasuryTreasuryAuditArchiveSchema, UmiTreasuryTreasuryAuditArchiveBound, UmiTreasuryTreasuryAuditArchiveWrite, UmiTreasuryTreasuryAuditArchiveRead, UmiTreasuryTreasuryAuditArchiveValidate)
