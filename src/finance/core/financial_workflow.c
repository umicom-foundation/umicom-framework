/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/financial_workflow.c
 *
 * PURPOSE:
 *   Implement reusable financial workflow state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/financial_workflow.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_financial_workflow_init(UmiFinancialWorkflow *item,const char *id,const char *name,const char *parent_id,uint32_t state){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->workflow_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_id_assign(&item->parent_id,parent_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_financial_workflow_is_valid(const UmiFinancialWorkflow *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->workflow_id.value, '\0', sizeof(item->workflow_id.value)) == NULL) return 0;
    if (memchr(item->parent_id.value, '\0', sizeof(item->parent_id.value)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
return item!=NULL&&umi_financial_id_is_valid(&item->workflow_id)&&item->name[0]!='\0'&&umi_financial_id_is_valid(&item->parent_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFinancialWorkflowArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4027e18b6a475b77);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialWorkflow *)0)->workflow_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialWorkflow *)0)->parent_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialWorkflow *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFinancialWorkflowArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFinancialWorkflow *)0)->workflow_id.value) - 1U +
        8U + sizeof(((UmiFinancialWorkflow *)0)->parent_id.value) - 1U +
        8U + sizeof(((UmiFinancialWorkflow *)0)->name) - 1U +
        8U +
        8U;
}
static void UmiFinancialWorkflowArchiveWrite(UmiArchiveWriter *writer, const UmiFinancialWorkflow *value)
{
    UmiArchiveWriteText(writer, value->workflow_id.value, sizeof(value->workflow_id.value));
    UmiArchiveWriteText(writer, value->parent_id.value, sizeof(value->parent_id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiFinancialWorkflowArchiveRead(UmiArchiveReader *reader, UmiFinancialWorkflow *value)
{
    UmiArchiveReadText(reader, value->workflow_id.value, sizeof(value->workflow_id.value));
    UmiArchiveReadText(reader, value->parent_id.value, sizeof(value->parent_id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->state = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFinancialWorkflowArchiveValidate(const UmiFinancialWorkflow *value)
{
    return umi_financial_workflow_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_financial_workflow_archive_encode, umi_financial_workflow_archive_decode,
    UmiFinancialWorkflow, UmiFinancialWorkflowArchiveSchema, UmiFinancialWorkflowArchiveBound, UmiFinancialWorkflowArchiveWrite, UmiFinancialWorkflowArchiveRead, UmiFinancialWorkflowArchiveValidate)
