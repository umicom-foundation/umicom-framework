/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/breadcrumb_strip.c
 *
 * PURPOSE:
 *   Compose ordered file/symbol breadcrumb segments for an editor surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/breadcrumb_strip.h"

#include <string.h>
/*
 * Initialise editor wb breadcrumb strip from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_breadcrumb_strip_init(UmiEditorWbBreadcrumbStrip *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Add editor wb breadcrumb strip only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_breadcrumb_strip_append(UmiEditorWbBreadcrumbStrip *s,const char *text,uint32_t depth){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||text==NULL||text[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->items[s->count],sizeof s->items[0],text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->depth[s->count]=depth;s->count++;return UMI_STATUS_OK;}
/*
 * Provide the editor wb breadcrumb strip activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_breadcrumb_strip_activate(UmiEditorWbBreadcrumbStrip *s,size_t index){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||index>=s->count)return UMI_STATUS_INVALID_ARGUMENT;s->active_index=index;return UMI_STATUS_OK;}
