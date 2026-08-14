/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/frontend/frontend_studio.h"
int main(void){UmiFrontendQualityBudget b=umi_frontend_quality_budget_default();UmiFrontendQualityMetrics m={0};UmiFrontendQualityResult r;m.transfer_bytes=1000U;m.script_bytes=500U;m.render_us=10000U;m.accessibility_score=98U;assert(umi_frontend_quality_audit_run(&b,&m,&r)==UMI_STATUS_OK);assert(r.passed&&strstr(r.summary,"passed")!=NULL);m.console_errors=1U;assert(umi_frontend_quality_audit_run(&b,&m,&r)==UMI_STATUS_OK&&!r.passed);return 0;}
