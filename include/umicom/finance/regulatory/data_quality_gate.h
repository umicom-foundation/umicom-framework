/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/data_quality_gate.h
 *
 * PURPOSE:
 *   Decide whether a governed data set passes its minimum quality threshold.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_QUALITY_GATE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_DATA_QUALITY_GATE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return one when an observed quality score reaches a configured threshold. */
int umi_reg_data_quality_gate_passes(double observed,double threshold);

#ifdef __cplusplus
}
#endif

#endif
