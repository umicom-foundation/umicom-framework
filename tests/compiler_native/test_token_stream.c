/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_token_stream.c
 *
 * PURPOSE:
 *   Regression coverage for provide a bounded lexical token stream with push, peek, consume and rewind operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/token_stream.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiNativeTokenStream s; umi_nc_token_stream_init(&s); UmiNativeToken t; UmiNativeSourceSpan sp={0}; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_token_init(&t,UMI_NC_TOKEN_IDENTIFIER,sp,"x")!=UMI_STATUS_OK) return 1; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_token_stream_push(&s,&t)!=UMI_STATUS_OK) return 2; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_token_stream_next(&s)==NULL||s.cursor!=1U) return 3; umi_nc_token_stream_rewind(&s); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_token_stream_peek(&s,0U)==NULL) return 4; return 0; }
