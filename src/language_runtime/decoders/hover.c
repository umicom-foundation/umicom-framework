/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/hover.c
 *
 * PURPOSE:
 *   Decode string or MarkupContent hover payloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/hover.h"
#include <string.h>
UmiStatus umi_language_runtime_decode_hover(const char*j,UmiLanguageRuntimeHoverResult*out){UmiLanguageRuntimeJsonDocument d;int r,c,g;UmiStatus s;if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;c=umi_language_runtime_json_object_get(&d,r,"contents");if(c>=0){if(d.tokens[c].type==UMI_LANGUAGE_RUNTIME_JSON_STRING)s=umi_language_runtime_json_string(&d,c,out->contents,sizeof(out->contents));else if(d.tokens[c].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT){int v=umi_language_runtime_json_object_get(&d,c,"value");if(v>=0)s=umi_language_runtime_json_string(&d,v,out->contents,sizeof(out->contents));}if(s!=UMI_STATUS_OK)return s;}g=umi_language_runtime_json_object_get(&d,r,"range");if(g>=0&&umi_language_runtime_decoder_range(&d,g,&out->range)==UMI_STATUS_OK)out->has_range=1;return UMI_STATUS_OK;}
