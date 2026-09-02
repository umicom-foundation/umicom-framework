/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/distribution_session.h
 *
 * PURPOSE:
 *   top-level distribution planning session aggregating runtime/package/update readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DISTRIBUTION_SESSION_H
#define UMICOM_DISTRIBUTION_RUNTIME_DISTRIBUTION_SESSION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr distribution session data shared with callers of this public contract.
 */
typedef struct UmiDrDistributionSession { char id[UMI_DR_ID_CAPACITY]; UmiDrState state; bool runtime_ready; bool package_ready; bool release_gate_passed; bool deployment_ready; uint64_t revision; } UmiDrDistributionSession;
/**
 * Initialise dr distribution session from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_distribution_session_init(UmiDrDistributionSession *session,const char *id); bool umi_dr_distribution_session_ready(const UmiDrDistributionSession *session); UmiStatus umi_dr_distribution_session_commit(UmiDrDistributionSession *session);

#ifdef __cplusplus
}
#endif
#endif
