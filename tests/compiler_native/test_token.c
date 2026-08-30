/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_token.c
 *
 * PURPOSE:
 *   Regression coverage for represent lexical tokens with source spans, keyword classification and deterministic text fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/token.h"
int main(void){ UmiNativeToken t; UmiNativeSourceSpan s={0}; if(umi_nc_token_init(&t,UMI_NC_TOKEN_IDENTIFIER,s,"return")!=UMI_STATUS_OK) return 1; if(t.kind!=UMI_NC_TOKEN_KEYWORD) return 2; if(!umi_nc_token_matches(&t,"return")||t.fingerprint==0U) return 3; return 0; }
