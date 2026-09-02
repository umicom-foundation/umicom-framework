/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/selection_model.c
 *
 * PURPOSE:
 *   Implement bounded multi-range selection state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/selection_model.h"
#include <string.h>
/*
 * Initialise ui ent selection model from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_selection_model_init(UmiUiEntSelectionModel *m,UmiUiEntSelectionMode mode){/* Apply this branch only when its contract condition is satisfied. */ if(m){memset(m,0,sizeof *m);m->mode=mode;}}
/*
 * Add ui ent selection model only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_selection_model_add(UmiUiEntSelectionModel *m,UmiUiEntSelectionRange r){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!m)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m->mode==UMI_UI_ENT_SELECTION_SINGLE)m->count=0U;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m->count>=UMI_UI_ENT_MAX_SELECTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;m->ranges[m->count++]=umi_ui_ent_selection_range_normalize(r);m->revision++;return UMI_STATUS_OK;}
/*
 * Release or reset state held by ui ent selection model so the same storage can be reused
 * safely.
 */
void umi_ui_ent_selection_model_clear(UmiUiEntSelectionModel *m){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m){m->count=0U;m->revision++;}}
/*
 * Provide the ui ent selection model contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_selection_model_contains(const UmiUiEntSelectionModel *m,size_t r,size_t c){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!m)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<m->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_ui_ent_selection_range_contains(m->ranges[i],r,c))return 1;return 0;}
