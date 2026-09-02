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
/*
 * Provide the ui ent selection range normalize operation used by this module and its
 * client applications.
 */
UmiUiEntSelectionRange umi_ui_ent_selection_range_normalize(UmiUiEntSelectionRange v){size_t t;/* Apply this branch only when its contract condition is satisfied. */ if(v.row_first>v.row_last){t=v.row_first;v.row_first=v.row_last;v.row_last=t;}/* Apply this branch only when its contract condition is satisfied. */ if(v.column_first>v.column_last){t=v.column_first;v.column_first=v.column_last;v.column_last=t;}return v;}
/*
 * Provide the ui ent selection range contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_selection_range_contains(UmiUiEntSelectionRange v,size_t r,size_t c){v=umi_ui_ent_selection_range_normalize(v);return r>=v.row_first&&r<=v.row_last&&c>=v.column_first&&c<=v.column_last;}
