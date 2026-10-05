/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/disassembly_viewport_model.c
 *
 * PURPOSE:
 *   Track disassembly viewport range, active instruction and follow-PC state.
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
#include "umicom/debug/workbench/disassembly_viewport_model.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise debug workbench disassembly viewport model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_disassembly_viewport_model_init(UmiDebugWorkbenchDisassemblyViewportModel *model, const char *id, uint64_t start_address, uint64_t extent)
{
    UmiDebugWorkbenchRange range = {{0U, 0U}, {0U, 0U}};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || extent == 0U || UINT64_MAX - start_address < extent - 1U) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_entry_init(&model->value, id, "Debug range", "", NULL, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->start_address = start_address;
    model->extent = extent;
    model->cursor_address = start_address;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench disassembly viewport model contains operation used by this
 * module and its client applications.
 */
int umi_debug_workbench_disassembly_viewport_model_contains(const UmiDebugWorkbenchDisassemblyViewportModel *model, uint64_t address)
{ return model != NULL && address >= model->start_address && address - model->start_address < model->extent; }
/*
 * Provide the debug workbench disassembly viewport model set cursor operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_disassembly_viewport_model_set_cursor(UmiDebugWorkbenchDisassemblyViewportModel *model, uint64_t address)
{ /* Preserve the original failure result so the caller can respond to the correct cause. */ if (!umi_debug_workbench_disassembly_viewport_model_contains(model, address)) return UMI_STATUS_INVALID_ARGUMENT; model->cursor_address = address; model->revision++; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench disassembly viewport model set follow execution operation
 * used by this module and its client applications.
 */
UmiStatus umi_debug_workbench_disassembly_viewport_model_set_follow_execution(UmiDebugWorkbenchDisassemblyViewportModel *model, bool follow)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT; model->follow_execution = follow; model->revision++; return UMI_STATUS_OK; }
/*
 * Check that debug workbench disassembly viewport model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_disassembly_viewport_model_valid(const UmiDebugWorkbenchDisassemblyViewportModel *model)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (model == NULL) return 0;
    if (memchr(model->value.id, '\0', sizeof(model->value.id)) == NULL) return 0;
    if (memchr(model->value.label, '\0', sizeof(model->value.label)) == NULL) return 0;
    if (memchr(model->value.detail, '\0', sizeof(model->value.detail)) == NULL) return 0;
    if (memchr(model->value.location.path, '\0', sizeof(model->value.location.path)) == NULL) return 0;
 return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->extent > 0U && umi_debug_workbench_disassembly_viewport_model_contains(model, model->cursor_address) && model->revision > 0U; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDebugWorkbenchDisassemblyViewportModelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcc861a4cf6f81b4f);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDisassemblyViewportModel *)0)->value.id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDisassemblyViewportModel *)0)->value.label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDisassemblyViewportModel *)0)->value.detail)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDisassemblyViewportModel *)0)->value.location.path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDebugWorkbenchDisassemblyViewportModelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDebugWorkbenchDisassemblyViewportModel *)0)->value.id) - 1U +
        8U + sizeof(((UmiDebugWorkbenchDisassemblyViewportModel *)0)->value.label) - 1U +
        8U + sizeof(((UmiDebugWorkbenchDisassemblyViewportModel *)0)->value.detail) - 1U +
        8U + sizeof(((UmiDebugWorkbenchDisassemblyViewportModel *)0)->value.location.path) - 1U +
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
        8U +
        8U +
        8U;
}
static void UmiDebugWorkbenchDisassemblyViewportModelArchiveWrite(UmiArchiveWriter *writer, const UmiDebugWorkbenchDisassemblyViewportModel *value)
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
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->start_address);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->extent);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cursor_address);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->follow_execution);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiDebugWorkbenchDisassemblyViewportModelArchiveRead(UmiArchiveReader *reader, UmiDebugWorkbenchDisassemblyViewportModel *value)
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
    value->start_address = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->extent = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->cursor_address = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->follow_execution = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDebugWorkbenchDisassemblyViewportModelArchiveValidate(const UmiDebugWorkbenchDisassemblyViewportModel *value)
{
    return umi_debug_workbench_disassembly_viewport_model_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_debug_workbench_disassembly_viewport_model_archive_encode, umi_debug_workbench_disassembly_viewport_model_archive_decode,
    UmiDebugWorkbenchDisassemblyViewportModel, UmiDebugWorkbenchDisassemblyViewportModelArchiveSchema, UmiDebugWorkbenchDisassemblyViewportModelArchiveBound, UmiDebugWorkbenchDisassemblyViewportModelArchiveWrite, UmiDebugWorkbenchDisassemblyViewportModelArchiveRead, UmiDebugWorkbenchDisassemblyViewportModelArchiveValidate)
