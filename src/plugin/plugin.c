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

UmiStatus umi_plugin_load(const char *path, UmiPluginLibrary **out_plugin)
{
    UmiPluginLibrary *plugin;
    UmiModuleQueryFn query = NULL;
    if (path == 0 || out_plugin == 0) return UMI_STATUS_INVALID_ARGUMENT;
    plugin = (UmiPluginLibrary *)calloc(1U, sizeof(*plugin));
    if (plugin == 0) return UMI_STATUS_OUT_OF_MEMORY;
#ifdef _WIN32
    {
        FARPROC symbol;
        plugin->handle = LoadLibraryA(path);
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
        if (plugin->handle == 0) { free(plugin); return UMI_STATUS_IO_ERROR; }
        symbol = dlsym(plugin->handle, "umicom_module_query");
        _Static_assert(sizeof(query) == sizeof(symbol),
                       "POSIX function and object pointer sizes must match");
        (void)memcpy(&query, &symbol, sizeof(query));
    }
#endif
    if (query == 0) {
        umi_plugin_unload(plugin);
        return UMI_STATUS_NOT_FOUND;
    }
    plugin->descriptor = query(UMICOM_FRAMEWORK_ABI_VERSION);
    if (plugin->descriptor == 0 ||
        plugin->descriptor->abi_version != UMICOM_FRAMEWORK_ABI_VERSION ||
        plugin->descriptor->structure_size < sizeof(UmiModuleDescriptor)) {
        umi_plugin_unload(plugin);
        return UMI_STATUS_INVALID_STATE;
    }
    *out_plugin = plugin;
    return UMI_STATUS_OK;
}

void umi_plugin_unload(UmiPluginLibrary *plugin)
{
    if (plugin == 0) return;
#ifdef _WIN32
    if (plugin->handle != 0) FreeLibrary(plugin->handle);
#else
    if (plugin->handle != 0) dlclose(plugin->handle);
#endif
    free(plugin);
}

const UmiModuleDescriptor *umi_plugin_descriptor(const UmiPluginLibrary *plugin)
{
    return plugin != 0 ? plugin->descriptor : 0;
}
