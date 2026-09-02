/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/report.c
 *
 * PURPOSE:
 *   Render CodeGuard findings as text, JSON or SARIF for local use and CI
 *   systems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/report.h"
#include "umicom/codeguard/category.h"
#include "umicom/codeguard/severity.h"
#include "umicom/codeguard/summary.h"
#include <stdio.h>
#include <string.h>
/* Provide the jsons operation used by this module and its client applications. */
static void jsons(FILE *s,const char *v){fputc('"',s);/* Visit each bounded item once so every record receives the same rule. */ for(;v!=NULL&&*v!='\0';++v){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*v=='"'||*v=='\\')fputc('\\',s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*v=='\n'){fputs("\\n",s);continue;}fputc(*v,s);}fputc('"',s);}
/* Provide the text operation used by this module and its client applications. */
static UmiStatus text(FILE *s,const UmiCodeGuardResult *r){UmiCodeGuardSummary m=umi_codeguard_summary_build(r);fprintf(s,"Umicom CodeGuard: %zu findings (critical=%zu high=%zu medium=%zu low=%zu info=%zu)\n",m.total,m.critical,m.high,m.medium,m.low,m.info);/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<umi_codeguard_result_count(r);++i){const UmiCodeGuardFinding *f=umi_codeguard_result_at(r,i);fprintf(s,"[%s] %s %s:%zu:%zu %s",umi_codeguard_severity_text(f->severity),f->rule_id,f->path,f->line,f->column,f->message);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(f->cwe[0]!='\0')fprintf(s," (%s)",f->cwe);fprintf(s,"\n  Fix: %s\n",f->remediation);}return UMI_STATUS_OK;}
/* Provide the json operation used by this module and its client applications. */
static UmiStatus json(FILE *s,const UmiCodeGuardResult *r){fputs("{\"tool\":\"Umicom CodeGuard\",\"findings\":[",s);/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<umi_codeguard_result_count(r);++i){const UmiCodeGuardFinding *f=umi_codeguard_result_at(r,i);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(i>0U)fputc(',',s);fputs("{\"rule\":",s);jsons(s,f->rule_id);fputs(",\"severity\":",s);jsons(s,umi_codeguard_severity_text(f->severity));fputs(",\"category\":",s);jsons(s,umi_codeguard_category_text(f->category));fputs(",\"path\":",s);jsons(s,f->path);fprintf(s,",\"line\":%zu,\"confidence\":%u,\"message\":",f->line,f->confidence);jsons(s,f->message);fputs("}",s);}fputs("]}\n",s);return UMI_STATUS_OK;}
/* Provide the sarif operation used by this module and its client applications. */
static UmiStatus sarif(FILE *s,const UmiCodeGuardResult *r){fputs("{\"version\":\"2.1.0\",\"runs\":[{\"tool\":{\"driver\":{\"name\":\"Umicom CodeGuard\"}},\"results\":[",s);/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<umi_codeguard_result_count(r);++i){const UmiCodeGuardFinding *f=umi_codeguard_result_at(r,i);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(i>0U)fputc(',',s);fputs("{\"ruleId\":",s);jsons(s,f->rule_id);fputs(",\"level\":",s);jsons(s,f->severity>=UMI_CODEGUARD_HIGH?"error":(f->severity==UMI_CODEGUARD_MEDIUM?"warning":"note"));fputs(",\"message\":{\"text\":",s);jsons(s,f->message);fputs("},\"locations\":[{\"physicalLocation\":{\"artifactLocation\":{\"uri\":",s);jsons(s,f->path);fprintf(s,"},\"region\":{\"startLine\":%zu}}}]} ",f->line);fputs("}",s);}fputs("]}]}\n",s);return UMI_STATUS_OK;}
/*
 * Write codeguard report in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_codeguard_report_write(FILE *s,UmiCodeGuardReportFormat f,const UmiCodeGuardResult *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Select the behaviour associated with the requested command or state value. */ switch(f){case UMI_CODEGUARD_REPORT_TEXT:return text(s,r);case UMI_CODEGUARD_REPORT_JSON:return json(s,r);case UMI_CODEGUARD_REPORT_SARIF:return sarif(s,r);default:return UMI_STATUS_INVALID_ARGUMENT;}}
/*
 * Provide the codeguard report file operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_report_file(const char *p,UmiCodeGuardReportFormat f,const UmiCodeGuardResult *r){FILE *s;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=fopen(p,"wb");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_IO_ERROR;st=umi_codeguard_report_write(s,f,r);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(fclose(s)!=0&&st==UMI_STATUS_OK)st=UMI_STATUS_IO_ERROR;return st;}
