/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/exception_breakpoint_model.c
 *
 * PURPOSE:
 *   Represent exception-filter breakpoint selection and presentation state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/exception_breakpoint_model.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise debug workbench exception breakpoint model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_exception_breakpoint_model_init(UmiDebugWorkbenchExceptionBreakpointModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_entry_init(&model->value, id, label, detail, path, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = true;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench exception breakpoint model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_exception_breakpoint_model_set_state(UmiDebugWorkbenchExceptionBreakpointModel *model, uint32_t state, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->value.state = state;
    model->value.value = value;
    model->value.revision++;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Find debug workbench exception breakpoint model set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_debug_workbench_exception_breakpoint_model_set_selected(UmiDebugWorkbenchExceptionBreakpointModel *model, bool selected)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->selected = selected;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench exception breakpoint model set enabled operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_exception_breakpoint_model_set_enabled(UmiDebugWorkbenchExceptionBreakpointModel *model, bool enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = enabled;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench exception breakpoint model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_exception_breakpoint_model_valid(const UmiDebugWorkbenchExceptionBreakpointModel *model)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (model == NULL) return 0;
    if (memchr(model->value.id, '\0', sizeof(model->value.id)) == NULL) return 0;
    if (memchr(model->value.label, '\0', sizeof(model->value.label)) == NULL) return 0;
    if (memchr(model->value.detail, '\0', sizeof(model->value.detail)) == NULL) return 0;
    if (memchr(model->value.location.path, '\0', sizeof(model->value.location.path)) == NULL) return 0;

    return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->revision > 0U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDebugWorkbenchExceptionBreakpointModelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5aef764f2652638f);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchExceptionBreakpointModel *)0)->value.id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchExceptionBreakpointModel *)0)->value.label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchExceptionBreakpointModel *)0)->value.detail)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchExceptionBreakpointModel *)0)->value.location.path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDebugWorkbenchExceptionBreakpointModelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDebugWorkbenchExceptionBreakpointModel *)0)->value.id) - 1U +
        8U + sizeof(((UmiDebugWorkbenchExceptionBreakpointModel *)0)->value.label) - 1U +
        8U + sizeof(((UmiDebugWorkbenchExceptionBreakpointModel *)0)->value.detail) - 1U +
        8U + sizeof(((UmiDebugWorkbenchExceptionBreakpointModel *)0)->value.location.path) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDebugWorkbenchExceptionBreakpointModelArchiveWrite(UmiArchiveWriter *writer, const UmiDebugWorkbenchExceptionBreakpointModel *value)
{
    UmiArchiveWriteText(writer, value->value.id, sizeof(value->value.id));
    UmiArchiveWriteText(writer, value->value.label, sizeof(value->value.label));
    UmiArchiveWriteText(writer, value->value.detail, sizeof(value->value.detail));
    UmiArchiveWriteText(writer, value->value.location.path, sizeof(value->value.location.path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.location.range.start.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.location.range.start.column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.location.range.end.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.location.range.end.column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.value);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->selected);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiDebugWorkbenchExceptionBreakpointModelArchiveRead(UmiArchiveReader *reader, UmiDebugWorkbenchExceptionBreakpointModel *value)
{
    UmiArchiveReadText(reader, value->value.id, sizeof(value->value.id));
    UmiArchiveReadText(reader, value->value.label, sizeof(value->value.label));
    UmiArchiveReadText(reader, value->value.detail, sizeof(value->value.detail));
    UmiArchiveReadText(reader, value->value.location.path, sizeof(value->value.location.path));
    value->value.location.range.start.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.location.range.start.column = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.location.range.end.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.location.range.end.column = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.state = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.flags = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.value = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->value.revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->selected = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDebugWorkbenchExceptionBreakpointModelArchiveValidate(const UmiDebugWorkbenchExceptionBreakpointModel *value)
{
    return umi_debug_workbench_exception_breakpoint_model_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_debug_workbench_exception_breakpoint_model_archive_encode, umi_debug_workbench_exception_breakpoint_model_archive_decode,
    UmiDebugWorkbenchExceptionBreakpointModel, UmiDebugWorkbenchExceptionBreakpointModelArchiveSchema, UmiDebugWorkbenchExceptionBreakpointModelArchiveBound, UmiDebugWorkbenchExceptionBreakpointModelArchiveWrite, UmiDebugWorkbenchExceptionBreakpointModelArchiveRead, UmiDebugWorkbenchExceptionBreakpointModelArchiveValidate)
