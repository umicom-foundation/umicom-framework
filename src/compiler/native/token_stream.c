/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/token_stream.c
 *
 * PURPOSE:
 *   Provide a bounded lexical token stream with push, peek, consume and rewind operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/token_stream.h"
#include <string.h>
void umi_nc_token_stream_init(UmiNativeTokenStream *stream){ if(stream!=NULL) memset(stream,0,sizeof(*stream)); }
UmiStatus umi_nc_token_stream_push(UmiNativeTokenStream *stream,const UmiNativeToken *token){ if(stream==NULL||token==NULL) return UMI_STATUS_INVALID_ARGUMENT; if(stream->count>=UMI_NC_MAX_TOKENS) return UMI_STATUS_CAPACITY_EXCEEDED; stream->items[stream->count++]=*token; return UMI_STATUS_OK; }
const UmiNativeToken *umi_nc_token_stream_peek(const UmiNativeTokenStream *stream,size_t lookahead){ if(stream==NULL||stream->cursor+lookahead>=stream->count) return NULL; return &stream->items[stream->cursor+lookahead]; }
const UmiNativeToken *umi_nc_token_stream_next(UmiNativeTokenStream *stream){ const UmiNativeToken *t=umi_nc_token_stream_peek(stream,0U); if(t!=NULL) stream->cursor++; return t; }
void umi_nc_token_stream_rewind(UmiNativeTokenStream *stream){ if(stream!=NULL) stream->cursor=0U; }
