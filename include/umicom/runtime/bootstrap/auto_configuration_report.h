/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/auto_configuration_report.h
 *
 * PURPOSE:
 *   Record applied, skipped and failed auto-configuration outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_REPORT_H
#define UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_REPORT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap auto configuration report from caller-provided values so later
 * operations receive a known state.
 */
void umi_bootstrap_auto_configuration_report_init(
    UmiBootstrapAutoConfigurationReport *report);
/**
 * Provide the bootstrap auto configuration report record operation used by this module and
 * its client applications.
 */
UmiStatus umi_bootstrap_auto_configuration_report_record(
    UmiBootstrapAutoConfigurationReport *report,
    UmiBootstrapStageState outcome,
    const char *message);

#ifdef __cplusplus
}
#endif

#endif
