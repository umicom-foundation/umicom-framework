/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release_channel.h
 *
 * PURPOSE:
 *   Define promotion requirements for development, nightly, beta and stable release channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Channels let stricter production releases demand more evidence without changing the package-building code.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_RELEASE_CHANNEL_H
#define INCLUDE_UMICOM_DELIVERY_RELEASE_CHANNEL_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseChannelPolicy {
    UmiReleaseChannel channel;
    int require_tests;
    int require_signature;
    int require_sbom;
    int require_zero_warnings;
} UmiReleaseChannelPolicy;

UmiReleaseChannelPolicy umi_release_channel_policy(UmiReleaseChannel channel);

#ifdef __cplusplus
}
#endif

#endif
