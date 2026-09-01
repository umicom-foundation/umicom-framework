/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/policy.h
 *
 * PURPOSE:
 *   Express release policy requirements and evaluate build evidence against them.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Policy keeps channel rules out of product-specific scripts and makes release decisions reproducible.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_POLICY_H
#define INCLUDE_UMICOM_DELIVERY_POLICY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/build_evidence.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeliveryPolicy {
    int require_tests;
    int require_zero_warnings;
    int require_signature;
    int require_sbom;
    int require_clean_source;
} UmiDeliveryPolicy;

UmiDeliveryPolicy umi_delivery_policy_default(void);
UmiStatus umi_delivery_policy_check_build(const UmiDeliveryPolicy *policy,
                                          const UmiBuildEvidence *evidence);

#ifdef __cplusplus
}
#endif

#endif
