/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded catalogue of binding identifiers and revisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_catalogue.h"
#include <string.h>
/* Initialise an empty binding catalogue. */
void umi_ui_reactive_binding_catalogue_init(UmiUiReactiveBindingCatalogue *c){if(c)memset(c,0,sizeof *c);}
/* Add a unique binding id and increment catalogue revision. */
UmiStatus umi_ui_reactive_binding_catalogue_add(UmiUiReactiveBindingCatalogue *c,const char *id){size_t i,n;if(!c||!id||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<c->count;i++)if(strcmp(c->ids[i],id)==0)return UMI_STATUS_ALREADY_EXISTS;if(c->count>=UMI_UI_REACTIVE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;n=strlen(id);if(n>=UMI_UI_REACTIVE_ID_CAPACITY)return UMI_STATUS_INVALID_ARGUMENT;memcpy(c->ids[c->count++],id,n+1U);c->revision++;return UMI_STATUS_OK;}
/* Find a binding id by exact stable identity. */
int umi_ui_reactive_binding_catalogue_contains(const UmiUiReactiveBindingCatalogue *c,const char *id){size_t i;if(!c||!id)return 0;for(i=0;i<c->count;i++)if(strcmp(c->ids[i],id)==0)return 1;return 0;}
