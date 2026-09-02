/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/discovery.c
 *
 * PURPOSE:
 *   Load validated .umilayout files into live template and reflection
 *   catalogues so applications can add layouts without being rebuilt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/workbench_layout/discovery.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/workbench_layout/json.h"

#define UMI_WORKBENCH_LAYOUT_DISCOVERY_MAXIMUM_BYTES (4U * 1024U * 1024U)

typedef struct UmiWorkbenchLayoutDiscoveryContext {
    UmiWorkbenchLayoutTemplateRegistry *templates;
    UmiReflectionEngine *reflection;
    const UmiWorkbenchLayoutDiscoveryConfig *config;
    UmiWorkbenchLayoutDiscoveryReport *report;
} UmiWorkbenchLayoutDiscoveryContext;

/* Copy text and report truncation instead of publishing an ambiguous identity. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    int written;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    if (written < 0 || (size_t)written >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Match the portable layout suffix without depending on filesystem case rules. */
static int has_layout_suffix(const char *name)
{
    static const char suffix[] = ".umilayout";
    size_t name_length;
    size_t suffix_length = sizeof(suffix) - 1U;
    size_t index;

    if (name == NULL) {
        return 0;
    }
    name_length = strlen(name);
    if (name_length < suffix_length) {
        return 0;
    }
    for (index = 0U; index < suffix_length; ++index) {
        if (tolower((unsigned char)name[name_length - suffix_length + index]) !=
            tolower((unsigned char)suffix[index])) {
            return 0;
        }
    }
    return 1;
}

/* Convert one validated document into catalogue metadata without copying widgets. */
static UmiStatus prepare_template(
    UmiWorkbenchLayoutTemplate *layout_template,
    const char *path,
    const UmiWorkbenchLayoutDiscoveryConfig *config,
    UmiReflectionResourceDescriptor *resource)
{
    const UmiWorkbenchLayoutDocument *document = &layout_template->document;
    const char *module_id = document->identity.owner_application_id[0] != '\0'
        ? document->identity.owner_application_id
        : config->default_module_id;
    UmiStatus status;

    layout_template->structure_size = (uint32_t)sizeof(*layout_template);
    status = copy_text(layout_template->template_id,
                       sizeof(layout_template->template_id),
                       document->identity.layout_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(layout_template->display_name,
                           sizeof(layout_template->display_name),
                           document->name);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(layout_template->category,
                           sizeof(layout_template->category),
                           document->category[0] != '\0'
                               ? document->category
                               : "custom");
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(layout_template->owner_application_id,
                           sizeof(layout_template->owner_application_id),
                           module_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(layout_template->description,
                           sizeof(layout_template->description),
                           document->description);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    layout_template->tag_count = document->tag_count;
    if (document->tag_count > 0U) {
        (void)memcpy(layout_template->tags,
                     document->tags,
                     document->tag_count * sizeof(*layout_template->tags));
    }
    layout_template->built_in = false;
    layout_template->recommended = false;
    layout_template->revision = document->version.revision != 0U
        ? document->version.revision
        : 1U;

    (void)memset(resource, 0, sizeof(*resource));
    resource->structure_size = (uint32_t)sizeof(*resource);
    resource->kind = UMI_REFLECTION_RESOURCE_LAYOUT;
    resource->flags = document->flags;
    resource->content_hash = document->content_hash;
    resource->revision = layout_template->revision;
    status = copy_text(resource->resource_id,
                       sizeof(resource->resource_id),
                       layout_template->template_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(resource->module_id,
                           sizeof(resource->module_id),
                           module_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(resource->display_name,
                           sizeof(resource->display_name),
                           layout_template->display_name);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(resource->description,
                           sizeof(resource->description),
                           layout_template->description);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(resource->resource_uri,
                           sizeof(resource->resource_uri),
                           path);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(resource->media_type,
                           sizeof(resource->media_type),
                           "umicom.layout");
    }
    return status;
}

UmiWorkbenchLayoutDiscoveryConfig
umi_workbench_layout_discovery_config_default(void)
{
    UmiWorkbenchLayoutDiscoveryConfig config;

    (void)memset(&config, 0, sizeof(config));
    config.structure_size = (uint32_t)sizeof(config);
    config.recursive = 1;
    config.maximum_file_bytes =
        UMI_WORKBENCH_LAYOUT_DISCOVERY_MAXIMUM_BYTES;
    (void)snprintf(config.default_module_id,
                   sizeof(config.default_module_id),
                   "%s",
                   "org.umicom.desktop");
    return config;
}

UmiStatus umi_workbench_layout_discovery_load_file(
    UmiWorkbenchLayoutTemplateRegistry *templates,
    UmiReflectionEngine *reflection,
    const char *path,
    const UmiWorkbenchLayoutDiscoveryConfig *config,
    int *out_replaced)
{
    UmiWorkbenchLayoutDiscoveryConfig effective;
    UmiWorkbenchLayoutTemplate *candidate;
    UmiWorkbenchLayoutTemplate *previous;
    const UmiWorkbenchLayoutTemplate *existing;
    UmiReflectionResourceDescriptor resource;
    UmiWorkbenchLayoutJsonResult json_result;
    UmiFileInfo file_info;
    char *json = NULL;
    size_t json_size = 0U;
    UmiStatus status;
    int replaced = 0;

    if (templates == NULL || reflection == NULL || path == NULL ||
        path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (config != NULL && config->structure_size < sizeof(*config)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = config != NULL
        ? *config
        : umi_workbench_layout_discovery_config_default();
    if (effective.maximum_file_bytes == 0U ||
        memchr(effective.default_module_id,
               '\0',
               sizeof(effective.default_module_id)) == NULL ||
        effective.default_module_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_directory_stat(path, &file_info);
    if (status != UMI_STATUS_OK || file_info.kind != UMI_FILE_KIND_REGULAR) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
    }
    if (file_info.size > (uint64_t)effective.maximum_file_bytes) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_fs_read_text(path, &json, &json_size);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Recheck the measured content after reading because another process may
     * replace or enlarge a file between the metadata check and the read. */
    if (json_size > effective.maximum_file_bytes) {
        umi_fs_free_text(json);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Large documents live on the heap to protect application thread stacks. */
    candidate = (UmiWorkbenchLayoutTemplate *)calloc(1U, sizeof(*candidate));
    previous = (UmiWorkbenchLayoutTemplate *)calloc(1U, sizeof(*previous));
    if (candidate == NULL || previous == NULL) {
        umi_fs_free_text(json);
        free(candidate);
        free(previous);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = umi_workbench_layout_json_decode(
        json, json_size, &candidate->document, &json_result);
    umi_fs_free_text(json);
    if (status == UMI_STATUS_OK) {
        status = prepare_template(candidate, path, &effective, &resource);
    }
    if (status != UMI_STATUS_OK) {
        free(candidate);
        free(previous);
        return status;
    }

    existing = umi_workbench_layout_template_registry_find(
        templates, candidate->template_id);
    if (existing != NULL) {
        *previous = *existing;
        replaced = 1;
        status = umi_workbench_layout_template_registry_remove(
            templates, candidate->template_id);
        if (status != UMI_STATUS_OK) {
            free(candidate);
            free(previous);
            return status;
        }
    }
    status = umi_workbench_layout_template_registry_add(templates, candidate);
    if (status != UMI_STATUS_OK) {
        if (replaced != 0) {
            (void)umi_workbench_layout_template_registry_add(
                templates, previous);
        }
        free(candidate);
        free(previous);
        return status;
    }

    status = umi_reflection_engine_upsert_resource(
        reflection, &resource, NULL);
    if (status != UMI_STATUS_OK) {
        (void)umi_workbench_layout_template_registry_remove(
            templates, candidate->template_id);
        if (replaced != 0) {
            (void)umi_workbench_layout_template_registry_add(
                templates, previous);
        }
        free(candidate);
        free(previous);
        return status;
    }
    if (out_replaced != NULL) {
        *out_replaced = replaced;
    }
    free(candidate);
    free(previous);
    return UMI_STATUS_OK;
}

/* Process one directory entry while keeping invalid layout files isolated. */
static UmiStatus discovery_visit(const UmiFileInfo *info, void *user_data)
{
    UmiWorkbenchLayoutDiscoveryContext *context =
        (UmiWorkbenchLayoutDiscoveryContext *)user_data;
    UmiStatus status;
    int replaced = 0;

    context->report->files_examined += 1U;
    if (info->kind != UMI_FILE_KIND_REGULAR ||
        !has_layout_suffix(info->name)) {
        return UMI_STATUS_OK;
    }
    status = umi_workbench_layout_discovery_load_file(
        context->templates,
        context->reflection,
        info->path,
        context->config,
        &replaced);
    if (status != UMI_STATUS_OK) {
        context->report->invalid_layouts += 1U;
        return UMI_STATUS_OK;
    }
    context->report->layouts_loaded += 1U;
    if (replaced != 0) {
        context->report->layouts_refreshed += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_discover(
    UmiWorkbenchLayoutTemplateRegistry *templates,
    UmiReflectionEngine *reflection,
    const char *root,
    const UmiWorkbenchLayoutDiscoveryConfig *config,
    UmiWorkbenchLayoutDiscoveryReport *out_report)
{
    UmiWorkbenchLayoutDiscoveryConfig effective;
    UmiDirectoryWalkOptions options;
    UmiWorkbenchLayoutDiscoveryContext context;

    if (templates == NULL || reflection == NULL || root == NULL ||
        root[0] == '\0' || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (config != NULL && config->structure_size < sizeof(*config)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = config != NULL
        ? *config
        : umi_workbench_layout_discovery_config_default();
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->structure_size = (uint32_t)sizeof(*out_report);
    options = umi_directory_walk_options_default();
    options.recursive = effective.recursive != 0;
    options.include_files = 1;
    options.include_directories = 0;
    options.follow_symbolic_links = 0;
    context.templates = templates;
    context.reflection = reflection;
    context.config = &effective;
    context.report = out_report;
    return umi_directory_walk(root, &options, discovery_visit, &context);
}
