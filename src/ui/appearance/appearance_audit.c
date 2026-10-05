/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_audit.c
 *
 * PURPOSE:
 *   Aggregate appearance accessibility, scaling, typography and renderer-parity findings into one audit result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_audit.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_audit_init(UmiAppearanceAppearanceAudit *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->audit_id,sizeof item->audit_id,"appearance.audit");
    item->checks=1U;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_audit_is_valid(const UmiAppearanceAppearanceAudit *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->audit_id, '\0', sizeof(item->audit_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->audit_id) && item->checks >= item->warnings + item->errors);
}
/*
 * Provide the appearance audit evaluate operation used by this module and its client
 * applications.
 */
void umi_appearance_audit_evaluate(UmiAppearanceAppearanceAudit *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item!=NULL)item->passed=item->errors==0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceAppearanceAuditArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3873756b90a4e837);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceAppearanceAudit *)0)->audit_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceAppearanceAuditArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceAppearanceAudit *)0)->audit_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceAppearanceAuditArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceAppearanceAudit *value)
{
    UmiArchiveWriteText(writer, value->audit_id, sizeof(value->audit_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->checks);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->warnings);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->errors);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->passed);
}
static void UmiAppearanceAppearanceAuditArchiveRead(UmiArchiveReader *reader, UmiAppearanceAppearanceAudit *value)
{
    UmiArchiveReadText(reader, value->audit_id, sizeof(value->audit_id));
    value->checks = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->warnings = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->errors = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->passed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceAppearanceAuditArchiveValidate(const UmiAppearanceAppearanceAudit *value)
{
    return umi_appearance_audit_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_audit_archive_encode, umi_appearance_audit_archive_decode,
    UmiAppearanceAppearanceAudit, UmiAppearanceAppearanceAuditArchiveSchema, UmiAppearanceAppearanceAuditArchiveBound, UmiAppearanceAppearanceAuditArchiveWrite, UmiAppearanceAppearanceAuditArchiveRead, UmiAppearanceAppearanceAuditArchiveValidate)
