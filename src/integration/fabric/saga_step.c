/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/saga_step.c
 *
 * PURPOSE:
 *   Describe a saga action and its compensation operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/saga_step.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric saga step from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_fabric_saga_step_init(UmiFabricSagaStep *item, const char *step_id, const char *action_operation, const char *compensation_operation, bool compensation_required) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->step_id,sizeof(item->step_id),step_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->action_operation,sizeof(item->action_operation),action_operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->compensation_operation,sizeof(item->compensation_operation),compensation_operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->compensation_required=compensation_required;
    return umi_fabric_saga_step_validate(item);
}
/* Check that fabric saga step satisfies its contract before another service relies on it. */
UmiStatus umi_fabric_saga_step_validate(const UmiFabricSagaStep *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->step_id, '\0', sizeof(item->step_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->action_operation, '\0', sizeof(item->action_operation)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->compensation_operation, '\0', sizeof(item->compensation_operation)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->step_id[0]!='\0' && item->action_operation[0]!='\0' && (!item->compensation_required || item->compensation_operation[0]!='\0'))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricSagaStepArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x966423f78ff6c6ee);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricSagaStep *)0)->step_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricSagaStep *)0)->action_operation)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricSagaStep *)0)->compensation_operation)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricSagaStepArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricSagaStep *)0)->step_id) - 1U +
        8U + sizeof(((UmiFabricSagaStep *)0)->action_operation) - 1U +
        8U + sizeof(((UmiFabricSagaStep *)0)->compensation_operation) - 1U +
        8U;
}
static void UmiFabricSagaStepArchiveWrite(UmiArchiveWriter *writer, const UmiFabricSagaStep *value)
{
    UmiArchiveWriteText(writer, value->step_id, sizeof(value->step_id));
    UmiArchiveWriteText(writer, value->action_operation, sizeof(value->action_operation));
    UmiArchiveWriteText(writer, value->compensation_operation, sizeof(value->compensation_operation));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->compensation_required);
}
static void UmiFabricSagaStepArchiveRead(UmiArchiveReader *reader, UmiFabricSagaStep *value)
{
    UmiArchiveReadText(reader, value->step_id, sizeof(value->step_id));
    UmiArchiveReadText(reader, value->action_operation, sizeof(value->action_operation));
    UmiArchiveReadText(reader, value->compensation_operation, sizeof(value->compensation_operation));
    value->compensation_required = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricSagaStepArchiveValidate(const UmiFabricSagaStep *value)
{
    return umi_fabric_saga_step_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_saga_step_archive_encode, umi_fabric_saga_step_archive_decode,
    UmiFabricSagaStep, UmiFabricSagaStepArchiveSchema, UmiFabricSagaStepArchiveBound, UmiFabricSagaStepArchiveWrite, UmiFabricSagaStepArchiveRead, UmiFabricSagaStepArchiveValidate)
