/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_bootstrap_report.c
 *
 * PURPOSE:
 *   Focused regression coverage for Summarise bootstrap completion, skip and failure outcomes.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/bootstrap_stage.h"
#include "umicom/runtime/bootstrap/bootstrap_report.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapStage s; UmiBootstrapReport r; umi_bootstrap_report_init(&r);
    CHECK(umi_bootstrap_stage_init(&s,"bootstrap.ready",50)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_stage_transition(&s,UMI_BOOTSTRAP_STAGE_READY)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_stage_transition(&s,UMI_BOOTSTRAP_STAGE_RUNNING)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_stage_transition(&s,UMI_BOOTSTRAP_STAGE_COMPLETE)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_report_record(&r,&s,"ready")==UMI_STATUS_OK);
    CHECK(r.completed==1U && strcmp(r.last_stage,"bootstrap.ready")==0);
    return 0;
}
