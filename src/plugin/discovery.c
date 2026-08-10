/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/discovery.c
 *
 * PURPOSE:
 *   Walk directories in stable order and parse files ending in .umicom-plugin
 *   without executing them.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/discovery.h"

#include <stddef.h>
#include <string.h>
#include "umicom/platform/directory.h"
typedef struct DiscoveryContext { UmiPluginDiscoverySink sink; void *user_data; UmiPluginDiscoveryReport *report; } DiscoveryContext;
static int has_suffix(const char *text, const char *suffix)
{ size_t a = strlen(text), b = strlen(suffix); return a >= b && strcmp(text + a - b, suffix) == 0; }
static UmiStatus discovery_visit(const UmiFileInfo *info, void *user_data)
{
    DiscoveryContext *context = (DiscoveryContext *)user_data; UmiPluginManifest manifest; UmiStatus status;
    ++context->report->files_examined;
    if (info->kind != UMI_FILE_KIND_REGULAR || !has_suffix(info->name, ".umicom-plugin")) return UMI_STATUS_OK;
    status = umi_plugin_manifest_load(info->path, &manifest);
    if (status != UMI_STATUS_OK) { ++context->report->invalid_manifests; return UMI_STATUS_OK; }
    ++context->report->manifests_loaded;
    return context->sink != NULL ? context->sink(info->path, &manifest, context->user_data) : UMI_STATUS_OK;
}
UmiStatus umi_plugin_discover(const char *root, int recursive, UmiPluginDiscoverySink sink, void *user_data, UmiPluginDiscoveryReport *out_report)
{
    UmiDirectoryWalkOptions options; DiscoveryContext context;
    if (root == NULL || out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_report->files_examined = 0U; out_report->manifests_loaded = 0U; out_report->invalid_manifests = 0U;
    options = umi_directory_walk_options_default(); options.recursive = recursive != 0; options.include_files = 1; options.include_directories = 0;
    context.sink = sink; context.user_data = user_data; context.report = out_report;
    return umi_directory_walk(root, &options, discovery_visit, &context);
}
