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
UmiStatus umi_editor_wb_editor_location_init(UmiEditorWbEditorLocation *l,const char *path,uint32_t line,uint32_t col){if(l==NULL||path==NULL||path[0]=='\0'||line==0U||col==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(l,0,sizeof *l);if(umi_editor_wb_copy_text(l->path,sizeof l->path,path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;l->position.line=line;l->position.column=col;return UMI_STATUS_OK;}
int umi_editor_wb_editor_location_valid(const UmiEditorWbEditorLocation *l){return l!=NULL&&l->path[0]!='\0'&&l->position.line>0U&&l->position.column>0U;}
