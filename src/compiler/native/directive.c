/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/directive.c
 *
 * PURPOSE:
 *   Classify C preprocessor directive lines without taking ownership of macro expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/directive.h"
#include <ctype.h>
#include <string.h>
/* Provide the kind of operation used by this module and its client applications. */
static UmiNativeDirectiveKind kind_of(const char *s){ /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"define")==0)return UMI_NC_DIRECTIVE_DEFINE; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"undef")==0)return UMI_NC_DIRECTIVE_UNDEF; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"include")==0)return UMI_NC_DIRECTIVE_INCLUDE; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"if")==0)return UMI_NC_DIRECTIVE_IF; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"ifdef")==0)return UMI_NC_DIRECTIVE_IFDEF; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"ifndef")==0)return UMI_NC_DIRECTIVE_IFNDEF; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"elif")==0)return UMI_NC_DIRECTIVE_ELIF; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"else")==0)return UMI_NC_DIRECTIVE_ELSE; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"endif")==0)return UMI_NC_DIRECTIVE_ENDIF; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"pragma")==0)return UMI_NC_DIRECTIVE_PRAGMA; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"error")==0)return UMI_NC_DIRECTIVE_ERROR; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(s,"line")==0)return UMI_NC_DIRECTIVE_LINE; return UMI_NC_DIRECTIVE_UNKNOWN; }
/*
 * Read nc directive into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_nc_directive_parse(const char *line,UmiNativeDirective *out){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(line==NULL||out==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(out,0,sizeof(*out)); /* Continue only while work remains available; the loop body advances the state on each pass. */ while(isspace((unsigned char)*line)!=0) line++; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(*line!='#') return UMI_STATUS_PARSE_ERROR; line++; /* Continue only while work remains available; the loop body advances the state on each pass. */ while(isspace((unsigned char)*line)!=0) line++; char word[32]; size_t n=0U; /* Continue only while work remains available; the loop body advances the state on each pass. */ while(isalpha((unsigned char)*line)!=0&&n+1U<sizeof(word)) word[n++]=*line++; word[n]='\0'; out->kind=kind_of(word); /* Continue only while work remains available; the loop body advances the state on each pass. */ while(isspace((unsigned char)*line)!=0) line++; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(out->argument,sizeof(out->argument),line)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; return out->kind==UMI_NC_DIRECTIVE_UNKNOWN?UMI_STATUS_NOT_FOUND:UMI_STATUS_OK; }
/*
 * Provide the nc directive kind name operation used by this module and its client
 * applications.
 */
const char *umi_nc_directive_kind_name(UmiNativeDirectiveKind k){ /* Select the behaviour associated with the requested command or state value. */ switch(k){case UMI_NC_DIRECTIVE_DEFINE:return "define";case UMI_NC_DIRECTIVE_UNDEF:return "undef";case UMI_NC_DIRECTIVE_INCLUDE:return "include";case UMI_NC_DIRECTIVE_IF:return "if";case UMI_NC_DIRECTIVE_IFDEF:return "ifdef";case UMI_NC_DIRECTIVE_IFNDEF:return "ifndef";case UMI_NC_DIRECTIVE_ELIF:return "elif";case UMI_NC_DIRECTIVE_ELSE:return "else";case UMI_NC_DIRECTIVE_ENDIF:return "endif";case UMI_NC_DIRECTIVE_PRAGMA:return "pragma";case UMI_NC_DIRECTIVE_ERROR:return "error";case UMI_NC_DIRECTIVE_LINE:return "line";default:return "unknown";} }
