/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/indentation_guides.c
 *
 * PURPOSE:
 *   Calculate semantic indentation-guide depth for whitespace columns.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/indentation_guides.h"

/*
 * Provide the editor wb indentation guides depth operation used by this module and its
 * client applications.
 */
uint32_t umi_editor_wb_indentation_guides_depth(uint32_t cols,uint32_t tabw){/* Apply this branch only when its contract condition is satisfied. */ if(tabw==0U)return 0U;return cols/tabw;}
