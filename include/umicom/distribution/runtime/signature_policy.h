/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/signature_policy.h
 *
 * PURPOSE:
 *   signature requirement and trusted-publisher policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_SIGNATURE_POLICY_H
#define UMICOM_DISTRIBUTION_RUNTIME_SIGNATURE_POLICY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr signature policy data shared with callers of this public contract.
 */
typedef struct UmiDrSignaturePolicy { bool required; bool trusted_publisher_only; bool timestamp_required; } UmiDrSignaturePolicy;
/**
 * Initialise dr signature policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_signature_policy_init(UmiDrSignaturePolicy *policy); bool umi_dr_signature_policy_accept(const UmiDrSignaturePolicy *policy,bool signed_artifact,bool trusted_publisher,bool timestamped);

#ifdef __cplusplus
}
#endif
#endif
