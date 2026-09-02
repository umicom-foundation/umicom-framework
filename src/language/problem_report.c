/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/problem_report.c
 *
 * PURPOSE:
 *   Implement the problem report behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework language problem summaries. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/language/problem_report.h"
#include <string.h>
/*
 * Provide the language problem report build operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_problem_report_build(const UmiLanguageDiagnosticRegistry*r,UmiLanguageProblemReport*out){size_t i,n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));n=umi_language_diagnostic_registry_count(r);/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<n;++i){UmiLanguageDiagnosticSnapshot d;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_language_diagnostic_registry_at(r,i,&d)!=UMI_STATUS_OK)continue;out->total+=1U;/* Select the behaviour associated with the requested command or state value. */ switch(d.severity){case 1:out->errors+=1U;break;case 2:out->warnings+=1U;break;case 3:out->information+=1U;break;default:out->hints+=1U;break;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d.revision>out->revision)out->revision=d.revision;}return UMI_STATUS_OK;}
