/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/auto_configuration_report.h
 *
 * PURPOSE:
 *   Record applied, skipped and failed auto-configuration outcomes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_REPORT_H
#define UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_REPORT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_auto_configuration_report_init(
    UmiBootstrapAutoConfigurationReport *report);
UmiStatus umi_bootstrap_auto_configuration_report_record(
    UmiBootstrapAutoConfigurationReport *report,
    UmiBootstrapStageState outcome,
    const char *message);

#ifdef __cplusplus
}
#endif

#endif
