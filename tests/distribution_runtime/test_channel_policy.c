/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_channel_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for allowed channel transitions and downgrade policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/channel_policy.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrChannelPolicy p; umi_dr_channel_policy_init(&p); CHECK(umi_dr_channel_policy_transition(&p,UMI_DR_CHANNEL_BETA,UMI_DR_CHANNEL_STABLE)); CHECK(!umi_dr_channel_policy_transition(&p,UMI_DR_CHANNEL_STABLE,UMI_DR_CHANNEL_BETA));
    return 0;
}
