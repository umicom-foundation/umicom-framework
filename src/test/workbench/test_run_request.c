/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test/workbench/test_run_request.c
 *
 * PURPOSE:
 *   Implement test run request state transitions and validation.
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
#include "umicom/test/workbench/test_run_request.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise test run request from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_run_request_init(UmiTestRunRequest *model,const char *id,const char *label){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_test_workbench_entry_init(&model->value,id,label)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->generation=1U;return UMI_STATUS_OK;}
/*
 * Exercise test run request set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_request_set_active(UmiTestRunRequest *model,bool active){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->active=active;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
/*
 * Return the number of records represented by test run request set without changing their
 * state.
 */
UmiStatus umi_test_run_request_set_count(UmiTestRunRequest *model,uint32_t item_count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL||item_count>UMI_TEST_WORKBENCH_MAX_ITEMS)return UMI_STATUS_INVALID_ARGUMENT;model->item_count=item_count;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
/*
 * Exercise test run request set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_run_request_set_state(UmiTestRunRequest *model,UmiTestWorkbenchState state){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_test_workbench_entry_set_state(&model->value,state);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)model->generation++;return s;}
/* Check that test run request satisfies its contract before another service relies on it. */
int umi_test_run_request_valid(const UmiTestRunRequest *model){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (model == NULL) return 0;
    if (memchr(model->value.id, '\0', sizeof(model->value.id)) == NULL) return 0;
    if (memchr(model->value.label, '\0', sizeof(model->value.label)) == NULL) return 0;
    if (memchr(model->value.detail, '\0', sizeof(model->value.detail)) == NULL) return 0;
return model!=NULL&&umi_test_workbench_entry_valid(&model->value)&&model->item_count<=UMI_TEST_WORKBENCH_MAX_ITEMS&&model->generation>0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTestRunRequestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe34a0b3a5e4d1a6d);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRunRequest *)0)->value.id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRunRequest *)0)->value.label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRunRequest *)0)->value.detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTestRunRequestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTestRunRequest *)0)->value.id) - 1U +
        8U + sizeof(((UmiTestRunRequest *)0)->value.label) - 1U +
        8U + sizeof(((UmiTestRunRequest *)0)->value.detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiTestRunRequestArchiveWrite(UmiArchiveWriter *writer, const UmiTestRunRequest *value)
{
    UmiArchiveWriteText(writer, value->value.id, sizeof(value->value.id));
    UmiArchiveWriteText(writer, value->value.label, sizeof(value->value.label));
    UmiArchiveWriteText(writer, value->value.detail, sizeof(value->value.detail));
    UmiArchiveWriteSigned(writer, (int64_t)value->value.state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.score);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.duration_us);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->generation);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiTestRunRequestArchiveRead(UmiArchiveReader *reader, UmiTestRunRequest *value)
{
    UmiArchiveReadText(reader, value->value.id, sizeof(value->value.id));
    UmiArchiveReadText(reader, value->value.label, sizeof(value->value.label));
    UmiArchiveReadText(reader, value->value.detail, sizeof(value->value.detail));
    value->value.state = (UmiTestWorkbenchState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->value.flags = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.score = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.duration_us = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->value.revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTestRunRequestArchiveValidate(const UmiTestRunRequest *value)
{
    return umi_test_run_request_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_run_request_archive_encode, umi_test_run_request_archive_decode,
    UmiTestRunRequest, UmiTestRunRequestArchiveSchema, UmiTestRunRequestArchiveBound, UmiTestRunRequestArchiveWrite, UmiTestRunRequestArchiveRead, UmiTestRunRequestArchiveValidate)
