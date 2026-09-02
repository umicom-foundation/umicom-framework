/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_location.c
 *
 * PURPOSE:
 *   Represent a validated file, line and column editor location.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_location.h"

#include <string.h>
/*
 * Initialise editor wb editor location from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_location_init(UmiEditorWbEditorLocation *l,const char *path,uint32_t line,uint32_t col){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||path==NULL||path[0]=='\0'||line==0U||col==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(l,0,sizeof *l);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(l->path,sizeof l->path,path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;l->position.line=line;l->position.column=col;return UMI_STATUS_OK;}
/*
 * Check that editor wb editor location satisfies its contract before another service
 * relies on it.
 */
int umi_editor_wb_editor_location_valid(const UmiEditorWbEditorLocation *l){return l!=NULL&&l->path[0]!='\0'&&l->position.line>0U&&l->position.column>0U;}
