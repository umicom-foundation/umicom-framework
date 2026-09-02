/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/checksum_policy.h
 *
 * PURPOSE:
 *   checksum algorithm and digest presence/match requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_CHECKSUM_POLICY_H
#define UMICOM_DISTRIBUTION_RUNTIME_CHECKSUM_POLICY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr checksum policy data shared with callers of this public contract.
 */
typedef struct UmiDrChecksumPolicy { char algorithm[32]; bool digest_required; bool match_required; } UmiDrChecksumPolicy;
/**
 * Initialise dr checksum policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_checksum_policy_init(UmiDrChecksumPolicy *policy); bool umi_dr_checksum_policy_accept(const UmiDrChecksumPolicy *policy,const char *expected,const char *actual);

#ifdef __cplusplus
}
#endif
#endif
