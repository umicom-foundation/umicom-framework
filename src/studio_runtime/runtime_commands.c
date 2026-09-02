/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/runtime_commands.c
 *
 * PURPOSE:
 *   Implement real Studio composition commands in UmiCommandRegistry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/runtime_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum StudioCommandKind {
    STUDIO_COMMAND_REFRESH = 1,
    STUDIO_COMMAND_LAYOUT_MANAGER = 2,
    STUDIO_COMMAND_LAYOUT_APPLY = 3,
    STUDIO_COMMAND_LAYOUT_SAVE = 4,
    STUDIO_COMMAND_LAYOUT_RESTORE = 5,
    STUDIO_COMMAND_COMMAND_SEARCH = 6,
    STUDIO_COMMAND_CLOSE_CHECK = 7,
    STUDIO_COMMAND_CONTRACT_CHECK = 8
} StudioCommandKind;

typedef struct StudioCommandDefinition {
    const char *command_id;
    const char *title;
    const char *description;
    StudioCommandKind kind;
    const char *fixed_argument;
    int mutates_state;
} StudioCommandDefinition;

typedef struct StudioCommandBinding {
    struct UmiStudioRuntimeCommandService *owner;
    const StudioCommandDefinition *definition;
} StudioCommandBinding;

struct UmiStudioRuntimeCommandService {
    UmiStudioRuntimePlatform *platform;
    UmiStudioRuntimeSessionController *session;
    StudioCommandBinding bindings[32];
    size_t registered_count;
};

static const StudioCommandDefinition COMMANDS[] = {
    {"studio.refresh", "Refresh Studio State",
     "Refresh Framework Studio composition state.",
     STUDIO_COMMAND_REFRESH, "", 1},
    {"studio.layout.manager", "Layout Manager",
     "Focus the Framework Layout Manager.",
     STUDIO_COMMAND_LAYOUT_MANAGER, "", 1},
    {"studio.layout.default", "Default Development Layout",
     "Apply the default development layout.",
     STUDIO_COMMAND_LAYOUT_APPLY, "umicom.studio.layout.default", 1},
    {"studio.layout.debugging", "Debugging Layout",
     "Apply the debugging layout.",
     STUDIO_COMMAND_LAYOUT_APPLY, "umicom.studio.layout.debugging", 1},
    {"studio.layout.testing", "Testing Layout",
     "Apply the testing layout.",
     STUDIO_COMMAND_LAYOUT_APPLY, "umicom.studio.layout.testing", 1},
    {"studio.layout.source-control", "Source Control Layout",
     "Apply the source-control layout.",
     STUDIO_COMMAND_LAYOUT_APPLY, "umicom.studio.layout.source-control", 1},
    {"studio.layout.ai-development", "AI Development Layout",
     "Apply the AI development layout.",
     STUDIO_COMMAND_LAYOUT_APPLY, "umicom.studio.layout.ai-development", 1},
    {"studio.layout.review", "AI Patch Review Layout",
     "Apply the AI patch-review layout.",
     STUDIO_COMMAND_LAYOUT_APPLY, "umicom.studio.layout.review", 1},
    {"studio.layout.zen", "Zen Coding Layout",
     "Apply the distraction-free editor layout.",
     STUDIO_COMMAND_LAYOUT_APPLY, "umicom.studio.layout.zen", 1},
    {"studio.layout.learning", "C Development and Learning Layout",
     "Place lessons and documentation beside the C development tools.",
     STUDIO_COMMAND_LAYOUT_APPLY, "umicom.studio.layout.learning", 1},
    {"studio.layout.visual-design", "Visual Application Design Layout",
     "Place the design canvas beside components, structure and properties.",
     STUDIO_COMMAND_LAYOUT_APPLY, "umicom.studio.layout.visual-design", 1},
    {"studio.layout.save", "Save Layout",
     "Persist the semantic Studio layout/session.",
     STUDIO_COMMAND_LAYOUT_SAVE, "", 1},
    {"studio.layout.restore", "Restore Layout",
     "Restore the semantic Studio layout/session.",
     STUDIO_COMMAND_LAYOUT_RESTORE, "", 1},
    {"studio.command.search", "Search Commands and Menus",
     "Search runtime commands and shell menu contributions.",
     STUDIO_COMMAND_COMMAND_SEARCH, "", 0},
    {"studio.close.check", "Check Close Safety",
     "Evaluate dirty documents and running developer operations.",
     STUDIO_COMMAND_CLOSE_CHECK, "", 0},
    {"studio.contract.check", "Check Studio Contract",
     "Verify thin-Studio surface and command closure.",
     STUDIO_COMMAND_CONTRACT_CHECK, "", 0}
};


/*
 * Return the number of records represented by studio runtime command without changing
 * their state.
 */
size_t umi_studio_runtime_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

/*
 * Find studio runtime command id while leaving the underlying catalogue or model owned by
 * this module.
 */
const char *umi_studio_runtime_command_id_at(size_t index)
{
    return index < umi_studio_runtime_command_count()
        ? COMMANDS[index].command_id
        : NULL;
}

/*
 * Provide the focus layout manager operation used by this module and its client
 * applications.
 */
static UmiStatus focus_layout_manager(
    UmiStudioRuntimeCommandService *service)
{
    UmiStudioRuntimeBindings *bindings =
        umi_studio_runtime_platform_bindings(service->platform);
    UmiApplicationShellContribution contribution;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_application_shell_registry_find(
        bindings->shell_registry,
        "umicom.shell.layout-manager.manager",
        &contribution);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_application_shell_registry_set_state(
        bindings->shell_registry,
        contribution.contribution_id,
        1,
        1,
        0,
        contribution.badge_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_application_shell_state_activate(
        bindings->shell_state,
        "",
        contribution.contribution_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_application_shell_state_focus(
        bindings->shell_state,
        contribution.contribution_id);
}

/* Provide the execute operation used by this module and its client applications. */
static UmiStatus execute(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    StudioCommandBinding *binding = (StudioCommandBinding *)user_data;
    UmiStudioRuntimeCommandService *service;
    const StudioCommandDefinition *definition;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->owner == NULL ||
        binding->definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    service = binding->owner;
    definition = binding->definition;

    /* Select the behaviour associated with the requested command or state value. */
    switch (definition->kind) {
        case STUDIO_COMMAND_REFRESH:
            status = umi_studio_runtime_platform_refresh(service->platform);
            break;

        case STUDIO_COMMAND_LAYOUT_MANAGER:
            status = focus_layout_manager(service);
            break;

        case STUDIO_COMMAND_LAYOUT_APPLY:
            status = umi_studio_runtime_platform_select_layout(
                service->platform,
                definition->fixed_argument);
            break;

        case STUDIO_COMMAND_LAYOUT_SAVE:
            status = service->session != NULL
                ? umi_studio_session_controller_save(
                    service->session,
                    service->platform)
                : UMI_STATUS_INVALID_STATE;
            break;

        case STUDIO_COMMAND_LAYOUT_RESTORE: {
            int restored = 0;
            status = service->session != NULL
                ? umi_studio_session_controller_restore(
                    service->session,
                    service->platform,
                    &restored)
                : UMI_STATUS_INVALID_STATE;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK && !restored) {
                status = UMI_STATUS_NOT_FOUND;
            }
            break;
        }

        case STUDIO_COMMAND_COMMAND_SEARCH: {
            UmiStudioRuntimeCommandSearchResults results;
            UmiStudioRuntimeBindings *bindings =
                umi_studio_runtime_platform_bindings(service->platform);

            status = umi_studio_command_search(
                bindings,
                argument != NULL ? argument : "",
                &results);

            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK &&
                out_message != NULL && message_capacity > 0U) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (results.count > 0U) {
                    (void)snprintf(
                        out_message,
                        message_capacity,
                        "%zu matches; best: %s — %s%s",
                        results.count,
                        results.items[0].title,
                        results.items[0].command_id,
                        results.items[0].enabled ? "" : " (disabled)");
                } /* Use this fallback path when the earlier condition does not apply. */ else {
                    (void)snprintf(
                        out_message,
                        message_capacity,
                        "No matching commands.");
                }
            }
            return status;
        }

        case STUDIO_COMMAND_CLOSE_CHECK: {
            UmiStudioRuntimeBindings *bindings =
                umi_studio_runtime_platform_bindings(service->platform);
            UmiStudioRuntimeSnapshot snapshot;
            UmiStudioRuntimeCloseReport report;

            status = umi_studio_runtime_platform_snapshot(
                service->platform,
                &snapshot);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                status = umi_studio_close_guard_evaluate(
                    bindings->documents,
                    &snapshot.ide.context,
                    &report);
            }

            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK &&
                out_message != NULL && message_capacity > 0U) {
                (void)snprintf(
                    out_message,
                    message_capacity,
                    "%s",
                    report.summary);
            }
            return status;
        }

        case STUDIO_COMMAND_CONTRACT_CHECK: {
            UmiStudioRuntimeBindings *bindings =
                umi_studio_runtime_platform_bindings(service->platform);
            UmiStudioRuntimeClosureReport report;

            status = umi_studio_runtime_closure_check(
                umi_studio_runtime_contract(),
                bindings->shell_registry,
                bindings->commands,
                &report);

            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK &&
                out_message != NULL && message_capacity > 0U) {
                (void)snprintf(
                    out_message,
                    message_capacity,
                    "Studio contract %s; surfaces %zu/%zu; missing commands %zu.",
                    report.ready ? "ready" : "incomplete",
                    report.installed_surface_count,
                    umi_studio_runtime_contract()->required_surface_count,
                    report.missing_command_count);
            }
            return status;
        }

        default:
            status = UMI_STATUS_NOT_IMPLEMENTED;
            break;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s: %s",
            definition->title,
            status == UMI_STATUS_OK ? "completed" : "failed");
    }

    return status;
}

/* Provide the enabled operation used by this module and its client applications. */
static int enabled(void *user_data, const char *argument)
{
    StudioCommandBinding *binding = (StudioCommandBinding *)user_data;
    (void)argument;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->owner == NULL ||
        binding->definition == NULL) {
        return 0;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((binding->definition->kind == STUDIO_COMMAND_LAYOUT_SAVE ||
         binding->definition->kind == STUDIO_COMMAND_LAYOUT_RESTORE) &&
        binding->owner->session == NULL) {
        return 0;
    }

    return 1;
}

/*
 * Initialise studio runtime command service from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_studio_runtime_command_service_create(
    UmiStudioRuntimePlatform *platform,
    UmiStudioRuntimeSessionController *session,
    UmiStudioRuntimeCommandService **out_service)
{
    UmiStudioRuntimeCommandService *service;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_service = NULL;

    service = (UmiStudioRuntimeCommandService *)calloc(
        1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    service->platform = platform;
    service->session = session;
    *out_service = service;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by studio runtime command service so the same storage can be
 * reused safely.
 */
void umi_studio_runtime_command_service_destroy(
    UmiStudioRuntimeCommandService *service)
{
    free(service);
}


/*
 * Provide the studio runtime command service set session operation used by this module and
 * its client applications.
 */
UmiStatus umi_studio_runtime_command_service_set_session(
    UmiStudioRuntimeCommandService *service,
    UmiStudioRuntimeSessionController *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->session = session;
    return UMI_STATUS_OK;
}

/*
 * Add studio runtime command service only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_studio_runtime_command_service_register(
    UmiStudioRuntimeCommandService *service)
{
    UmiStudioRuntimeBindings *bindings;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    bindings = umi_studio_runtime_platform_bindings(service->platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || bindings->commands == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(COMMANDS) / sizeof(COMMANDS[0]); ++index) {
        UmiCommandDescriptor descriptor;
        UmiStatus status;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_command_registry_contains(
                bindings->commands,
                COMMANDS[index].command_id)) {
            continue;
        }

        service->bindings[index].owner = service;
        service->bindings[index].definition = &COMMANDS[index];

        (void)memset(&descriptor, 0, sizeof(descriptor));
        descriptor.structure_size = (uint32_t)sizeof(descriptor);
        descriptor.command_id = COMMANDS[index].command_id;
        descriptor.title = COMMANDS[index].title;
        descriptor.category = "Studio";
        descriptor.description = COMMANDS[index].description;
        descriptor.required_permission =
            COMMANDS[index].mutates_state
                ? "studio.control"
                : "studio.read";
        descriptor.flags =
            COMMANDS[index].mutates_state
                ? UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED
                : UMI_COMMAND_NONE;
        descriptor.handler = execute;
        descriptor.enabled = enabled;
        descriptor.user_data = &service->bindings[index];

        status = umi_command_registry_register(
            bindings->commands,
            &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        service->registered_count += 1U;
    }

    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by studio runtime command service registered
 * without changing their state.
 */
size_t umi_studio_runtime_command_service_registered_count(
    const UmiStudioRuntimeCommandService *service)
{
    return service != NULL ? service->registered_count : 0U;
}
