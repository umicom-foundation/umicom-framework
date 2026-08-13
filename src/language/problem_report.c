/* Umicom Framework language problem summaries. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/language/problem_report.h"
#include <string.h>
UmiStatus umi_language_problem_report_build(const UmiLanguageDiagnosticRegistry*r,UmiLanguageProblemReport*out){size_t i,n;if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));n=umi_language_diagnostic_registry_count(r);for(i=0U;i<n;++i){UmiLanguageDiagnosticSnapshot d;if(umi_language_diagnostic_registry_at(r,i,&d)!=UMI_STATUS_OK)continue;out->total+=1U;switch(d.severity){case 1:out->errors+=1U;break;case 2:out->warnings+=1U;break;case 3:out->information+=1U;break;default:out->hints+=1U;break;}if(d.revision>out->revision)out->revision=d.revision;}return UMI_STATUS_OK;}
