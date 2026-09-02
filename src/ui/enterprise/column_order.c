/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/column_order.c
 *
 * PURPOSE:
 *   Implement stable semantic column ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_order.h"
#include <string.h>
/*
 * Initialise ui ent column order from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_column_order_init(UmiUiEntColumnOrder *o){/* Apply this branch only when its contract condition is satisfied. */ if(o)memset(o,0,sizeof *o);}
/* Add ui ent column order only after its inputs and available capacity have been checked. */
UmiStatus umi_ui_ent_column_order_append(UmiUiEntColumnOrder *o,const char *id){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!o||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(o->count>=UMI_UI_ENT_MAX_COLUMNS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<o->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(o->ids[i],id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_ui_ent_copy_text(o->ids[o->count],UMI_UI_ENT_ID_CAPACITY,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;o->count++;return UMI_STATUS_OK;}
/*
 * Provide the ui ent column order move operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_column_order_move(UmiUiEntColumnOrder *o,size_t from,size_t to){char tmp[UMI_UI_ENT_ID_CAPACITY];/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!o||from>=o->count||to>=o->count)return UMI_STATUS_INVALID_ARGUMENT;memcpy(tmp,o->ids[from],sizeof tmp);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(from<to)memmove(o->ids[from],o->ids[from+1U],(to-from)*sizeof o->ids[0]);else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(from>to)memmove(o->ids[to+1U],o->ids[to],(from-to)*sizeof o->ids[0]);memcpy(o->ids[to],tmp,sizeof tmp);return UMI_STATUS_OK;}
