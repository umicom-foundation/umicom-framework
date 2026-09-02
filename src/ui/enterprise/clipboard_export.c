/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/clipboard_export.c
 *
 * PURPOSE:
 *   Implement bounded TSV export construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/clipboard_export.h"
#include <string.h>
/*
 * Initialise ui ent clipboard export from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_clipboard_export_init(UmiUiEntClipboardExport *e,char *b,size_t c){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!e||!b||c==0U)return UMI_STATUS_INVALID_ARGUMENT;e->buffer=b;e->capacity=c;e->length=0U;b[0]='\0';return UMI_STATUS_OK;}
/*
 * Add ui ent clipboard export only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_clipboard_export_append(UmiUiEntClipboardExport *e,const char *t,int end){size_t n;char sep=end?'\n':'\t';/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!e||!t)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(t);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n+2U>e->capacity-e->length)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(e->buffer+e->length,t,n);e->length+=n;e->buffer[e->length++]=sep;e->buffer[e->length]='\0';return UMI_STATUS_OK;}
