/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/release_channel.c
 *
 * PURPOSE:
 *   Implement promotion requirements for development, nightly, beta and stable release channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Channels let stricter production releases demand more evidence without changing the package-building code.
 */

#include "umicom/delivery/release_channel.h"

UmiReleaseChannelPolicy umi_release_channel_policy(UmiReleaseChannel channel)
{
    UmiReleaseChannelPolicy policy = {0};
    policy.channel = channel;
    policy.require_tests = 1;
    if (channel == UMI_RELEASE_BETA || channel == UMI_RELEASE_STABLE) {
        policy.require_sbom = 1;
    }
    if (channel == UMI_RELEASE_STABLE) {
        policy.require_signature = 1;
        policy.require_zero_warnings = 1;
    }
    return policy;
}
