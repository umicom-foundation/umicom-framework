/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/selection_model.c
 *
 * PURPOSE:
 *   Implement bounded multi-range selection state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/selection_model.h"
#include <string.h>
void umi_ui_ent_selection_model_init(UmiUiEntSelectionModel *m,UmiUiEntSelectionMode mode){if(m){memset(m,0,sizeof *m);m->mode=mode;}}
UmiStatus umi_ui_ent_selection_model_add(UmiUiEntSelectionModel *m,UmiUiEntSelectionRange r){if(!m)return UMI_STATUS_INVALID_ARGUMENT;if(m->mode==UMI_UI_ENT_SELECTION_SINGLE)m->count=0U;if(m->count>=UMI_UI_ENT_MAX_SELECTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;m->ranges[m->count++]=umi_ui_ent_selection_range_normalize(r);m->revision++;return UMI_STATUS_OK;}
void umi_ui_ent_selection_model_clear(UmiUiEntSelectionModel *m){if(m){m->count=0U;m->revision++;}}
int umi_ui_ent_selection_model_contains(const UmiUiEntSelectionModel *m,size_t r,size_t c){size_t i;if(!m)return 0;for(i=0;i<m->count;i++)if(umi_ui_ent_selection_range_contains(m->ranges[i],r,c))return 1;return 0;}
