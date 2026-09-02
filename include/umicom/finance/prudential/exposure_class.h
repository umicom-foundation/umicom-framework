/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/exposure_class.h
 *
 * PURPOSE:
 *   Describe one prudential exposure class and default risk weight.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_EXPOSURE_CLASS_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_EXPOSURE_CLASS_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential exposure class data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialExposureClass { char exposure_class_id[UMI_PRU_ID_CAPACITY]; double risk_weight; } UmiPrudentialExposureClass;
/* Initialise a bounded risk-weight rule. */
UmiStatus umi_pru_exposure_class_init(UmiPrudentialExposureClass *rule, const char *exposure_class_id, double risk_weight);

#ifdef __cplusplus
}
#endif

#endif
