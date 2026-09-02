/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_health.c
 * PURPOSE: Implement developer-kit health/readiness evaluation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_health.h"

#include <string.h>

/* Provide the count bits operation used by this module and its client applications. */
static size_t count_bits(uint64_t value)
{
    size_t count = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (value != 0U) {
        count += (size_t)(value & UINT64_C(1));
        value >>= 1U;
    }
    return count;
}

/*
 * Provide the toolchain kit health evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_health_evaluate(
    const UmiToolchainKitSnapshot *kit,
    UmiToolchainKitCapabilityFlags required_capabilities,
    int require_sysroot_for_cross_compile,
    UmiToolchainKitHealthSnapshot *out_health)
{
    UmiToolchainKitHealthIssueFlags issues = 0U;
    UmiToolchainKitCapabilityFlags missing;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || out_health == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_health, 0, sizeof(*out_health));
    out_health->struct_size = (uint32_t)sizeof(*out_health);
    out_health->api_version = UMI_TOOLCHAIN_KIT_HEALTH_API_VERSION;
    out_health->required_capabilities = required_capabilities;
    out_health->supported_capabilities = kit->capabilities;
    missing = required_capabilities & ~kit->capabilities;
    out_health->missing_capabilities = missing;

    /* Apply this operation only while the related capability or state is available. */
    if (kit->enabled == 0) issues |= UMI_TOOLCHAIN_KIT_HEALTH_DISABLED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->state == UMI_TOOLCHAIN_KIT_UNAVAILABLE)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_UNAVAILABLE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->c_compiler[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_COMPILER;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->cmake[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_CONFIGURE) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_BUILD_SYSTEM;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->ctest[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_TEST) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_TEST_RUNNER;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->debugger[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_DEBUG) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_DEBUGGER;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (kit->sdk_id[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_SDK) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_SDK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->target_triple[0] == '\0')
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_TARGET;
    /* Apply this branch only when its contract condition is satisfied. */
    if (require_sysroot_for_cross_compile != 0 &&
        umi_toolchain_kit_is_cross_compile(kit) != 0 && kit->sysroot[0] == '\0')
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_SYSROOT;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_C23) != 0U &&
        (kit->capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_C23) == 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_C23;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_PYTHON) != 0U &&
        kit->python[0] == '\0')
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_PYTHON;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_JAVA) != 0U &&
        (kit->java[0] == '\0' || kit->javac[0] == '\0'))
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_JAVA;
    /* Apply this branch only when its contract condition is satisfied. */
    if (missing != 0U) issues |= UMI_TOOLCHAIN_KIT_HEALTH_CAPABILITY_GAP;

    out_health->issue_flags = issues;
    out_health->issue_count = count_bits(issues);
    /* Apply this operation only while the related capability or state is available. */
    if (issues == 0U && kit->state == UMI_TOOLCHAIN_KIT_READY) {
        out_health->state = UMI_TOOLCHAIN_KIT_HEALTH_HEALTHY;
        out_health->ready = 1;
    } else /* Apply this branch only when its contract condition is satisfied. */ if ((issues & (UMI_TOOLCHAIN_KIT_HEALTH_DISABLED |
                          UMI_TOOLCHAIN_KIT_HEALTH_UNAVAILABLE |
                          UMI_TOOLCHAIN_KIT_HEALTH_MISSING_COMPILER |
                          UMI_TOOLCHAIN_KIT_HEALTH_MISSING_TARGET)) != 0U) {
        out_health->state = UMI_TOOLCHAIN_KIT_HEALTH_UNHEALTHY;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_health->state = UMI_TOOLCHAIN_KIT_HEALTH_DEGRADED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain kit health state text operation used by this module and its client
 * applications.
 */
const char *umi_toolchain_kit_health_state_text(
    UmiToolchainKitHealthState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_TOOLCHAIN_KIT_HEALTH_HEALTHY: return "HEALTHY";
        case UMI_TOOLCHAIN_KIT_HEALTH_DEGRADED: return "DEGRADED";
        case UMI_TOOLCHAIN_KIT_HEALTH_UNHEALTHY: return "UNHEALTHY";
        default: return "UNKNOWN";
    }
}
