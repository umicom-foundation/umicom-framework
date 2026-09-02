/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_lexer.c
 *
 * PURPOSE:
 *   Regression coverage for tokenize a practical c23 source subset with comments, literals, identifiers, keywords, numbers and punctuation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/lexer.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiNativeSourceBuffer b; umi_nc_source_buffer_init(&b); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_source_buffer_set(&b,"a.c","int main(void){ // hi\n return 42; }")!=UMI_STATUS_OK) return 1; UmiNativeTokenStream s; UmiNativeLexerReport r; if(umi_nc_lexer_run(&b,&s,&r)!=UMI_STATUS_OK) return 2; if(r.tokens<10U||r.comments!=1U||r.errors!=0U) return 3; if(s.items[0].kind!=UMI_NC_TOKEN_KEYWORD) return 4; return 0; }
