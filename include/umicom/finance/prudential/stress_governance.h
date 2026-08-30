/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_governance.h
 *
 * PURPOSE:
 *   Define review, approval and sign-off state for an enterprise stress run.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_GOVERNANCE_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_GOVERNANCE_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialStressGovernance { char run_id[UMI_PRU_ID_CAPACITY]; int model_validated; int independent_reviewed; int management_approved; UmiStressRunState state; } UmiPrudentialStressGovernance;
/* Derive stress-run readiness from model validation, independent review and management approval. */
UmiStatus umi_pru_stress_governance_evaluate(UmiPrudentialStressGovernance *record, const char *run_id, int model_validated, int independent_reviewed, int management_approved);

#ifdef __cplusplus
}
#endif

#endif
