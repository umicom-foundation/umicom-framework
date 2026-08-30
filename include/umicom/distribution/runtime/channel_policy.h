/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/channel_policy.h
 *
 * PURPOSE:
 *   allowed channel transitions and downgrade policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_CHANNEL_POLICY_H
#define UMICOM_DISTRIBUTION_RUNTIME_CHANNEL_POLICY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrChannelPolicy { bool allow_downgrade; bool allow_prerelease_to_stable; } UmiDrChannelPolicy;
void umi_dr_channel_policy_init(UmiDrChannelPolicy *policy); bool umi_dr_channel_policy_transition(const UmiDrChannelPolicy *policy,UmiDrChannelKind from,UmiDrChannelKind to);

#ifdef __cplusplus
}
#endif
#endif
