/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_workspace_profile.c
 *
 * PURPOSE:
 *   Persist per-workspace debugger layout and presentation preferences.
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
#include "umicom/debug/workbench/debug_workspace_profile.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Provide the view bit operation used by this module and its client applications. */
static uint64_t view_bit(UmiDebugWorkbenchViewKind view)
{ /* Apply this branch only when its contract condition is satisfied. */ if (view < UMI_DEBUG_WORKBENCH_VIEW_BREAKPOINTS || view > UMI_DEBUG_WORKBENCH_VIEW_TIMELINE) return 0U; return UINT64_C(1) << (unsigned int)(view - UMI_DEBUG_WORKBENCH_VIEW_BREAKPOINTS); }
/*
 * Initialise debug workbench debug workspace profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_init(UmiDebugWorkbenchDebugWorkspaceProfile *model, const char *workspace_id)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL || !umi_debug_workbench_id_valid(workspace_id)) return UMI_STATUS_INVALID_ARGUMENT; memset(model, 0, sizeof *model); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (umi_debug_workbench_copy_text(model->workspace_id, sizeof model->workspace_id, workspace_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; model->primary_view = UMI_DEBUG_WORKBENCH_VIEW_VARIABLES; model->follow_instruction_pointer = true; model->revision = 1U; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench debug workspace profile set primary view operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_set_primary_view(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL || view_bit(view) == 0U) return UMI_STATUS_INVALID_ARGUMENT; model->primary_view = view; model->revision++; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench debug workspace profile set view visible operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_set_view_visible(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view, bool visible)
{ uint64_t bit; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT; bit = view_bit(view); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (bit == 0U) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (visible) model->visible_views |= bit; /* Use this fallback path when the earlier condition does not apply. */ else model->visible_views &= ~bit; model->revision++; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench debug workspace profile view visible operation used by this
 * module and its client applications.
 */
int umi_debug_workbench_debug_workspace_profile_view_visible(const UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view)
{ uint64_t bit = view_bit(view); return model != NULL && bit != 0U && (model->visible_views & bit) != 0U; }
/*
 * Check that debug workbench debug workspace profile satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_workspace_profile_valid(const UmiDebugWorkbenchDebugWorkspaceProfile *model)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (model == NULL) return 0;
    if (memchr(model->workspace_id, '\0', sizeof(model->workspace_id)) == NULL) return 0;
 return model != NULL && umi_debug_workbench_id_valid(model->workspace_id) && view_bit(model->primary_view) != 0U && model->revision > 0U; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDebugWorkbenchDebugWorkspaceProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe895dc4613705ff8);
    schema = (schema ^ (uint64_t)sizeof(((UmiDebugWorkbenchDebugWorkspaceProfile *)0)->workspace_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDebugWorkbenchDebugWorkspaceProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDebugWorkbenchDebugWorkspaceProfile *)0)->workspace_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDebugWorkbenchDebugWorkspaceProfileArchiveWrite(UmiArchiveWriter *writer, const UmiDebugWorkbenchDebugWorkspaceProfile *value)
{
    UmiArchiveWriteText(writer, value->workspace_id, sizeof(value->workspace_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->primary_view);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible_views);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->follow_instruction_pointer);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->open_console_on_output);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiDebugWorkbenchDebugWorkspaceProfileArchiveRead(UmiArchiveReader *reader, UmiDebugWorkbenchDebugWorkspaceProfile *value)
{
    UmiArchiveReadText(reader, value->workspace_id, sizeof(value->workspace_id));
    value->primary_view = (UmiDebugWorkbenchViewKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->visible_views = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->follow_instruction_pointer = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->open_console_on_output = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDebugWorkbenchDebugWorkspaceProfileArchiveValidate(const UmiDebugWorkbenchDebugWorkspaceProfile *value)
{
    return umi_debug_workbench_debug_workspace_profile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_debug_workbench_debug_workspace_profile_archive_encode, umi_debug_workbench_debug_workspace_profile_archive_decode,
    UmiDebugWorkbenchDebugWorkspaceProfile, UmiDebugWorkbenchDebugWorkspaceProfileArchiveSchema, UmiDebugWorkbenchDebugWorkspaceProfileArchiveBound, UmiDebugWorkbenchDebugWorkspaceProfileArchiveWrite, UmiDebugWorkbenchDebugWorkspaceProfileArchiveRead, UmiDebugWorkbenchDebugWorkspaceProfileArchiveValidate)
