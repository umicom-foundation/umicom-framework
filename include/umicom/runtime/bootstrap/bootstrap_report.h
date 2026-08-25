/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/bootstrap_report.h
 *
 * PURPOSE:
 *   Summarise bootstrap completion, skip and failure outcomes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_REPORT_H
#define UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_REPORT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_report_init(UmiBootstrapReport *report);
UmiStatus umi_bootstrap_report_record(UmiBootstrapReport *report,
                                      const UmiBootstrapStage *stage,
                                      const char *message);

#ifdef __cplusplus
}
#endif

#endif
