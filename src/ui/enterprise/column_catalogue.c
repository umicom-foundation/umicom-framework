/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/column_catalogue.c
 *
 * PURPOSE:
 *   Implement unique ordered column registration and reordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_catalogue.h"
#include <string.h>
void umi_ui_ent_column_catalogue_init(UmiUiEntColumnCatalogue *c){if(c)memset(c,0,sizeof *c);}
const UmiUiEntColumnDescriptor *umi_ui_ent_column_catalogue_find(const UmiUiEntColumnCatalogue *c,const char *id){size_t i;if(!c||!id)return NULL;for(i=0;i<c->count;i++)if(strcmp(c->items[i].column_id,id)==0)return &c->items[i];return NULL;}
UmiStatus umi_ui_ent_column_catalogue_add(UmiUiEntColumnCatalogue *c,const UmiUiEntColumnDescriptor *x){if(!c||!umi_ui_ent_column_descriptor_validate(x))return UMI_STATUS_INVALID_ARGUMENT;if(c->count>=UMI_UI_ENT_MAX_COLUMNS)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_ui_ent_column_catalogue_find(c,x->column_id))return UMI_STATUS_ALREADY_EXISTS;c->items[c->count++]=*x;c->revision++;return UMI_STATUS_OK;}
UmiStatus umi_ui_ent_column_catalogue_move(UmiUiEntColumnCatalogue *c,size_t from,size_t to){UmiUiEntColumnDescriptor t;if(!c||from>=c->count||to>=c->count)return UMI_STATUS_INVALID_ARGUMENT;if(from==to)return UMI_STATUS_OK;t=c->items[from];if(from<to)memmove(&c->items[from],&c->items[from+1U],(to-from)*sizeof c->items[0]);else memmove(&c->items[to+1U],&c->items[to],(from-to)*sizeof c->items[0]);c->items[to]=t;c->revision++;return UMI_STATUS_OK;}
