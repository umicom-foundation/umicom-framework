/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/json.c
 *
 * PURPOSE:
 *   Implement small JSON string escaping and object helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/json.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_web_json_escape(const char *text,char *out_text,size_t capacity){size_t i=0U,o=0U;if(text==NULL||out_text==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;while(text[i]!='\0'){char c=text[i++];const char *esc=NULL;if(c=='"')esc="\\\"";else if(c=='\\')esc="\\\\";else if(c=='\n')esc="\\n";else if(c=='\r')esc="\\r";else if(c=='\t')esc="\\t";if(esc!=NULL){size_t n=strlen(esc);if(o+n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out_text+o,esc,n);o+=n;}else{if(o+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;out_text[o++]=c;}}out_text[o]='\0';return UMI_STATUS_OK;}
UmiStatus umi_web_json_object1(const char *name,const char *value,char *out_text,size_t capacity){char n[512],v[2048];int count;UmiStatus s=umi_web_json_escape(name,n,sizeof(n));if(s!=UMI_STATUS_OK)return s;s=umi_web_json_escape(value,v,sizeof(v));if(s!=UMI_STATUS_OK)return s;count=snprintf(out_text,capacity,"{\"%s\":\"%s\"}",n,v);return count<0||(size_t)count>=capacity?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
