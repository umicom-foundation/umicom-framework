/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/observable_store.c
 *
 * PURPOSE:
 *   Hold bounded revisioned UI state keyed by canonical property paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/observable_store.h"
#include <string.h>
/* Initialise an empty observable state store. */
void umi_ui_reactive_observable_store_init(UmiUiReactiveObservableStore *s){/* Apply this operation only while the related capability or state is available. */ if(s)memset(s,0,sizeof *s);}
/* Upsert one value by path and advance revisions only when state is accepted. */
UmiStatus umi_ui_reactive_observable_store_put(UmiUiReactiveObservableStore *s,const char *key,const UmiUiValue *value){size_t i,n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!s||!key||!value||key[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<s->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(s->items[i].key,key)==0){s->items[i].value=*value;s->items[i].revision=++s->revision;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->count>=UMI_UI_REACTIVE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;n=strlen(key);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>=UMI_UI_REACTIVE_PATH_CAPACITY)return UMI_STATUS_INVALID_ARGUMENT;memcpy(s->items[s->count].key,key,n+1U);s->items[s->count].value=*value;s->items[s->count].revision=++s->revision;s->count++;return UMI_STATUS_OK;}
/* Read a stored value without transferring ownership. */
UmiStatus umi_ui_reactive_observable_store_get(const UmiUiReactiveObservableStore *s,const char *key,UmiUiValue *out){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!s||!key||!out)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<s->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(s->items[i].key,key)==0){*out=s->items[i].value;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
