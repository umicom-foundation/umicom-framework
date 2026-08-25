/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_health.c
 * PURPOSE: Implement developer-kit health/readiness evaluation.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_health.h"

#include <string.h>

static size_t count_bits(uint64_t value)
{
    size_t count = 0U;
    while (value != 0U) {
        count += (size_t)(value & UINT64_C(1));
        value >>= 1U;
    }
    return count;
}

UmiStatus umi_toolchain_kit_health_evaluate(
    const UmiToolchainKitSnapshot *kit,
    UmiToolchainKitCapabilityFlags required_capabilities,
    int require_sysroot_for_cross_compile,
    UmiToolchainKitHealthSnapshot *out_health)
{
    UmiToolchainKitHealthIssueFlags issues = 0U;
    UmiToolchainKitCapabilityFlags missing;
    if (kit == NULL || out_health == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_health, 0, sizeof(*out_health));
    out_health->struct_size = (uint32_t)sizeof(*out_health);
    out_health->api_version = UMI_TOOLCHAIN_KIT_HEALTH_API_VERSION;
    out_health->required_capabilities = required_capabilities;
    out_health->supported_capabilities = kit->capabilities;
    missing = required_capabilities & ~kit->capabilities;
    out_health->missing_capabilities = missing;

    if (kit->enabled == 0) issues |= UMI_TOOLCHAIN_KIT_HEALTH_DISABLED;
    if (kit->state == UMI_TOOLCHAIN_KIT_UNAVAILABLE)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_UNAVAILABLE;
    if (kit->c_compiler[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_COMPILER;
    if (kit->cmake[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_CONFIGURE) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_BUILD_SYSTEM;
    if (kit->ctest[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_TEST) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_TEST_RUNNER;
    if (kit->debugger[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_DEBUG) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_DEBUGGER;
    if (kit->sdk_id[0] == '\0' &&
        (required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_SDK) != 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_SDK;
    if (kit->target_triple[0] == '\0')
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_TARGET;
    if (require_sysroot_for_cross_compile != 0 &&
        umi_toolchain_kit_is_cross_compile(kit) != 0 && kit->sysroot[0] == '\0')
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_SYSROOT;
    if ((required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_C23) != 0U &&
        (kit->capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_C23) == 0U)
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_C23;
    if ((required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_PYTHON) != 0U &&
        kit->python[0] == '\0')
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_PYTHON;
    if ((required_capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_JAVA) != 0U &&
        (kit->java[0] == '\0' || kit->javac[0] == '\0'))
        issues |= UMI_TOOLCHAIN_KIT_HEALTH_MISSING_JAVA;
    if (missing != 0U) issues |= UMI_TOOLCHAIN_KIT_HEALTH_CAPABILITY_GAP;

    out_health->issue_flags = issues;
    out_health->issue_count = count_bits(issues);
    if (issues == 0U && kit->state == UMI_TOOLCHAIN_KIT_READY) {
        out_health->state = UMI_TOOLCHAIN_KIT_HEALTH_HEALTHY;
        out_health->ready = 1;
    } else if ((issues & (UMI_TOOLCHAIN_KIT_HEALTH_DISABLED |
                          UMI_TOOLCHAIN_KIT_HEALTH_UNAVAILABLE |
                          UMI_TOOLCHAIN_KIT_HEALTH_MISSING_COMPILER |
                          UMI_TOOLCHAIN_KIT_HEALTH_MISSING_TARGET)) != 0U) {
        out_health->state = UMI_TOOLCHAIN_KIT_HEALTH_UNHEALTHY;
    } else {
        out_health->state = UMI_TOOLCHAIN_KIT_HEALTH_DEGRADED;
    }
    return UMI_STATUS_OK;
}

const char *umi_toolchain_kit_health_state_text(
    UmiToolchainKitHealthState state)
{
    switch (state) {
        case UMI_TOOLCHAIN_KIT_HEALTH_HEALTHY: return "HEALTHY";
        case UMI_TOOLCHAIN_KIT_HEALTH_DEGRADED: return "DEGRADED";
        case UMI_TOOLCHAIN_KIT_HEALTH_UNHEALTHY: return "UNHEALTHY";
        default: return "UNKNOWN";
    }
}
