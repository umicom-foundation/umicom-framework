/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/designer_workspace.c
 *
 * PURPOSE:
 *   Represent the complete visual designer workspace selection and dirty state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/designer_workspace.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer workspace from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_designer_workspace_init(UmiRadDesignerWorkspace *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->workspace_id, sizeof item->workspace_id, "designer_workspace");
    (void)umi_rad_copy_text(item->document_id, sizeof item->document_id, "designer_workspace");
    (void)umi_rad_copy_text(item->active_surface_id, sizeof item->active_surface_id, "designer_workspace");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer workspace satisfies its contract before another service relies
 * on it.
 */
int umi_rad_designer_workspace_is_valid(const UmiRadDesignerWorkspace *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->workspace_id, '\0', sizeof(item->workspace_id)) == NULL) return 0;
    if (memchr(item->document_id, '\0', sizeof(item->document_id)) == NULL) return 0;
    if (memchr(item->active_surface_id, '\0', sizeof(item->active_surface_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->workspace_id) && umi_rad_id_valid(item->document_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDesignerWorkspaceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc1abf639267c2db7);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDesignerWorkspace *)0)->workspace_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDesignerWorkspace *)0)->document_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDesignerWorkspace *)0)->active_surface_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadDesignerWorkspaceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadDesignerWorkspace *)0)->workspace_id) - 1U +
        8U + sizeof(((UmiRadDesignerWorkspace *)0)->document_id) - 1U +
        8U + sizeof(((UmiRadDesignerWorkspace *)0)->active_surface_id) - 1U +
        8U +
        8U;
}
static void UmiRadDesignerWorkspaceArchiveWrite(UmiArchiveWriter *writer, const UmiRadDesignerWorkspace *value)
{
    UmiArchiveWriteText(writer, value->workspace_id, sizeof(value->workspace_id));
    UmiArchiveWriteText(writer, value->document_id, sizeof(value->document_id));
    UmiArchiveWriteText(writer, value->active_surface_id, sizeof(value->active_surface_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->selection_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dirty);
}
static void UmiRadDesignerWorkspaceArchiveRead(UmiArchiveReader *reader, UmiRadDesignerWorkspace *value)
{
    UmiArchiveReadText(reader, value->workspace_id, sizeof(value->workspace_id));
    UmiArchiveReadText(reader, value->document_id, sizeof(value->document_id));
    UmiArchiveReadText(reader, value->active_surface_id, sizeof(value->active_surface_id));
    value->selection_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->dirty = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadDesignerWorkspaceArchiveValidate(const UmiRadDesignerWorkspace *value)
{
    return umi_rad_designer_workspace_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_designer_workspace_archive_encode, umi_rad_designer_workspace_archive_decode,
    UmiRadDesignerWorkspace, UmiRadDesignerWorkspaceArchiveSchema, UmiRadDesignerWorkspaceArchiveBound, UmiRadDesignerWorkspaceArchiveWrite, UmiRadDesignerWorkspaceArchiveRead, UmiRadDesignerWorkspaceArchiveValidate)
