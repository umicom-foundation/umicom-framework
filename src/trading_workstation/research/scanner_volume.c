/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/research/scanner_volume.c
 *
 * PURPOSE:
 *   Apply the shared Scanner Volume Signal calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/research/scanner_volume.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_research_scanner_volume(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "research.scanner-volume",
        "Scanner Volume Signal",
        UMI_TRADING_POLICY_VOLUME,
        100.000000,
        50.000000,
        UINT32_C(1),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
