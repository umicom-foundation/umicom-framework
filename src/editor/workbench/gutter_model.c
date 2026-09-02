/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/gutter_model.c
 *
 * PURPOSE:
 *   Describe reusable editor gutter channels and their visibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/gutter_model.h"

/*
 * Initialise editor wb gutter model from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_gutter_model_init(UmiEditorWbGutterModel *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){s->line_numbers=true;s->folding=true;s->diagnostics=true;s->bookmarks=true;s->width=48U;}}
/*
 * Return the number of records represented by editor wb gutter model channel without
 * changing their state.
 */
uint32_t umi_editor_wb_gutter_model_channel_count(const UmiEditorWbGutterModel *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return 0U;return (uint32_t)((s->line_numbers?1:0)+(s->folding?1:0)+(s->diagnostics?1:0)+(s->bookmarks?1:0));}
