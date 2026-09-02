/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/builtin_profiles.c
 *
 * PURPOSE:
 *   Enumerate/register built-in external DAP adapters without placing adapter
 *   discovery or executable names in Umicom Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/builtin_profiles.h"
#include "umicom/debug_runtime/profiles/profiles.h"
#include <string.h>

typedef const UmiDebugAdapterProfile *(*ProfileFactory)(void);

static const ProfileFactory FACTORIES[] = {
    umi_debug_runtime_profile_gdb_dap,
    umi_debug_runtime_profile_lldb_dap,
    umi_debug_runtime_profile_codelldb,
    umi_debug_runtime_profile_cppvsdbg,
    umi_debug_runtime_profile_debugpy,
    umi_debug_runtime_profile_java_debug,
    umi_debug_runtime_profile_js_debug,
    umi_debug_runtime_profile_php_debug,
    umi_debug_runtime_profile_netcoredbg
};

/*
 * Return the number of records represented by debug runtime builtin profile without
 * changing their state.
 */
size_t umi_debug_runtime_builtin_profile_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

/*
 * Find debug runtime builtin profile while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_at(size_t index)
{
    return index < umi_debug_runtime_builtin_profile_count()
        ? FACTORIES[index]() : NULL;
}

/*
 * Find debug runtime builtin profile while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_find(
    const char *profile_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_runtime_builtin_profile_count(); ++index) {
        const UmiDebugAdapterProfile *profile = FACTORIES[index]();
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(profile->id, profile_id) == 0) return profile;
    }
    return NULL;
}

/*
 * Provide the debug runtime builtin profile for kind operation used by this module and its
 * client applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_for_kind(
    const char *debugger_kind)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (debugger_kind == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_runtime_builtin_profile_count(); ++index) {
        const UmiDebugAdapterProfile *profile = FACTORIES[index]();
        /* Apply this operation only while the related capability or state is available. */
        if (profile->enabled &&
            strcmp(profile->debugger_kind, debugger_kind) == 0) {
            return profile;
        }
    }
    return NULL;
}

/*
 * Provide the debug runtime register builtin profiles operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_register_builtin_profiles(UmiDebugService *service)
{
    UmiDebugAdapterProfileRegistry *registry;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry = umi_debug_service_adapter_profiles(service);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_runtime_builtin_profile_count(); ++index) {
        UmiStatus status = umi_debug_adapter_profile_registry_upsert(
            registry, FACTORIES[index]());
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
