/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_conformance.c
 *
 * PURPOSE:
 *   Evaluate editor production-readiness requirements across frontend renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_conformance.h"

/*
 * Return the number of records represented by editor wb editor conformance pass without
 * changing their state.
 */
size_t umi_editor_wb_editor_conformance_pass_count(const UmiEditorWbEditorConformance *s){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return 0U;n+=s->split_groups?1U:0U;n+=s->navigation?1U:0U;n+=s->search?1U:0U;n+=s->multi_cursor?1U:0U;n+=s->accessibility?1U:0U;n+=s->session_restore?1U:0U;return n;}
/*
 * Provide the editor wb editor conformance ready operation used by this module and its
 * client applications.
 */
int umi_editor_wb_editor_conformance_ready(const UmiEditorWbEditorConformance *s){return umi_editor_wb_editor_conformance_pass_count(s)==6U;}
