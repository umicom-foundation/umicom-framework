/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/directive.c
 *
 * PURPOSE:
 *   Classify C preprocessor directive lines without taking ownership of macro expansion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/directive.h"
#include <ctype.h>
#include <string.h>
static UmiNativeDirectiveKind kind_of(const char *s){ if(strcmp(s,"define")==0)return UMI_NC_DIRECTIVE_DEFINE; if(strcmp(s,"undef")==0)return UMI_NC_DIRECTIVE_UNDEF; if(strcmp(s,"include")==0)return UMI_NC_DIRECTIVE_INCLUDE; if(strcmp(s,"if")==0)return UMI_NC_DIRECTIVE_IF; if(strcmp(s,"ifdef")==0)return UMI_NC_DIRECTIVE_IFDEF; if(strcmp(s,"ifndef")==0)return UMI_NC_DIRECTIVE_IFNDEF; if(strcmp(s,"elif")==0)return UMI_NC_DIRECTIVE_ELIF; if(strcmp(s,"else")==0)return UMI_NC_DIRECTIVE_ELSE; if(strcmp(s,"endif")==0)return UMI_NC_DIRECTIVE_ENDIF; if(strcmp(s,"pragma")==0)return UMI_NC_DIRECTIVE_PRAGMA; if(strcmp(s,"error")==0)return UMI_NC_DIRECTIVE_ERROR; if(strcmp(s,"line")==0)return UMI_NC_DIRECTIVE_LINE; return UMI_NC_DIRECTIVE_UNKNOWN; }
UmiStatus umi_nc_directive_parse(const char *line,UmiNativeDirective *out){ if(line==NULL||out==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(out,0,sizeof(*out)); while(isspace((unsigned char)*line)!=0) line++; if(*line!='#') return UMI_STATUS_PARSE_ERROR; line++; while(isspace((unsigned char)*line)!=0) line++; char word[32]; size_t n=0U; while(isalpha((unsigned char)*line)!=0&&n+1U<sizeof(word)) word[n++]=*line++; word[n]='\0'; out->kind=kind_of(word); while(isspace((unsigned char)*line)!=0) line++; if(umi_nc_copy_text(out->argument,sizeof(out->argument),line)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; return out->kind==UMI_NC_DIRECTIVE_UNKNOWN?UMI_STATUS_NOT_FOUND:UMI_STATUS_OK; }
const char *umi_nc_directive_kind_name(UmiNativeDirectiveKind k){ switch(k){case UMI_NC_DIRECTIVE_DEFINE:return "define";case UMI_NC_DIRECTIVE_UNDEF:return "undef";case UMI_NC_DIRECTIVE_INCLUDE:return "include";case UMI_NC_DIRECTIVE_IF:return "if";case UMI_NC_DIRECTIVE_IFDEF:return "ifdef";case UMI_NC_DIRECTIVE_IFNDEF:return "ifndef";case UMI_NC_DIRECTIVE_ELIF:return "elif";case UMI_NC_DIRECTIVE_ELSE:return "else";case UMI_NC_DIRECTIVE_ENDIF:return "endif";case UMI_NC_DIRECTIVE_PRAGMA:return "pragma";case UMI_NC_DIRECTIVE_ERROR:return "error";case UMI_NC_DIRECTIVE_LINE:return "line";default:return "unknown";} }
