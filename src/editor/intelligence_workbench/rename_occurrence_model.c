/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/rename_occurrence_model.c
 *
 * PURPOSE:
 *   Model rename occurrence model as toolkit-neutral Framework-owned editor intelligence state.
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
#include "umicom/editor/intelligence_workbench/rename_occurrence_model.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise editor intel rename occurrence model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_rename_occurrence_model_init(UmiEditorIntelRenameOccurrenceModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_entry_init(&model->value,id,label,path,range)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->applicability=UMI_EDITOR_INTEL_APPLICABILITY_AVAILABLE;model->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the editor intel rename occurrence model set score operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_rename_occurrence_model_set_score(UmiEditorIntelRenameOccurrenceModel *model,uint32_t score){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->value.score=score;model->value.revision++;model->revision++;return UMI_STATUS_OK;}
/*
 * Find editor intel rename occurrence model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_intel_rename_occurrence_model_set_selected(UmiEditorIntelRenameOccurrenceModel *model,bool selected){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->selected=selected;model->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor intel rename occurrence model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_rename_occurrence_model_valid(const UmiEditorIntelRenameOccurrenceModel *model){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (model == NULL) return 0;
    if (memchr(model->value.id, '\0', sizeof(model->value.id)) == NULL) return 0;
    if (memchr(model->value.label, '\0', sizeof(model->value.label)) == NULL) return 0;
    if (memchr(model->value.detail, '\0', sizeof(model->value.detail)) == NULL) return 0;
    if (memchr(model->value.location.path, '\0', sizeof(model->value.location.path)) == NULL) return 0;
return model!=NULL&&umi_editor_intel_entry_valid(&model->value)&&model->applicability>=UMI_EDITOR_INTEL_APPLICABILITY_DISABLED&&model->applicability<=UMI_EDITOR_INTEL_APPLICABILITY_PREFERRED;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorIntelRenameOccurrenceModelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x765637fb9b6a1adb);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelRenameOccurrenceModel *)0)->value.id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelRenameOccurrenceModel *)0)->value.label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelRenameOccurrenceModel *)0)->value.detail)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorIntelRenameOccurrenceModel *)0)->value.location.path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorIntelRenameOccurrenceModelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorIntelRenameOccurrenceModel *)0)->value.id) - 1U +
        8U + sizeof(((UmiEditorIntelRenameOccurrenceModel *)0)->value.label) - 1U +
        8U + sizeof(((UmiEditorIntelRenameOccurrenceModel *)0)->value.detail) - 1U +
        8U + sizeof(((UmiEditorIntelRenameOccurrenceModel *)0)->value.location.path) - 1U +
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
static void UmiEditorIntelRenameOccurrenceModelArchiveWrite(UmiArchiveWriter *writer, const UmiEditorIntelRenameOccurrenceModel *value)
{
    UmiArchiveWriteText(writer, value->value.id, sizeof(value->value.id));
    UmiArchiveWriteText(writer, value->value.label, sizeof(value->value.label));
    UmiArchiveWriteText(writer, value->value.detail, sizeof(value->value.detail));
    UmiArchiveWriteText(writer, value->value.location.path, sizeof(value->value.location.path));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.location.range.start.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.location.range.start.column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.location.range.end.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.location.range.end.column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.score);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->value.revision);
    UmiArchiveWriteSigned(writer, (int64_t)value->applicability);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->selected);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiEditorIntelRenameOccurrenceModelArchiveRead(UmiArchiveReader *reader, UmiEditorIntelRenameOccurrenceModel *value)
{
    UmiArchiveReadText(reader, value->value.id, sizeof(value->value.id));
    UmiArchiveReadText(reader, value->value.label, sizeof(value->value.label));
    UmiArchiveReadText(reader, value->value.detail, sizeof(value->value.detail));
    UmiArchiveReadText(reader, value->value.location.path, sizeof(value->value.location.path));
    value->value.location.range.start.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.location.range.start.column = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.location.range.end.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.location.range.end.column = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.score = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.flags = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->value.revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->applicability = (UmiEditorIntelApplicability)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->selected = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiEditorIntelRenameOccurrenceModelArchiveValidate(const UmiEditorIntelRenameOccurrenceModel *value)
{
    return umi_editor_intel_rename_occurrence_model_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_intel_rename_occurrence_model_archive_encode, umi_editor_intel_rename_occurrence_model_archive_decode,
    UmiEditorIntelRenameOccurrenceModel, UmiEditorIntelRenameOccurrenceModelArchiveSchema, UmiEditorIntelRenameOccurrenceModelArchiveBound, UmiEditorIntelRenameOccurrenceModelArchiveWrite, UmiEditorIntelRenameOccurrenceModelArchiveRead, UmiEditorIntelRenameOccurrenceModelArchiveValidate)
