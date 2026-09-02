/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/health.h
 *
 * PURPOSE:
 *   Summarise post-deployment readiness and liveness checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Health gates decide whether a generation should stay active or whether the release process should roll back.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_HEALTH_H
#define INCLUDE_UMICOM_DELIVERY_HEALTH_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the delivery health data shared with callers of this public contract.
 */
typedef struct UmiDeliveryHealth {
    int ready;
    int live;
    uint32_t checks_passed;
    uint32_t checks_failed;
    char message[UMI_DELIVERY_TEXT_CAPACITY];
} UmiDeliveryHealth;
/**
 * Initialise delivery health from caller-provided values so later operations receive a
 * known state.
 */
void umi_delivery_health_init(UmiDeliveryHealth *health);
/**
 * Provide the delivery health acceptable operation used by this module and its client
 * applications.
 */
int umi_delivery_health_acceptable(const UmiDeliveryHealth *health);

#ifdef __cplusplus
}
#endif

#endif
