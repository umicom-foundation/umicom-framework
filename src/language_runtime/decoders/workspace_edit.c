/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/workspace_edit.c
 *
 * PURPOSE:
 *   Decode common WorkspaceEdit.changes maps for rename/refactor preview.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/workspace_edit.h"
#include "umicom/language_runtime/decoders/text_edits.h"
#include <string.h>
#include <stdio.h>
UmiStatus umi_language_runtime_decode_workspace_edit(const char*j,UmiLanguageRuntimeWorkspaceEdit*out){UmiLanguageRuntimeJsonDocument d;int r,c;size_t i,n;UmiStatus s;if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;c=umi_language_runtime_json_object_get(&d,r,"changes");if(c<0)return UMI_STATUS_NOT_IMPLEMENTED;n=umi_language_runtime_json_object_count(&d,c);for(i=0;i<n;i++){int k=-1,v=-1;char uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];UmiLanguageRuntimeTextEditList e;size_t q;s=umi_language_runtime_json_object_entry_at(&d,c,i,&k,&v);if(s!=UMI_STATUS_OK)return s;s=umi_language_runtime_json_string(&d,k,uri,sizeof(uri));if(s!=UMI_STATUS_OK)return s;s=umi_language_runtime_decode_text_edit_array_token(&d,v,&e);if(s!=UMI_STATUS_OK)return s;for(q=0;q<e.count;q++){if(out->count>=UMI_LANGUAGE_RUNTIME_MAX_EDITS)return UMI_STATUS_CAPACITY_EXCEEDED;snprintf(out->items[out->count].uri,sizeof(out->items[out->count].uri),"%s",uri);out->items[out->count].edit=e.edits[q];out->count++;}}return UMI_STATUS_OK;}
