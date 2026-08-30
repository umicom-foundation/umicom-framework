/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_auto_configuration_report.c
 *
 * PURPOSE:
 *   Implement the test auto configuration report behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_auto_configuration_report.c
 *
 * PURPOSE:
 *   Focused regression coverage for Record applied, skipped and failed auto-configuration outcomes.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/auto_configuration_report.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapAutoConfigurationReport r; umi_bootstrap_auto_configuration_report_init(&r);
    CHECK(umi_bootstrap_auto_configuration_report_record(&r,UMI_BOOTSTRAP_STAGE_COMPLETE,"ok")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_auto_configuration_report_record(&r,UMI_BOOTSTRAP_STAGE_SKIPPED,"skip")==UMI_STATUS_OK);
    CHECK(r.applied==1U && r.skipped==1U);
    return 0;
}
