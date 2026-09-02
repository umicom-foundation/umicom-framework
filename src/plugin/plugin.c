/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/plugin.c
 *
 * PURPOSE:
 *   Implement the plugin behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/plugin.h"

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

struct UmiPluginLibrary {
#ifdef _WIN32
    HMODULE handle;
#else
    void *handle;
#endif
    const UmiModuleDescriptor *descriptor;
};

/* Read plugin into validated module state and return a status when input cannot be used. */
UmiStatus umi_plugin_load(const char *path, UmiPluginLibrary **out_plugin)
{
    UmiPluginLibrary *plugin;
    UmiModuleQueryFn query = NULL;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (path == 0 || out_plugin == 0) return UMI_STATUS_INVALID_ARGUMENT;
    plugin = (UmiPluginLibrary *)calloc(1U, sizeof(*plugin));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (plugin == 0) return UMI_STATUS_OUT_OF_MEMORY;
#ifdef _WIN32
    {
        FARPROC symbol;
        plugin->handle = LoadLibraryA(path);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (plugin->handle == 0) { free(plugin); return UMI_STATUS_IO_ERROR; }
        symbol = GetProcAddress(plugin->handle, "umicom_module_query");

        /* ISO C does not define a cast between object and function pointers.
         * Windows exposes procedure addresses as FARPROC, so copy the
         * representation into the correctly typed callback after proving that
         * the two ABI representations have the same size.  This is the same
         * strict-C technique used by the POSIX dlsym branch below. */
        _Static_assert(sizeof(query) == sizeof(symbol),
                       "Windows procedure and module-query pointer sizes must match");
        (void)memcpy(&query, &symbol, sizeof(query));
    }
#else
    {
        void *symbol;
        plugin->handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (plugin->handle == 0) { free(plugin); return UMI_STATUS_IO_ERROR; }
        symbol = dlsym(plugin->handle, "umicom_module_query");
        _Static_assert(sizeof(query) == sizeof(symbol),
                       "POSIX function and object pointer sizes must match");
        (void)memcpy(&query, &symbol, sizeof(query));
    }
#endif
    /* Apply this branch only when its contract condition is satisfied. */
    if (query == 0) {
        umi_plugin_unload(plugin);
        return UMI_STATUS_NOT_FOUND;
    }
    plugin->descriptor = query(UMICOM_FRAMEWORK_ABI_VERSION);
    /* Apply this branch only when its contract condition is satisfied. */
    if (plugin->descriptor == 0 ||
        plugin->descriptor->abi_version != UMICOM_FRAMEWORK_ABI_VERSION ||
        plugin->descriptor->structure_size < sizeof(UmiModuleDescriptor)) {
        umi_plugin_unload(plugin);
        return UMI_STATUS_INVALID_STATE;
    }
    *out_plugin = plugin;
    return UMI_STATUS_OK;
}

/* Provide the plugin unload operation used by this module and its client applications. */
void umi_plugin_unload(UmiPluginLibrary *plugin)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (plugin == 0) return;
#ifdef _WIN32
    /* Apply this branch only when its contract condition is satisfied. */
    if (plugin->handle != 0) FreeLibrary(plugin->handle);
#else
    /* Apply this branch only when its contract condition is satisfied. */
    if (plugin->handle != 0) dlclose(plugin->handle);
#endif
    free(plugin);
}

/* Provide the plugin descriptor operation used by this module and its client applications. */
const UmiModuleDescriptor *umi_plugin_descriptor(const UmiPluginLibrary *plugin)
{
    return plugin != 0 ? plugin->descriptor : 0;
}
