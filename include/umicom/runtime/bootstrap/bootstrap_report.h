/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/bootstrap_report.h
 *
 * PURPOSE:
 *   Summarise bootstrap completion, skip and failure outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_REPORT_H
#define UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_REPORT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap report from caller-provided values so later operations receive a
 * known state.
 */
void umi_bootstrap_report_init(UmiBootstrapReport *report);
/**
 * Provide the bootstrap report record operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_report_record(UmiBootstrapReport *report,
                                      const UmiBootstrapStage *stage,
                                      const char *message);

#ifdef __cplusplus
}
#endif

#endif
