/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/locations.c
 *
 * PURPOSE:
 *   Decode definition/reference locations and location links.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/locations.h"
#include <string.h>
static UmiStatus one(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeLocation*out){int u=umi_language_runtime_json_object_get(d,t,"uri"),r=umi_language_runtime_json_object_get(d,t,"range");UmiStatus s;if(u>=0&&r>=0)return umi_language_runtime_decoder_location(d,t,out);u=umi_language_runtime_json_object_get(d,t,"targetUri");r=umi_language_runtime_json_object_get(d,t,"targetSelectionRange");if(u<0||r<0)return UMI_STATUS_PARSE_ERROR;memset(out,0,sizeof(*out));s=umi_language_runtime_json_string(d,u,out->uri,sizeof(out->uri));return s==UMI_STATUS_OK?umi_language_runtime_decoder_range(d,r,&out->range):s;}
UmiStatus umi_language_runtime_decode_locations(const char*j,UmiLanguageRuntimeLocationList*out){UmiLanguageRuntimeJsonDocument d;int r;size_t i,n;UmiStatus s;if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;if(d.tokens[r].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT){s=one(&d,r,&out->items[0]);if(s==UMI_STATUS_OK)out->count=1;return s;}if(d.tokens[r].type!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return UMI_STATUS_PARSE_ERROR;n=umi_language_runtime_json_array_count(&d,r);if(n>UMI_LANGUAGE_RUNTIME_LOCATION_CAPACITY)n=UMI_LANGUAGE_RUNTIME_LOCATION_CAPACITY;for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(&d,r,i);if(t>=0&&one(&d,t,&out->items[out->count])==UMI_STATUS_OK)out->count++;}return UMI_STATUS_OK;}
