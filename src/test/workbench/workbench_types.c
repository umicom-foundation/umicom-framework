/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test/workbench/workbench_types.c
 *
 * PURPOSE:
 *   Implement bounded common Test/Quality workbench value operations.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/workbench_types.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Exercise test workbench copy text and return a clear result when the behaviour no longer
 * matches its contract.
 */
UmiStatus umi_test_workbench_copy_text(char *destination,size_t capacity,const char *source){size_t n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(destination==NULL||capacity==0U||source==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(source);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(destination,source,n+1U);return UMI_STATUS_OK;}
/*
 * Initialise test workbench entry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_test_workbench_entry_init(UmiTestWorkbenchEntry *entry,const char *id,const char *label){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(entry==NULL||id==NULL||label==NULL||id[0]=='\0'||label[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(entry,0,sizeof *entry);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_test_workbench_copy_text(entry->id,sizeof entry->id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_test_workbench_copy_text(entry->label,sizeof entry->label,label)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;entry->state=UMI_TEST_WORKBENCH_STATE_IDLE;entry->revision=1U;return UMI_STATUS_OK;}
/*
 * Exercise test workbench entry set detail and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_workbench_entry_set_detail(UmiTestWorkbenchEntry *entry,const char *detail){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(entry==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_test_workbench_copy_text(entry->detail,sizeof entry->detail,detail!=NULL?detail:"");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)entry->revision++;return s;}
/*
 * Exercise test workbench entry set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_workbench_entry_set_state(UmiTestWorkbenchEntry *entry,UmiTestWorkbenchState state){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(entry==NULL||state<UMI_TEST_WORKBENCH_STATE_IDLE||state>UMI_TEST_WORKBENCH_STATE_CANCELLED)return UMI_STATUS_INVALID_ARGUMENT;entry->state=state;entry->revision++;return UMI_STATUS_OK;}
/*
 * Check that test workbench entry satisfies its contract before another service relies on
 * it.
 */
int umi_test_workbench_entry_valid(const UmiTestWorkbenchEntry *entry){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (entry == NULL) return 0;
    if (memchr(entry->id, '\0', sizeof(entry->id)) == NULL) return 0;
    if (memchr(entry->label, '\0', sizeof(entry->label)) == NULL) return 0;
    if (memchr(entry->detail, '\0', sizeof(entry->detail)) == NULL) return 0;
return entry!=NULL&&entry->id[0]!='\0'&&entry->label[0]!='\0'&&entry->state>=UMI_TEST_WORKBENCH_STATE_IDLE&&entry->state<=UMI_TEST_WORKBENCH_STATE_CANCELLED;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTestWorkbenchEntryArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7dc59c085936eb6a);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestWorkbenchEntry *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestWorkbenchEntry *)0)->label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestWorkbenchEntry *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTestWorkbenchEntryArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTestWorkbenchEntry *)0)->id) - 1U +
        8U + sizeof(((UmiTestWorkbenchEntry *)0)->label) - 1U +
        8U + sizeof(((UmiTestWorkbenchEntry *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiTestWorkbenchEntryArchiveWrite(UmiArchiveWriter *writer, const UmiTestWorkbenchEntry *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->score);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->duration_us);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiTestWorkbenchEntryArchiveRead(UmiArchiveReader *reader, UmiTestWorkbenchEntry *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->state = (UmiTestWorkbenchState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->flags = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->score = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->duration_us = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiTestWorkbenchEntryArchiveValidate(const UmiTestWorkbenchEntry *value)
{
    return umi_test_workbench_entry_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_workbench_entry_archive_encode, umi_test_workbench_entry_archive_decode,
    UmiTestWorkbenchEntry, UmiTestWorkbenchEntryArchiveSchema, UmiTestWorkbenchEntryArchiveBound, UmiTestWorkbenchEntryArchiveWrite, UmiTestWorkbenchEntryArchiveRead, UmiTestWorkbenchEntryArchiveValidate)
