/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/clipboard_export.c
 *
 * PURPOSE:
 *   Implement bounded TSV export construction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/clipboard_export.h"
#include <string.h>
UmiStatus umi_ui_ent_clipboard_export_init(UmiUiEntClipboardExport *e,char *b,size_t c){if(!e||!b||c==0U)return UMI_STATUS_INVALID_ARGUMENT;e->buffer=b;e->capacity=c;e->length=0U;b[0]='\0';return UMI_STATUS_OK;}
UmiStatus umi_ui_ent_clipboard_export_append(UmiUiEntClipboardExport *e,const char *t,int end){size_t n;char sep=end?'\n':'\t';if(!e||!t)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(t);if(n+2U>e->capacity-e->length)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(e->buffer+e->length,t,n);e->length+=n;e->buffer[e->length++]=sep;e->buffer[e->length]='\0';return UMI_STATUS_OK;}
