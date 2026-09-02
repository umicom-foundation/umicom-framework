/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/content_runtime.c
 *
 * PURPOSE:
 *   Implement transactional Framework component hosting for desktop windows.
 *   Applications contribute view factories and declarations; Framework owns
 *   lifecycle, validation, linked context and Federation Router dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/content_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/component_catalogue.h"

typedef struct UmiDesktopContentEntry {
    char window_id[UMI_DESKTOP_ID_CAPACITY];
    char capability_id[UMI_DESKTOP_ID_CAPACITY];
    int owns_window;
    int joined_context;
    uint64_t revision;
} UmiDesktopContentEntry;

struct UmiDesktopContentRuntime {
    UmiDesktopRuntime *desktop;
    UmiFederationRouter *federation;
    UmiUiComponentHostService *component_host;
    UmiDesktopContentEntry entries[UMI_DESKTOP_CONTENT_MAX];
    size_t count;
    uint64_t revision;
};

/* Provide the find entry operation used by this module and its client applications. */
static size_t find_entry(
    const UmiDesktopContentRuntime *runtime,
    const char *window_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || window_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(runtime->entries[index].window_id, window_id) == 0)
            return index;
    }
    return SIZE_MAX;
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    return (size_t)written < capacity ? UMI_STATUS_OK
                                      : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Provide the validate window operation used by this module and its client applications. */
static UmiStatus validate_window(
    const UmiDesktopContentRuntime *runtime,
    const UmiDesktopWindow *window,
    const UmiApplicationComponentDefinition **out_component)
{
    const UmiApplicationComponentDefinition *component;
    const UmiApplicationDefinition *application;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || window == NULL || out_component == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_component = NULL;
    component = umi_application_component_catalogue_find(window->component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (component == NULL) return UMI_STATUS_NOT_FOUND;
    application = umi_federation_router_find_application(
        runtime->federation, window->owner_application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_application_definition_uses_domain(application,
                                                component->domain_id))
        return UMI_STATUS_PERMISSION_DENIED;
    *out_component = component;
    return UMI_STATUS_OK;
}

/*
 * Provide the component instance exists operation used by this module and its client
 * applications.
 */
static int component_instance_exists(
    const UmiDesktopContentRuntime *runtime,
    const char *component_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->count; ++index) {
        const UmiDesktopWindow *window = umi_desktop_window_manager_find(
            umi_desktop_runtime_windows(runtime->desktop),
            runtime->entries[index].window_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (window != NULL && strcmp(window->component_id, component_id) == 0)
            return 1;
    }
    return 0;
}

/*
 * Provide the create host descriptor operation used by this module and its client
 * applications.
 */
static UmiStatus create_host_descriptor(
    const UmiDesktopWindow *window,
    const char *view_type,
    const char *view_id,
    UmiUiComponentHostDescriptor *out_descriptor)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL || view_type == NULL || view_id == NULL ||
        out_descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_descriptor, 0, sizeof(*out_descriptor));
    out_descriptor->structure_size = (uint32_t)sizeof(*out_descriptor);
    status = copy_text(out_descriptor->host_id,
                       sizeof(out_descriptor->host_id), window->window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(out_descriptor->component_id,
                           sizeof(out_descriptor->component_id),
                           window->component_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(out_descriptor->owner_application_id,
                           sizeof(out_descriptor->owner_application_id),
                           window->owner_application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(out_descriptor->view_type,
                           sizeof(out_descriptor->view_type), view_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(out_descriptor->view_id,
                           sizeof(out_descriptor->view_id), view_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(out_descriptor->title,
                           sizeof(out_descriptor->title), window->title);
    out_descriptor->visible = window->visible;
    return status;
}

/* Provide the add entry operation used by this module and its client applications. */
static UmiStatus add_entry(
    UmiDesktopContentRuntime *runtime,
    const UmiDesktopWindow *window,
    const UmiApplicationComponentDefinition *component,
    int owns_window,
    int joined_context)
{
    UmiDesktopContentEntry *entry;
    UmiStatus status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (runtime->count >= UMI_DESKTOP_CONTENT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &runtime->entries[runtime->count];
    (void)memset(entry, 0, sizeof(*entry));
    status = copy_text(entry->window_id, sizeof(entry->window_id),
                       window->window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(entry->capability_id,
                           sizeof(entry->capability_id),
                           component->capability_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    entry->owns_window = owns_window;
    entry->joined_context = joined_context;
    entry->revision = 1U;
    runtime->count += 1U;
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

/* Provide the join context operation used by this module and its client applications. */
static UmiStatus join_context(
    UmiDesktopContentRuntime *runtime,
    const UmiDesktopWindow *window,
    UmiUiWindowGroupRole role,
    int *out_joined)
{
    UmiStatus status;
    *out_joined = 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (window->context_group_id[0] == '\0') return UMI_STATUS_OK;
    status = umi_desktop_context_links_join(
        umi_desktop_runtime_context_links(runtime->desktop),
        window->context_group_id, window->window_id, role);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_ALREADY_EXISTS) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) *out_joined = 1;
    return status;
}

/* Provide the bind content operation used by this module and its client applications. */
static UmiStatus bind_content(
    UmiDesktopContentRuntime *runtime,
    const UmiDesktopWindow *window,
    const char *view_type,
    const char *view_id,
    UmiUiWindowGroupRole context_role,
    int owns_window)
{
    const UmiApplicationComponentDefinition *component;
    UmiUiComponentHostDescriptor descriptor;
    int joined_context = 0;
    UmiStatus status;
    status = validate_window(runtime, window, &component);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!component->multi_instance &&
        component_instance_exists(runtime, window->component_id))
        return UMI_STATUS_ALREADY_EXISTS;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_entry(runtime, window->window_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Apply this branch only when its contract condition is satisfied. */
    if (context_role < UMI_UI_WINDOW_GROUP_SOURCE ||
        context_role > UMI_UI_WINDOW_GROUP_BIDIRECTIONAL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = create_host_descriptor(window, view_type, view_id, &descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ui_component_host_mount(runtime->component_host,
                                             &descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = join_context(runtime, window, context_role, &joined_context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = add_entry(runtime, window, component, owns_window,
                           joined_context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (joined_context) {
            (void)umi_desktop_context_links_leave(
                umi_desktop_runtime_context_links(runtime->desktop),
                window->context_group_id, window->window_id);
        }
        (void)umi_ui_component_host_unmount(runtime->component_host,
                                            window->window_id);
    }
    return status;
}

/*
 * Initialise desktop content runtime from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_content_runtime_create(
    UmiDesktopRuntime *desktop,
    UmiFederationRouter *federation,
    UmiDesktopContentRuntime **out_runtime)
{
    UmiDesktopContentRuntime *runtime;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desktop == NULL || federation == NULL || out_runtime == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    runtime = (UmiDesktopContentRuntime *)calloc(1U, sizeof(*runtime));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    runtime->desktop = desktop;
    runtime->federation = federation;
    status = umi_ui_component_host_service_create(&runtime->component_host);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(runtime);
        return status;
    }
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by desktop content runtime so the same storage can be reused
 * safely.
 */
void umi_desktop_content_runtime_destroy(UmiDesktopContentRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return;
    umi_ui_component_host_service_destroy(runtime->component_host);
    free(runtime);
}

/*
 * Provide the desktop content runtime component host operation used by this module and its
 * client applications.
 */
UmiUiComponentHostService *umi_desktop_content_runtime_component_host(
    UmiDesktopContentRuntime *runtime)
{
    return runtime != NULL ? runtime->component_host : NULL;
}

/*
 * Provide the desktop content runtime view factories operation used by this module and its
 * client applications.
 */
UmiUiViewFactoryRegistry *umi_desktop_content_runtime_view_factories(
    UmiDesktopContentRuntime *runtime)
{
    return runtime != NULL
        ? umi_ui_component_host_view_factories(runtime->component_host)
        : NULL;
}

/*
 * Provide the desktop content runtime bind window operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_bind_window(
    UmiDesktopContentRuntime *runtime,
    const UmiDesktopContentBinding *binding)
{
    const UmiDesktopWindow *window;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || binding == NULL ||
        binding->structure_size < sizeof(*binding))
        return UMI_STATUS_INVALID_ARGUMENT;
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime->desktop), binding->window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    return bind_content(runtime, window, binding->view_type, binding->view_id,
                        binding->context_role, 0);
}

/*
 * Provide the desktop content runtime mount operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_mount(
    UmiDesktopContentRuntime *runtime,
    const UmiDesktopContentMountRequest *request)
{
    const UmiApplicationComponentDefinition *component;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || request == NULL ||
        request->structure_size < sizeof(*request))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!request->window.closable) return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_window(runtime, &request->window, &component);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)component;
    status = umi_desktop_runtime_open_window(runtime->desktop,
                                             &request->window);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = bind_content(runtime, &request->window, request->view_type,
                          request->view_id, request->context_role, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
        (void)umi_desktop_runtime_close_window(runtime->desktop,
                                               request->window.window_id);
    return status;
}

/*
 * Provide the desktop content runtime unmount operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_unmount(
    UmiDesktopContentRuntime *runtime,
    const char *window_id)
{
    UmiDesktopContentEntry entry;
    const UmiDesktopWindow *window;
    char context_group_id[UMI_DESKTOP_ID_CAPACITY] = {0};
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || window_id == NULL || window_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(runtime, window_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    entry = runtime->entries[index];
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime->desktop), window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window != NULL && window->context_group_id[0] != '\0')
        (void)snprintf(context_group_id, sizeof(context_group_id), "%s",
                       window->context_group_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (entry.owns_window) {
        status = umi_desktop_runtime_close_window(runtime->desktop, window_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (entry.joined_context && context_group_id[0] != '\0') {
        (void)umi_desktop_context_links_leave(
            umi_desktop_runtime_context_links(runtime->desktop),
            context_group_id, window_id);
    }
    status = umi_ui_component_host_unmount(runtime->component_host, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < runtime->count) {
        (void)memmove(&runtime->entries[index], &runtime->entries[index + 1U],
                      (runtime->count - index - 1U) *
                          sizeof(runtime->entries[0]));
    }
    runtime->count -= 1U;
    (void)memset(&runtime->entries[runtime->count], 0,
                 sizeof(runtime->entries[0]));
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop content runtime activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_activate(
    UmiDesktopContentRuntime *runtime,
    const char *window_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_component_host_activate(runtime->component_host, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Provide the desktop content runtime suspend operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_suspend(
    UmiDesktopContentRuntime *runtime,
    const char *window_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_component_host_suspend(runtime->component_host, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Provide the desktop content runtime resume operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_resume(
    UmiDesktopContentRuntime *runtime,
    const char *window_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_component_host_resume(runtime->component_host, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Provide the desktop content runtime show operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_show(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    int visible)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_runtime_show_window(
        runtime->desktop, window_id, visible != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ui_component_host_show(runtime->component_host,
                                            window_id, visible);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Provide the desktop content runtime refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_refresh(
    UmiDesktopContentRuntime *runtime,
    const char *window_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_component_host_refresh(runtime->component_host, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Perform desktop content runtime through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_desktop_content_runtime_dispatch(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    const UmiDesktopContentMessage *message,
    UmiFederationResponse *response)
{
    UmiFederationRequest request;
    const UmiDesktopWindow *window;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || message == NULL ||
        message->structure_size < sizeof(*message))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(runtime, window_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime->desktop), window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_INVALID_STATE;
    (void)memset(&request, 0, sizeof(request));
    request.structure_size = (uint32_t)sizeof(request);
    request.message_id = message->message_id;
    request.source_application_id = window->owner_application_id;
    request.capability_id = runtime->entries[index].capability_id;
    request.operation_id = message->operation_id;
    request.schema_id = message->schema_id;
    request.correlation_id = message->correlation_id;
    request.kind = message->kind;
    request.payload = message->payload;
    request.payload_size = message->payload_size;
    return umi_federation_router_dispatch(runtime->federation, &request,
                                          response);
}

/*
 * Provide the desktop content runtime publish context operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_content_runtime_publish_context(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id)
{
    const UmiDesktopWindow *window;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || find_entry(runtime, window_id) == SIZE_MAX)
        return runtime == NULL ? UMI_STATUS_INVALID_ARGUMENT
                               : UMI_STATUS_NOT_FOUND;
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime->desktop), window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (window->context_group_id[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return umi_desktop_context_links_publish(
        umi_desktop_runtime_context_links(runtime->desktop),
        window->context_group_id, window->owner_application_id, context_type,
        subject_id, correlation_id);
}

/*
 * Provide the context member role operation used by this module and its client
 * applications.
 */
static UmiUiWindowGroupRole context_member_role(
    const UmiDesktopContextLinks *links,
    const char *group_id,
    const char *window_id)
{
    const UmiUiWindowGroup *group;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (links == NULL || group_id == NULL || group_id[0] == '\0' ||
        window_id == NULL) return 0;
    group = umi_ui_window_group_find(&links->groups, group_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (group == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < group->member_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(group->members[index].window_id, window_id) == 0)
            return group->members[index].role;
    }
    return 0;
}

/*
 * Provide the desktop content runtime link context operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_link_context(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    const char *group_id,
    UmiUiWindowGroupRole role)
{
    UmiDesktopContextLinks *links;
    const UmiDesktopWindow *window;
    char previous_group[UMI_DESKTOP_ID_CAPACITY] = {0};
    UmiUiWindowGroupRole previous_role;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || window_id == NULL || window_id[0] == '\0' ||
        group_id == NULL || group_id[0] == '\0' ||
        role < UMI_UI_WINDOW_GROUP_SOURCE ||
        role > UMI_UI_WINDOW_GROUP_BIDIRECTIONAL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(runtime, window_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    links = umi_desktop_runtime_context_links(runtime->desktop);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ui_window_group_find(&links->groups, group_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime->desktop), window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_INVALID_STATE;
    (void)snprintf(previous_group, sizeof(previous_group), "%s",
                   window->context_group_id);
    previous_role = context_member_role(links, previous_group, window_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (previous_group[0] != '\0') {
        status = umi_desktop_context_links_leave(
            links, previous_group, window_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND)
            return status;
    }
    status = umi_desktop_runtime_set_window_context_group(
        runtime->desktop, window_id, group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_context_links_join(
            links, group_id, window_id, role);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)umi_desktop_runtime_set_window_context_group(
            runtime->desktop, window_id, previous_group);
        /* Apply this branch only when its contract condition is satisfied. */
        if (previous_group[0] != '\0' && previous_role != 0) {
            (void)umi_desktop_context_links_join(
                links, previous_group, window_id, previous_role);
        }
        return status;
    }
    runtime->entries[index].joined_context = 1;
    runtime->entries[index].revision += 1U;
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop content runtime snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_snapshot(
    const UmiDesktopContentRuntime *runtime,
    const char *window_id,
    UmiDesktopContentSnapshot *out_snapshot)
{
    const UmiDesktopWindow *window;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || window_id == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(runtime, window_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime->desktop), window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_INVALID_STATE;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->window = *window;
    status = umi_ui_component_host_snapshot(runtime->component_host,
                                            window_id, &out_snapshot->host);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_snapshot->capability_id,
                       sizeof(out_snapshot->capability_id),
                       runtime->entries[index].capability_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->owns_window = runtime->entries[index].owns_window;
    out_snapshot->joined_context = runtime->entries[index].joined_context;
    out_snapshot->revision = runtime->entries[index].revision +
                             out_snapshot->host.revision;
    return UMI_STATUS_OK;
}

/*
 * Find desktop content runtime while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_desktop_content_runtime_at(
    const UmiDesktopContentRuntime *runtime,
    size_t index,
    UmiDesktopContentSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= runtime->count) return UMI_STATUS_NOT_FOUND;
    return umi_desktop_content_runtime_snapshot(
        runtime, runtime->entries[index].window_id, out_snapshot);
}

/*
 * Provide the desktop content runtime presentation operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_presentation(
    const UmiDesktopContentRuntime *runtime,
    const char *window_id,
    UmiUiViewPresentation *out_presentation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ui_component_host_presentation(runtime->component_host,
                                              window_id, out_presentation);
}

/*
 * Return the number of records represented by desktop content runtime without changing
 * their state.
 */
size_t umi_desktop_content_runtime_count(
    const UmiDesktopContentRuntime *runtime)
{
    return runtime != NULL ? runtime->count : 0U;
}
