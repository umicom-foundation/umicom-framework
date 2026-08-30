/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/token_stream.h
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
#ifndef UMICOM_COMPILER_NATIVE_TOKEN_STREAM_H
#define UMICOM_COMPILER_NATIVE_TOKEN_STREAM_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/token.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeTokenStream { UmiNativeToken items[UMI_NC_MAX_TOKENS]; size_t count; size_t cursor; } UmiNativeTokenStream;
void umi_nc_token_stream_init(UmiNativeTokenStream *stream);
UmiStatus umi_nc_token_stream_push(UmiNativeTokenStream *stream,const UmiNativeToken *token);
const UmiNativeToken *umi_nc_token_stream_peek(const UmiNativeTokenStream *stream,size_t lookahead);
const UmiNativeToken *umi_nc_token_stream_next(UmiNativeTokenStream *stream);
void umi_nc_token_stream_rewind(UmiNativeTokenStream *stream);
#ifdef __cplusplus
}
#endif
#endif
