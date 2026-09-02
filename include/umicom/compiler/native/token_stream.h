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
/**
 * Represent the native token stream data shared with callers of this public contract.
 */
typedef struct UmiNativeTokenStream { UmiNativeToken items[UMI_NC_MAX_TOKENS]; size_t count; size_t cursor; } UmiNativeTokenStream;
/**
 * Initialise nc token stream from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_token_stream_init(UmiNativeTokenStream *stream);
/**
 * Provide the nc token stream push operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_token_stream_push(UmiNativeTokenStream *stream,const UmiNativeToken *token);
/**
 * Provide the nc token stream peek operation used by this module and its client
 * applications.
 */
const UmiNativeToken *umi_nc_token_stream_peek(const UmiNativeTokenStream *stream,size_t lookahead);
/**
 * Provide the nc token stream next operation used by this module and its client
 * applications.
 */
const UmiNativeToken *umi_nc_token_stream_next(UmiNativeTokenStream *stream);
/**
 * Provide the nc token stream rewind operation used by this module and its client
 * applications.
 */
void umi_nc_token_stream_rewind(UmiNativeTokenStream *stream);
#ifdef __cplusplus
}
#endif
#endif
