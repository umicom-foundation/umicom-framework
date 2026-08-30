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

uint32_t umi_editor_wb_indentation_guides_depth(uint32_t cols,uint32_t tabw){if(tabw==0U)return 0U;return cols/tabw;}
