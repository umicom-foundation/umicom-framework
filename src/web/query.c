/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/query.c
 *
 * PURPOSE:
 *   Implement deterministic query-string parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/query.h"
#include "umicom/web/url.h"
#include <string.h>
UmiStatus umi_web_query_parse(const char *text,UmiWebQuery *out_query){const char *p,*amp,*eq;char key[128],value[512];size_t kn,vn;UmiStatus s;if(text==NULL||out_query==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_query,0,sizeof(*out_query));p=text;while(*p!='\0'){if(out_query->count>=UMI_WEB_MAX_QUERY_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;amp=strchr(p,'&');if(amp==NULL)amp=p+strlen(p);eq=memchr(p,'=',(size_t)(amp-p));kn=(size_t)((eq!=NULL?eq:amp)-p);vn=eq!=NULL?(size_t)(amp-eq-1):0U;if(kn+1U>sizeof(key)||vn+1U>sizeof(value))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(key,p,kn);key[kn]='\0';if(eq!=NULL)(void)memcpy(value,eq+1,vn);value[vn]='\0';s=umi_web_url_decode(key,out_query->items[out_query->count].name,sizeof(out_query->items[out_query->count].name));if(s!=UMI_STATUS_OK)return s;s=umi_web_url_decode(value,out_query->items[out_query->count].value,sizeof(out_query->items[out_query->count].value));if(s!=UMI_STATUS_OK)return s;++out_query->count;p=*amp=='&'?amp+1:amp;}return UMI_STATUS_OK;}
const char *umi_web_query_get(const UmiWebQuery *query,const char *name){size_t i;if(query==NULL||name==NULL)return NULL;for(i=0U;i<query->count;++i)if(strcmp(query->items[i].name,name)==0)return query->items[i].value;return NULL;}
