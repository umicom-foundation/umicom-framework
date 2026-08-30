/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/token.h
 *
 * PURPOSE:
 *   Represent lexical tokens with source spans, keyword classification and deterministic text fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_TOKEN_H
#define UMICOM_COMPILER_NATIVE_TOKEN_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeToken { UmiNativeTokenKind kind; UmiNativeSourceSpan span; char text[UMI_NC_TOKEN_TEXT_CAPACITY]; uint64_t fingerprint; } UmiNativeToken;
UmiStatus umi_nc_token_init(UmiNativeToken *token,UmiNativeTokenKind kind,UmiNativeSourceSpan span,const char *text);
bool umi_nc_token_is_keyword_text(const char *text);
bool umi_nc_token_matches(const UmiNativeToken *token,const char *text);
#ifdef __cplusplus
}
#endif
#endif
