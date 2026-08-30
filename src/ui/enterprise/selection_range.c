/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/selection_range.c
 *
 * PURPOSE:
 *   Implement rectangular range normalisation and containment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/selection_range.h"
UmiUiEntSelectionRange umi_ui_ent_selection_range_normalize(UmiUiEntSelectionRange v){size_t t;if(v.row_first>v.row_last){t=v.row_first;v.row_first=v.row_last;v.row_last=t;}if(v.column_first>v.column_last){t=v.column_first;v.column_first=v.column_last;v.column_last=t;}return v;}
int umi_ui_ent_selection_range_contains(UmiUiEntSelectionRange v,size_t r,size_t c){v=umi_ui_ent_selection_range_normalize(v);return r>=v.row_first&&r<=v.row_last&&c>=v.column_first&&c<=v.column_last;}
