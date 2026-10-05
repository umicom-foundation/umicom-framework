/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/workflow_step.c
 *
 * PURPOSE:
 *   Describe an orchestrated integration step with timeout and compensation metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_step.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric workflow step from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_workflow_step_init(UmiFabricWorkflowStep *item, const char *step_id, const char *operation_id, uint64_t timeout_ms, bool optional, bool compensatable) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->step_id,sizeof(item->step_id),step_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->operation_id,sizeof(item->operation_id),operation_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->timeout_ms=timeout_ms;item->optional=optional;item->compensatable=compensatable;
    return umi_fabric_workflow_step_validate(item);
}
/*
 * Check that fabric workflow step satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_workflow_step_validate(const UmiFabricWorkflowStep *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->step_id, '\0', sizeof(item->step_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->operation_id, '\0', sizeof(item->operation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->step_id[0]!='\0' && item->operation_id[0]!='\0' && item->timeout_ms>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricWorkflowStepArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x53ce3cd5e368daf5);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricWorkflowStep *)0)->step_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricWorkflowStep *)0)->operation_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricWorkflowStepArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricWorkflowStep *)0)->step_id) - 1U +
        8U + sizeof(((UmiFabricWorkflowStep *)0)->operation_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiFabricWorkflowStepArchiveWrite(UmiArchiveWriter *writer, const UmiFabricWorkflowStep *value)
{
    UmiArchiveWriteText(writer, value->step_id, sizeof(value->step_id));
    UmiArchiveWriteText(writer, value->operation_id, sizeof(value->operation_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timeout_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->optional);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->compensatable);
}
static void UmiFabricWorkflowStepArchiveRead(UmiArchiveReader *reader, UmiFabricWorkflowStep *value)
{
    UmiArchiveReadText(reader, value->step_id, sizeof(value->step_id));
    UmiArchiveReadText(reader, value->operation_id, sizeof(value->operation_id));
    value->timeout_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->optional = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->compensatable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricWorkflowStepArchiveValidate(const UmiFabricWorkflowStep *value)
{
    return umi_fabric_workflow_step_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_workflow_step_archive_encode, umi_fabric_workflow_step_archive_decode,
    UmiFabricWorkflowStep, UmiFabricWorkflowStepArchiveSchema, UmiFabricWorkflowStepArchiveBound, UmiFabricWorkflowStepArchiveWrite, UmiFabricWorkflowStepArchiveRead, UmiFabricWorkflowStepArchiveValidate)
