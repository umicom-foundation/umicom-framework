/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/column_catalogue.c
 *
 * PURPOSE:
 *   Implement unique ordered column registration and reordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_catalogue.h"
#include <string.h>
/*
 * Initialise ui ent column catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_column_catalogue_init(UmiUiEntColumnCatalogue *c){/* Apply this branch only when its contract condition is satisfied. */ if(c)memset(c,0,sizeof *c);}
/*
 * Find ui ent column catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiUiEntColumnDescriptor *umi_ui_ent_column_catalogue_find(const UmiUiEntColumnCatalogue *c,const char *id){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!c||!id)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<c->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(c->items[i].column_id,id)==0)return &c->items[i];return NULL;}
/*
 * Add ui ent column catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_column_catalogue_add(UmiUiEntColumnCatalogue *c,const UmiUiEntColumnDescriptor *x){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!c||!umi_ui_ent_column_descriptor_validate(x))return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->count>=UMI_UI_ENT_MAX_COLUMNS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_ui_ent_column_catalogue_find(c,x->column_id))return UMI_STATUS_ALREADY_EXISTS;c->items[c->count++]=*x;c->revision++;return UMI_STATUS_OK;}
/*
 * Provide the ui ent column catalogue move operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_column_catalogue_move(UmiUiEntColumnCatalogue *c,size_t from,size_t to){UmiUiEntColumnDescriptor t;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!c||from>=c->count||to>=c->count)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(from==to)return UMI_STATUS_OK;t=c->items[from];/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(from<to)memmove(&c->items[from],&c->items[from+1U],(to-from)*sizeof c->items[0]);/* Use this fallback path when the earlier condition does not apply. */ else memmove(&c->items[to+1U],&c->items[to],(from-to)*sizeof c->items[0]);c->items[to]=t;c->revision++;return UMI_STATUS_OK;}
