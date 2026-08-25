/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/types.c
 *
 * PURPOSE:
 *   Define stable constants, enums and utility helpers for the native Umicom compiler pipeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/types.h"
#include <string.h>

UmiStatus umi_nc_copy_text(char *destination,size_t capacity,const char *source) {
    if (destination==NULL || capacity==0U || source==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t n=strlen(source); if (n>=capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination,source,n+1U); return UMI_STATUS_OK;
}
uint64_t umi_nc_hash_bytes(const void *data,size_t size) {
    if (data==NULL && size!=0U) return 0U;
    const unsigned char *p=(const unsigned char*)data; uint64_t h=1469598103934665603ULL;
    for(size_t i=0U;i<size;i++){ h^=(uint64_t)p[i]; h*=1099511628211ULL; } return h;
}
uint64_t umi_nc_hash_text(const char *text){ return text==NULL?0U:umi_nc_hash_bytes(text,strlen(text)); }
const char *umi_nc_token_kind_name(UmiNativeTokenKind kind){ switch(kind){case UMI_NC_TOKEN_EOF:return "eof";case UMI_NC_TOKEN_IDENTIFIER:return "identifier";case UMI_NC_TOKEN_KEYWORD:return "keyword";case UMI_NC_TOKEN_NUMBER:return "number";case UMI_NC_TOKEN_STRING:return "string";case UMI_NC_TOKEN_CHARACTER:return "character";case UMI_NC_TOKEN_PUNCTUATION:return "punctuation";case UMI_NC_TOKEN_COMMENT:return "comment";case UMI_NC_TOKEN_ERROR:return "error";default:return "unknown";} }
const char *umi_nc_stage_name(UmiNativeCompileStage stage){ switch(stage){case UMI_NC_STAGE_SOURCE:return "source";case UMI_NC_STAGE_LEX:return "lex";case UMI_NC_STAGE_PREPROCESS:return "preprocess";case UMI_NC_STAGE_PARSE:return "parse";case UMI_NC_STAGE_SEMANTIC:return "semantic";case UMI_NC_STAGE_IR:return "ir";case UMI_NC_STAGE_OPTIMIZE:return "optimize";case UMI_NC_STAGE_SELECT:return "select";case UMI_NC_STAGE_ASSEMBLY:return "assembly";case UMI_NC_STAGE_OBJECT:return "object";case UMI_NC_STAGE_LINK:return "link";case UMI_NC_STAGE_COMPLETE:return "complete";default:return "unknown";} }
