/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test/workbench/quality_workbench_snapshot.c
 *
 * PURPOSE:
 *   Implement quality workbench snapshot state transitions and validation.
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
#include "umicom/test/workbench/quality_workbench_snapshot.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise quality workbench snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_quality_workbench_snapshot_init(UmiQualityWorkbenchSnapshot *model,const char *id,const char *label){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_test_workbench_entry_init(&model->value,id,label)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->generation=1U;return UMI_STATUS_OK;}
/*
 * Exercise quality workbench snapshot set active and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_quality_workbench_snapshot_set_active(UmiQualityWorkbenchSnapshot *model,bool active){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->active=active;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
/*
 * Return the number of records represented by quality workbench snapshot set without
 * changing their state.
 */
UmiStatus umi_quality_workbench_snapshot_set_count(UmiQualityWorkbenchSnapshot *model,uint32_t item_count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL||item_count>UMI_TEST_WORKBENCH_MAX_ITEMS)return UMI_STATUS_INVALID_ARGUMENT;model->item_count=item_count;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
/*
 * Exercise quality workbench snapshot set state and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_quality_workbench_snapshot_set_state(UmiQualityWorkbenchSnapshot *model,UmiTestWorkbenchState state){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_test_workbench_entry_set_state(&model->value,state);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)model->generation++;return s;}
/*
 * Check that quality workbench snapshot satisfies its contract before another service
 * relies on it.
 */
int umi_quality_workbench_snapshot_valid(const UmiQualityWorkbenchSnapshot *model){
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
static uint64_t UmiQualityWorkbenchSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x55a856f4829dc0a4);
    schema = (schema ^ (uint64_t)sizeof(((UmiQualityWorkbenchSnapshot *)0)->value.id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiQualityWorkbenchSnapshot *)0)->value.label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiQualityWorkbenchSnapshot *)0)->value.detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiQualityWorkbenchSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiQualityWorkbenchSnapshot *)0)->value.id) - 1U +
        8U + sizeof(((UmiQualityWorkbenchSnapshot *)0)->value.label) - 1U +
        8U + sizeof(((UmiQualityWorkbenchSnapshot *)0)->value.detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiQualityWorkbenchSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiQualityWorkbenchSnapshot *value)
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
static void UmiQualityWorkbenchSnapshotArchiveRead(UmiArchiveReader *reader, UmiQualityWorkbenchSnapshot *value)
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
static UmiStatus UmiQualityWorkbenchSnapshotArchiveValidate(const UmiQualityWorkbenchSnapshot *value)
{
    return umi_quality_workbench_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_quality_workbench_snapshot_archive_encode, umi_quality_workbench_snapshot_archive_decode,
    UmiQualityWorkbenchSnapshot, UmiQualityWorkbenchSnapshotArchiveSchema, UmiQualityWorkbenchSnapshotArchiveBound, UmiQualityWorkbenchSnapshotArchiveWrite, UmiQualityWorkbenchSnapshotArchiveRead, UmiQualityWorkbenchSnapshotArchiveValidate)
