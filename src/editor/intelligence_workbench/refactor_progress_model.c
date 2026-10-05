/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/refactor_progress_model.c
 *
 * PURPOSE:
 *   Model refactor progress model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/refactor_progress_model.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Provide the editor intel refactor progress model begin operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_progress_model_begin(UmiEditorIntelRefactorProgressModel *session,const char *session_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL||!umi_editor_intel_id_valid(session_id))return UMI_STATUS_INVALID_ARGUMENT;memset(session,0,sizeof *session);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_copy_text(session->session_id,sizeof session->session_id,session_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;session->phase=UMI_EDITOR_INTEL_PHASE_PREPARING;session->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the editor intel refactor progress model set ready operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_progress_model_set_ready(UmiEditorIntelRefactorProgressModel *session,uint32_t item_count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL||session->phase!=UMI_EDITOR_INTEL_PHASE_PREPARING)return UMI_STATUS_INVALID_STATE;session->item_count=item_count;session->phase=UMI_EDITOR_INTEL_PHASE_READY;session->changed=true;session->revision++;return UMI_STATUS_OK;}
/*
 * Provide the editor intel refactor progress model cancel operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_progress_model_cancel(UmiEditorIntelRefactorProgressModel *session){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL||session->phase==UMI_EDITOR_INTEL_PHASE_COMMITTED)return UMI_STATUS_INVALID_STATE;session->phase=UMI_EDITOR_INTEL_PHASE_CANCELLED;session->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor intel refactor progress model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_progress_model_valid(const UmiEditorIntelRefactorProgressModel *session){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (session == NULL) return 0;
    if (memchr(session->session_id, '\0', sizeof(session->session_id)) == NULL) return 0;
return session!=NULL&&umi_editor_intel_id_valid(session->session_id)&&session->phase>=UMI_EDITOR_INTEL_PHASE_PREPARING&&session->phase<=UMI_EDITOR_INTEL_PHASE_CANCELLED;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorIntelRefactorProgressModelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6e5d6a3d7f462add);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelRefactorProgressModel *)0)->session_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorIntelRefactorProgressModelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorIntelRefactorProgressModel *)0)->session_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiEditorIntelRefactorProgressModelArchiveWrite(UmiArchiveWriter *writer, const UmiEditorIntelRefactorProgressModel *value)
{
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->phase);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->changed);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiEditorIntelRefactorProgressModelArchiveRead(UmiArchiveReader *reader, UmiEditorIntelRefactorProgressModel *value)
{
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    value->phase = (UmiEditorIntelPhase)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->item_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->changed = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiEditorIntelRefactorProgressModelArchiveValidate(const UmiEditorIntelRefactorProgressModel *value)
{
    return umi_editor_intel_refactor_progress_model_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_intel_refactor_progress_model_archive_encode, umi_editor_intel_refactor_progress_model_archive_decode,
    UmiEditorIntelRefactorProgressModel, UmiEditorIntelRefactorProgressModelArchiveSchema, UmiEditorIntelRefactorProgressModelArchiveBound, UmiEditorIntelRefactorProgressModelArchiveWrite, UmiEditorIntelRefactorProgressModelArchiveRead, UmiEditorIntelRefactorProgressModelArchiveValidate)
