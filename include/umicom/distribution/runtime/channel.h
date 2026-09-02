/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/channel.h
 *
 * PURPOSE:
 *   release channel descriptors and stability ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_CHANNEL_H
#define UMICOM_DISTRIBUTION_RUNTIME_CHANNEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr channel data shared with callers of this public contract.
 */
typedef struct UmiDrChannel { char id[UMI_DR_ID_CAPACITY]; UmiDrChannelKind kind; uint32_t stability_rank; bool signed_only; bool automatic_updates; } UmiDrChannel;
/**
 * Initialise dr channel from caller-provided values so later operations receive a known
 * state.
 */
void umi_dr_channel_init(UmiDrChannel *value);
/**
 * Check that dr channel satisfies its contract before another service relies on it.
 */
bool umi_dr_channel_valid(const UmiDrChannel *value);
/**
 * Provide the dr channel fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_channel_fingerprint(const UmiDrChannel *value);

#ifdef __cplusplus
}
#endif
#endif
