/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/settlement/instruction.h
 *
 * PURPOSE:
 *   Evaluate Settlement Instruction through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_SETTLEMENT_INSTRUCTION_H
#define UMICOM_FINANCIAL_PLATFORM_SETTLEMENT_INSTRUCTION_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_settlement_instruction(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
