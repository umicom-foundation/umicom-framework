/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/preprocess_plan.c
 *
 * PURPOSE:
 *   Scan preprocessor structure, count directives and validate conditional nesting before expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/preprocess_plan.h"
#include <string.h>
UmiStatus umi_nc_preprocess_plan_scan(const UmiNativeSourceBuffer *s,UmiNativePreprocessPlan *p){ if(s==NULL||p==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(p,0,sizeof(*p)); p->source_fingerprint=umi_nc_source_buffer_fingerprint(s); size_t depth=0U,start=0U; while(start<s->length){ size_t end=start; while(end<s->length&&s->text[end]!='\n') end++; size_t n=end-start; if(n>=UMI_NC_TEXT_CAPACITY){p->errors++;} else { char line[UMI_NC_TEXT_CAPACITY]; memcpy(line,s->text+start,n);line[n]='\0'; const char *q=line; while(*q==' '||*q=='\t') q++; if(*q=='#'){ UmiNativeDirective d; UmiStatus st=umi_nc_directive_parse(q,&d); p->directives++; if(st!=UMI_STATUS_OK){p->errors++;} else { if(d.kind==UMI_NC_DIRECTIVE_DEFINE)p->defines++; if(d.kind==UMI_NC_DIRECTIVE_INCLUDE)p->includes++; if(d.kind==UMI_NC_DIRECTIVE_IF||d.kind==UMI_NC_DIRECTIVE_IFDEF||d.kind==UMI_NC_DIRECTIVE_IFNDEF){depth++;p->conditionals++;if(depth>p->max_conditional_depth)p->max_conditional_depth=depth;} else if(d.kind==UMI_NC_DIRECTIVE_ENDIF){ if(depth==0U)p->errors++; else depth--; } } } } start=end<s->length?end+1U:end; } if(depth!=0U)p->errors+=depth; return p->errors==0U?UMI_STATUS_OK:UMI_STATUS_PARSE_ERROR; }
bool umi_nc_preprocess_plan_ready(const UmiNativePreprocessPlan *p){ return p!=NULL&&p->errors==0U&&p->source_fingerprint!=0U; }
