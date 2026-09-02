/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/refactor_text_hunk.c
 *
 * PURPOSE:
 *   Model refactor text hunk as toolkit-neutral Framework-owned editor intelligence state.
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
#include "umicom/editor/intelligence_workbench/refactor_text_hunk.h"

#include <string.h>

/*
 * Initialise editor intel refactor text hunk from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_refactor_text_hunk_init(UmiEditorIntelRefactorTextHunk *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_entry_init(&model->value,id,label,path,range)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->applicability=UMI_EDITOR_INTEL_APPLICABILITY_AVAILABLE;model->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the editor intel refactor text hunk set score operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_text_hunk_set_score(UmiEditorIntelRefactorTextHunk *model,uint32_t score){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->value.score=score;model->value.revision++;model->revision++;return UMI_STATUS_OK;}
/*
 * Find editor intel refactor text hunk set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_intel_refactor_text_hunk_set_selected(UmiEditorIntelRefactorTextHunk *model,bool selected){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->selected=selected;model->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor intel refactor text hunk satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_refactor_text_hunk_valid(const UmiEditorIntelRefactorTextHunk *model){return model!=NULL&&umi_editor_intel_entry_valid(&model->value)&&model->applicability>=UMI_EDITOR_INTEL_APPLICABILITY_DISABLED&&model->applicability<=UMI_EDITOR_INTEL_APPLICABILITY_PREFERRED;}
