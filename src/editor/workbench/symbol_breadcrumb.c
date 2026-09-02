/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/symbol_breadcrumb.c
 *
 * PURPOSE:
 *   Represent a semantic symbol path distinct from the filesystem breadcrumb.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/symbol_breadcrumb.h"

#include <string.h>
/*
 * Initialise editor wb symbol breadcrumb from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_symbol_breadcrumb_init(UmiEditorWbSymbolBreadcrumb *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Add editor wb symbol breadcrumb only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_symbol_breadcrumb_append(UmiEditorWbSymbolBreadcrumb *s,const char *text,uint32_t depth){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||text==NULL||text[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->items[s->count],sizeof s->items[0],text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->depth[s->count]=depth;s->count++;return UMI_STATUS_OK;}
/*
 * Provide the editor wb symbol breadcrumb activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_symbol_breadcrumb_activate(UmiEditorWbSymbolBreadcrumb *s,size_t index){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||index>=s->count)return UMI_STATUS_INVALID_ARGUMENT;s->active_index=index;return UMI_STATUS_OK;}
