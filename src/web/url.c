/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/url.c
 *
 * PURPOSE:
 *   Implement URL parsing and percent decoding.
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

#include "umicom/web/url.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
static int hex_value(char c){if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;return -1;}
UmiStatus umi_web_url_decode(const char *text,char *out_text,size_t capacity){size_t i=0U,o=0U;if(text==NULL||out_text==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;while(text[i]!='\0'){char c=text[i++];if(c=='%'&&text[i]!='\0'&&text[i+1U]!='\0'){int hi=hex_value(text[i]),lo=hex_value(text[i+1U]);if(hi<0||lo<0)return UMI_STATUS_PARSE_ERROR;c=(char)((hi<<4)|lo);i+=2U;}else if(c=='+')c=' ';if(o+1U>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;out_text[o++]=c;}out_text[o]='\0';return UMI_STATUS_OK;}
UmiStatus umi_web_url_parse(const char *text,UmiWebUrl *out_url){const char *scheme_end,*authority,*path,*colon,*query;size_t n;if(text==NULL||out_url==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_url,0,sizeof(*out_url));scheme_end=strstr(text,"://");if(scheme_end==NULL)return UMI_STATUS_PARSE_ERROR;n=(size_t)(scheme_end-text);if(n+1U>sizeof(out_url->scheme))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out_url->scheme,text,n);out_url->scheme[n]='\0';authority=scheme_end+3;path=strchr(authority,'/');if(path==NULL)path=authority+strlen(authority);colon=memchr(authority,':',(size_t)(path-authority));n=(size_t)((colon!=NULL?colon:path)-authority);if(n+1U>sizeof(out_url->host))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out_url->host,authority,n);out_url->host[n]='\0';out_url->port=(uint16_t)(colon!=NULL?strtoul(colon+1,NULL,10):(strcmp(out_url->scheme,"https")==0?443U:80U));if(*path=='\0')return umi_web_copy_text(out_url->path,sizeof(out_url->path),"/");query=strchr(path,'?');if(query==NULL)return umi_web_copy_text(out_url->path,sizeof(out_url->path),path);n=(size_t)(query-path);if(n+1U>sizeof(out_url->path))return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(out_url->path,path,n);out_url->path[n]='\0';return umi_web_copy_text(out_url->query,sizeof(out_url->query),query+1);}
