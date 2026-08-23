/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/builtin_profiles.c
 *
 * PURPOSE:
 *   Enumerate/register built-in external DAP adapters without placing adapter
 *   discovery or executable names in Umicom Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

size_t umi_debug_runtime_builtin_profile_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_at(size_t index)
{
    return index < umi_debug_runtime_builtin_profile_count()
        ? FACTORIES[index]() : NULL;
}

const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_find(
    const char *profile_id)
{
    size_t index;
    if (profile_id == NULL) return NULL;

    for (index = 0U; index < umi_debug_runtime_builtin_profile_count(); ++index) {
        const UmiDebugAdapterProfile *profile = FACTORIES[index]();
        if (strcmp(profile->id, profile_id) == 0) return profile;
    }
    return NULL;
}

const UmiDebugAdapterProfile *umi_debug_runtime_builtin_profile_for_kind(
    const char *debugger_kind)
{
    size_t index;
    if (debugger_kind == NULL) return NULL;

    for (index = 0U; index < umi_debug_runtime_builtin_profile_count(); ++index) {
        const UmiDebugAdapterProfile *profile = FACTORIES[index]();
        if (profile->enabled &&
            strcmp(profile->debugger_kind, debugger_kind) == 0) {
            return profile;
        }
    }
    return NULL;
}

UmiStatus umi_debug_runtime_register_builtin_profiles(UmiDebugService *service)
{
    UmiDebugAdapterProfileRegistry *registry;
    size_t index;

    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry = umi_debug_service_adapter_profiles(service);
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    for (index = 0U; index < umi_debug_runtime_builtin_profile_count(); ++index) {
        UmiStatus status = umi_debug_adapter_profile_registry_upsert(
            registry, FACTORIES[index]());
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
