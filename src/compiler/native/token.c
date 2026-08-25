/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/token.c
 *
 * PURPOSE:
 *   Represent lexical tokens with source spans, keyword classification and deterministic text fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/token.h"
#include <string.h>
static const char *const keywords[]={"alignas","alignof","auto","bool","break","case","char","const","constexpr","continue","default","do","double","else","enum","extern","false","float","for","goto","if","inline","int","long","nullptr","register","restrict","return","short","signed","sizeof","static","static_assert","struct","switch","thread_local","true","typedef","typeof","typeof_unqual","union","unsigned","void","volatile","while","_Atomic","_BitInt","_Complex","_Decimal32","_Decimal64","_Decimal128","_Generic","_Noreturn"};
bool umi_nc_token_is_keyword_text(const char *text){ if(text==NULL) return false; for(size_t i=0U;i<sizeof(keywords)/sizeof(keywords[0]);i++) if(strcmp(text,keywords[i])==0) return true; return false; }
UmiStatus umi_nc_token_init(UmiNativeToken *token,UmiNativeTokenKind kind,UmiNativeSourceSpan span,const char *text){ if(token==NULL||text==NULL) return UMI_STATUS_INVALID_ARGUMENT; token->kind=kind; token->span=span; UmiStatus st=umi_nc_copy_text(token->text,sizeof(token->text),text); if(st!=UMI_STATUS_OK) return st; if(kind==UMI_NC_TOKEN_IDENTIFIER && umi_nc_token_is_keyword_text(text)) token->kind=UMI_NC_TOKEN_KEYWORD; token->fingerprint=umi_nc_hash_text(token->text)^((uint64_t)token->kind<<56U); return UMI_STATUS_OK; }
bool umi_nc_token_matches(const UmiNativeToken *token,const char *text){ return token!=NULL&&text!=NULL&&strcmp(token->text,text)==0; }
