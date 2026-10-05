/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/financial_audit.c
 *
 * PURPOSE:
 *   Implement auditable financial-domain evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/financial_audit.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_financial_audit_init(UmiFinancialAuditRecord *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->audit_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_id_assign(&item->parent_id,parent_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_financial_date_is_valid(effective_date))return UMI_STATUS_INVALID_ARGUMENT; item->effective_date=effective_date; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_financial_audit_is_valid(const UmiFinancialAuditRecord *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->audit_id.value, '\0', sizeof(item->audit_id.value)) == NULL) return 0;
    if (memchr(item->parent_id.value, '\0', sizeof(item->parent_id.value)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
return item!=NULL&&umi_financial_id_is_valid(&item->audit_id)&&item->name[0]!='\0'&&umi_financial_id_is_valid(&item->parent_id)&&umi_financial_date_is_valid(item->effective_date);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFinancialAuditRecordArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6197ed34660b0830);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialAuditRecord *)0)->audit_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialAuditRecord *)0)->parent_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialAuditRecord *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFinancialAuditRecordArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFinancialAuditRecord *)0)->audit_id.value) - 1U +
        8U + sizeof(((UmiFinancialAuditRecord *)0)->parent_id.value) - 1U +
        8U + sizeof(((UmiFinancialAuditRecord *)0)->name) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiFinancialAuditRecordArchiveWrite(UmiArchiveWriter *writer, const UmiFinancialAuditRecord *value)
{
    UmiArchiveWriteText(writer, value->audit_id.value, sizeof(value->audit_id.value));
    UmiArchiveWriteText(writer, value->parent_id.value, sizeof(value->parent_id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteSigned(writer, (int64_t)value->effective_date.year);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->effective_date.month);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->effective_date.day);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiFinancialAuditRecordArchiveRead(UmiArchiveReader *reader, UmiFinancialAuditRecord *value)
{
    UmiArchiveReadText(reader, value->audit_id.value, sizeof(value->audit_id.value));
    UmiArchiveReadText(reader, value->parent_id.value, sizeof(value->parent_id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->effective_date.year = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->effective_date.month = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->effective_date.day = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->state = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFinancialAuditRecordArchiveValidate(const UmiFinancialAuditRecord *value)
{
    return umi_financial_audit_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_financial_audit_archive_encode, umi_financial_audit_archive_decode,
    UmiFinancialAuditRecord, UmiFinancialAuditRecordArchiveSchema, UmiFinancialAuditRecordArchiveBound, UmiFinancialAuditRecordArchiveWrite, UmiFinancialAuditRecordArchiveRead, UmiFinancialAuditRecordArchiveValidate)
