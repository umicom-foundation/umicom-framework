/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/data_quality_gate.c
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

#include "umicom/finance/regulatory/data_quality_gate.h"

int umi_reg_data_quality_gate_passes(double observed,double threshold){ return umi_reg_number_valid(observed)&&umi_reg_number_valid(threshold)&&observed>=0.0&&observed<=1.0&&threshold>=0.0&&threshold<=1.0&&observed>=threshold?1:0; }
