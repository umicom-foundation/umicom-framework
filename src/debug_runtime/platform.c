/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/platform.c
 *
 * PURPOSE:
 *   Implement the operational DAP debugger runtime on top of the established
 *   Debug Service, Advanced Debug Platform and stable adapter C ABI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/platform.h"

#include "umicom/base/text.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/debug_runtime/requests/requests.h"

struct UmiDebugRuntimePlatform {
    UmiDebugService *service;
    UmiDebugAdvancedPlatform *advanced;
    UmiDebugRuntimeContractAdapter *contract_owner;
    UmiDebugAdapterDescriptor descriptor;
    UmiDebugRuntimeAdapter *adapter;
    UmiDebugRuntimeServiceBridge bridge;
    UmiDebugRuntimeCapabilities capabilities;
    uint64_t capability_bits;
    char active_session_id[128];
    char active_configuration_id[128];
    char active_profile_id[128];
    uint64_t active_thread_id;
    uint64_t active_frame_id;
    uint64_t revision;
    int active;
    int initialized;
    int attached;
    int paused;
};

/*
 * Provide the project memory view operation used by this module and its client
 * applications.
 */
static UmiStatus project_memory_view(
    UmiDebugRuntimePlatform *platform,
    const char *memory_reference,
    int64_t offset,
    uint32_t requested_count,
    const UmiDebugRuntimeMemoryResult *result);

/*
 * Provide the project disassembly view operation used by this module and its client
 * applications.
 */
static UmiStatus project_disassembly_view(
    UmiDebugRuntimePlatform *platform,
    const UmiDebugRuntimeDisassembly *result);

/* Find profile while leaving the underlying catalogue or model owned by this module. */
static UmiStatus profile_find(
    UmiDebugRuntimePlatform *platform,
    const char *profile_id,
    UmiDebugAdapterProfile *out_profile)
{
    UmiStatus status;
    const UmiDebugAdapterProfile *builtin;

    status = umi_debug_adapter_profile_registry_find(
        umi_debug_service_adapter_profiles(platform->service),
        profile_id,
        out_profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) return status;

    builtin = umi_debug_runtime_builtin_profile_find(profile_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builtin == NULL) {
        builtin = umi_debug_runtime_builtin_profile_for_kind(profile_id);
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builtin == NULL) return UMI_STATUS_NOT_FOUND;
    *out_profile = *builtin;
    return UMI_STATUS_OK;
}

/*
 * Initialise debug runtime platform from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_runtime_platform_create(
    UmiDebugRuntimePlatform **out_platform)
{
    UmiDebugRuntimePlatform *platform;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_platform = NULL;

    platform = (UmiDebugRuntimePlatform *)calloc(1U, sizeof(*platform));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_debug_service_create(&platform->service);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_debug_runtime_register_builtin_profiles(
            platform->service);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_debug_advanced_platform_create(
            platform->service,
            &platform->advanced);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_debug_runtime_platform_destroy(platform);
        return status;
    }

    platform->revision = 1U;
    *out_platform = platform;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug runtime platform so the same storage can be reused
 * safely.
 */
void umi_debug_runtime_platform_destroy(
    UmiDebugRuntimePlatform *platform)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return;

    /* Apply this operation only while the related capability or state is available. */
    if (platform->active) {
        (void)umi_debug_runtime_platform_stop(
            platform, 1, 100U);
    }

    umi_debug_advanced_platform_destroy(platform->advanced);
    platform->advanced = NULL;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform->contract_owner != NULL) {
        umi_debug_runtime_contract_adapter_destroy(
            platform->contract_owner);
        platform->contract_owner = NULL;
        platform->adapter = NULL;
    }

    umi_debug_service_destroy(platform->service);
    platform->service = NULL;
    free(platform);
}

/*
 * Provide the build default launch arguments operation used by this module and its client
 * applications.
 */
static UmiStatus build_default_launch_arguments(
    UmiDebugRuntimePlatform *platform,
    const char *configuration_id,
    int attach,
    char *out_json,
    size_t capacity)
{
    UmiDebugLaunchConfigurationSnapshot configuration;
    UmiLanguageRuntimeArguments arguments;
    UmiLanguageRuntimeJsonWriter writer;
    size_t index;
    UmiStatus status;

    status = umi_debug_launch_configuration_registry_find(
        umi_debug_service_launch_configuration(platform->service),
        configuration_id,
        &configuration);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_language_runtime_arguments_parse(
        configuration.arguments,
        &arguments);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_language_runtime_json_writer_init(&writer, out_json, capacity);
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"name\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, configuration.name);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"request\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, attach ? "attach" : "launch");
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"program\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, configuration.program);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"cwd\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, configuration.working_directory);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"stopOnEntry\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, configuration.stop_on_entry);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"args\":[");

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < arguments.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index > 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }
        (void)umi_language_runtime_json_writer_string(
            &writer, arguments.values[index]);
    }

    (void)umi_language_runtime_json_writer_raw(&writer, "]}");
    return writer.status;
}

/*
 * Provide the publish event envelope operation used by this module and its client
 * applications.
 */
static UmiStatus publish_event_envelope(
    UmiDebugRuntimePlatform *platform,
    const UmiDebugRuntimeEnvelope *envelope)
{
    UmiDebugRuntimeEvent event;
    UmiStatus status;

    status = umi_debug_runtime_decode_event(envelope->json, &event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_publish_event(&platform->bridge, &event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(event.event, "initialized") == 0) {
        platform->initialized = 1;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(event.event, "stopped") == 0) {
        platform->paused = 1;
        platform->active_thread_id = event.thread_id;
        (void)umi_debug_runtime_adapter_set_state(
            platform->adapter,
            UMI_DEBUG_RUNTIME_ADAPTER_PAUSED);
        (void)umi_debug_inspection_session_set_state(
            umi_debug_advanced_platform_inspection(platform->advanced),
            UMI_DEBUG_INSPECTION_PAUSED);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(event.event, "continued") == 0) {
        platform->paused = 0;
        (void)umi_debug_runtime_adapter_set_state(
            platform->adapter,
            UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
        (void)umi_debug_inspection_session_set_state(
            umi_debug_advanced_platform_inspection(platform->advanced),
            UMI_DEBUG_INSPECTION_RUNNING);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(event.event, "terminated") == 0 ||
               strcmp(event.event, "exited") == 0) {
        platform->paused = 0;
        (void)umi_debug_runtime_adapter_set_state(
            platform->adapter,
            UMI_DEBUG_RUNTIME_ADAPTER_STOPPED);
    }

    platform->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the initialize adapter operation used by this module and its client
 * applications.
 */
static UmiStatus initialize_adapter(
    UmiDebugRuntimePlatform *platform,
    const UmiDebugAdapterProfile *profile,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    uint64_t sequence = 0U;
    UmiStatus status;

    (void)umi_debug_runtime_adapter_set_state(
        platform->adapter,
        UMI_DEBUG_RUNTIME_ADAPTER_INITIALIZING);

    status = umi_debug_runtime_request_initialize(
        platform->adapter,
        profile->debugger_kind,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_initialize(
        response.json,
        &platform->capabilities);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    platform->capability_bits =
        umi_debug_runtime_capability_bits(&platform->capabilities);

    return umi_debug_runtime_adapter_set_state(
        platform->adapter,
        UMI_DEBUG_RUNTIME_ADAPTER_READY);
}


/* Provide the register contract operation used by this module and its client applications. */
static UmiStatus register_contract(
    UmiDebugRuntimePlatform *platform,
    const UmiDebugAdapterProfile *profile,
    const char *session_id,
    uint32_t timeout_ms)
{
    char descriptor_id[128];
    int written;
    UmiStatus status;

    written = snprintf(
        descriptor_id,
        sizeof(descriptor_id),
        "dap.%s",
        session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(descriptor_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_debug_runtime_contract_adapter_create(
        platform->adapter,
        descriptor_id,
        profile->display_name,
        profile->debugger_kind,
        platform->capability_bits,
        timeout_ms,
        &platform->contract_owner,
        &platform->descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_advanced_platform_register_adapter(
        platform->advanced,
        &platform->descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_debug_runtime_contract_adapter_destroy(
            platform->contract_owner);
        platform->contract_owner = NULL;
        platform->adapter = NULL;
        return status;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the wait launch and initialized operation used by this module and its client
 * applications.
 */
static UmiStatus wait_launch_and_initialized(
    UmiDebugRuntimePlatform *platform,
    uint64_t launch_sequence,
    uint32_t timeout_ms)
{
    int launch_response_received = 0;
    int configuration_sent = 0;
    uint32_t attempts = 0U;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (attempts < 256U &&
           (!launch_response_received ||
            (platform->capabilities.supports_configuration_done &&
             !configuration_sent))) {
        UmiDebugRuntimeEnvelope envelope;

        status = umi_debug_runtime_adapter_receive(
            platform->adapter,
            timeout_ms,
            &envelope);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) {
            attempts += 1U;
            continue;
        }

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Apply this branch only when its contract condition is satisfied. */
        if (envelope.kind == UMI_DEBUG_RUNTIME_MESSAGE_EVENT) {
            status = publish_event_envelope(platform, &envelope);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;

            /* Apply this branch only when its contract condition is satisfied. */
            if (platform->initialized &&
                platform->capabilities.supports_configuration_done &&
                !configuration_sent) {
                uint64_t configuration_sequence = 0U;
                UmiDebugRuntimeEnvelope configuration_response;

                status = umi_debug_runtime_request_configuration_done(
                    platform->adapter,
                    &configuration_sequence);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;

                status = umi_debug_runtime_adapter_wait_response(
                    platform->adapter,
                    configuration_sequence,
                    timeout_ms,
                    &configuration_response);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;

                configuration_sent = 1;
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (envelope.kind == UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE &&
                   envelope.request_sequence == launch_sequence) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (!envelope.success) return UMI_STATUS_UNAVAILABLE;
            launch_response_received = 1;

            /*
             * Some adapters respond to launch before emitting initialized.
             * A configurationDone request is only sent after the event.
             */
            if (!platform->capabilities.supports_configuration_done) {
                configuration_sent = 1;
            }
        }

        attempts += 1U;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!launch_response_received) return UMI_STATUS_TIMEOUT;

    /*
     * Adapters that advertise configurationDone but do not emit initialized
     * within the bounded wait are left ready for the caller to pump events.
     * This avoids deadlocking on adapter-specific launch sequencing.
     */
    return UMI_STATUS_OK;
}

/*
 * Provide the debug runtime platform start operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_platform_start(
    UmiDebugRuntimePlatform *platform,
    const char *profile_id,
    const char *session_id,
    const char *configuration_id,
    const char *launch_or_attach_arguments_json,
    int attach,
    const char *working_directory,
    uint32_t timeout_ms)
{
    UmiDebugAdapterProfile profile;
    UmiDebugRuntimeProfileHealth health;
    UmiLanguageRuntimeArguments arguments;
    char generated_arguments[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    const char *launch_arguments = launch_or_attach_arguments_json;
    uint64_t launch_sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || profile_id == NULL ||
        session_id == NULL || session_id[0] == '\0' ||
        configuration_id == NULL || configuration_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (platform->active) return UMI_STATUS_BUSY;

    status = profile_find(platform, profile_id, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if ((attach && !profile.supports_attach) ||
        (!attach && !profile.supports_launch)) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_debug_runtime_profile_health_probe(&profile, &health);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!health.available) return UMI_STATUS_UNAVAILABLE;

    status = umi_language_runtime_arguments_parse(
        profile.arguments,
        &arguments);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_start_process(
        session_id,
        profile.executable,
        arguments.values,
        arguments.count,
        working_directory,
        &platform->adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = initialize_adapter(platform, &profile, timeout_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = register_contract(
        platform,
        &profile,
        session_id,
        timeout_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = umi_debug_runtime_service_bridge_init(
        &platform->bridge,
        platform->service,
        session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = umi_debug_advanced_platform_open_session(
        platform->advanced,
        session_id,
        platform->descriptor.id,
        platform->capability_bits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    (void)snprintf(
        platform->active_session_id,
        sizeof(platform->active_session_id),
        "%s",
        session_id);
    (void)snprintf(
        platform->active_configuration_id,
        sizeof(platform->active_configuration_id),
        "%s",
        configuration_id);
    (void)snprintf(
        platform->active_profile_id,
        sizeof(platform->active_profile_id),
        "%s",
        profile.id);
    platform->attached = attach != 0;

    status = umi_debug_runtime_publish_session(
        &platform->bridge,
        configuration_id,
        platform->descriptor.id,
        "initializing",
        UMI_DEBUG_INSPECTION_READY,
        attach,
        platform->capabilities.supports_restart);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (launch_arguments == NULL) {
        status = build_default_launch_arguments(
            platform,
            configuration_id,
            attach,
            generated_arguments,
            sizeof(generated_arguments));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;
        launch_arguments = generated_arguments;
    }

    status = attach
        ? umi_debug_runtime_request_attach(
            platform->adapter,
            launch_arguments,
            &launch_sequence)
        : umi_debug_runtime_request_launch(
            platform->adapter,
            launch_arguments,
            &launch_sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = wait_launch_and_initialized(
        platform,
        launch_sequence,
        timeout_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    platform->active = 1;
    platform->paused = 0;
    (void)umi_debug_runtime_adapter_set_state(
        platform->adapter,
        UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
    (void)umi_debug_inspection_session_set_state(
        umi_debug_advanced_platform_inspection(platform->advanced),
        UMI_DEBUG_INSPECTION_RUNNING);

    status = umi_debug_runtime_publish_session(
        &platform->bridge,
        configuration_id,
        platform->descriptor.id,
        "running",
        UMI_DEBUG_INSPECTION_RUNNING,
        attach,
        platform->capabilities.supports_restart);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    platform->revision += 1U;
    return UMI_STATUS_OK;

failure:
    /* Apply this branch only when its contract condition is satisfied. */
    if (platform->descriptor.id[0] != '\0') {
        (void)umi_debug_advanced_platform_close_session(platform->advanced);
        (void)umi_debug_advanced_platform_unregister_adapter(
            platform->advanced,
            platform->descriptor.id);
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform->contract_owner != NULL) {
        umi_debug_runtime_contract_adapter_destroy(
            platform->contract_owner);
        platform->contract_owner = NULL;
        platform->adapter = NULL;
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (platform->adapter != NULL) {
        umi_debug_runtime_adapter_destroy(platform->adapter);
        platform->adapter = NULL;
    }

    (void)memset(&platform->descriptor, 0, sizeof(platform->descriptor));
    return status;
}

/*
 * Provide the debug runtime platform stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_platform_stop(
    UmiDebugRuntimePlatform *platform,
    int terminate_debuggee,
    uint32_t timeout_ms)
{
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!platform->active || platform->adapter == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    {
        uint64_t sequence = 0U;
        UmiDebugRuntimeEnvelope response;

        status = umi_debug_runtime_request_disconnect(
            platform->adapter,
            0,
            terminate_debuggee,
            0,
            &sequence);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_debug_runtime_adapter_wait_response(
                platform->adapter,
                sequence,
                timeout_ms,
                &response);
        }
    }

    (void)umi_debug_runtime_adapter_stop(
        platform->adapter,
        timeout_ms);
    (void)umi_debug_advanced_platform_close_session(platform->advanced);

    /* Apply this branch only when its contract condition is satisfied. */
    if (platform->descriptor.id[0] != '\0') {
        (void)umi_debug_advanced_platform_unregister_adapter(
            platform->advanced,
            platform->descriptor.id);
    }

    (void)umi_debug_runtime_publish_session(
        &platform->bridge,
        platform->active_configuration_id,
        platform->descriptor.id,
        "stopped",
        UMI_DEBUG_INSPECTION_UNBOUND,
        platform->attached,
        platform->capabilities.supports_restart);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform->contract_owner != NULL) {
        umi_debug_runtime_contract_adapter_destroy(
            platform->contract_owner);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        umi_debug_runtime_adapter_destroy(platform->adapter);
    }

    platform->contract_owner = NULL;
    platform->adapter = NULL;
    (void)memset(&platform->descriptor, 0, sizeof(platform->descriptor));
    (void)memset(&platform->capabilities, 0, sizeof(platform->capabilities));
    platform->capability_bits = 0U;
    platform->active_thread_id = 0U;
    platform->active_frame_id = 0U;
    platform->active = 0;
    platform->initialized = 0;
    platform->attached = 0;
    platform->paused = 0;
    platform->revision += 1U;

    return status;
}


/*
 * Provide the debug runtime platform pump event operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_pump_event(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms,
    int *out_handled)
{
    UmiDebugRuntimeEnvelope envelope;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || out_handled == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_handled = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!platform->active || platform->adapter == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_debug_runtime_adapter_next_event(
        platform->adapter,
        &envelope);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) {
        status = umi_debug_runtime_adapter_receive(
            platform->adapter,
            timeout_ms,
            &envelope);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (envelope.kind != UMI_DEBUG_RUNTIME_MESSAGE_EVENT) {
        /*
         * Platform operations are synchronous and ordinarily consume responses
         * themselves. If a response reaches the event pump, preserve protocol
         * integrity by reporting it as unhandled rather than fabricating state.
         */
        return UMI_STATUS_OK;
    }

    status = publish_event_envelope(platform, &envelope);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        *out_handled = 1;
    }

    return status;
}

/*
 * Provide the debug runtime platform sync breakpoints operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_platform_sync_breakpoints(
    UmiDebugRuntimePlatform *platform,
    const char *source_uri,
    uint32_t timeout_ms)
{
    UmiDebugBreakpointRegistry *registry;
    UmiDebugBreakpointSnapshot
        requested[UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS];
    UmiDebugRuntimeBreakpointList result;
    UmiDebugRuntimeEnvelope response;
    size_t requested_count = 0U;
    size_t index;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || source_uri == NULL ||
        source_uri[0] == '\0' || platform->adapter == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_debug_service_breakpoint(platform->service);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_debug_breakpoint_registry_count(registry);
         ++index) {
        UmiDebugBreakpointSnapshot item;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_debug_breakpoint_registry_at(
                registry, index, &item) != UMI_STATUS_OK ||
            strcmp(item.uri, source_uri) != 0) {
            continue;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (requested_count >= UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        requested[requested_count++] = item;
    }

    status = umi_debug_runtime_request_set_breakpoints(
        platform->adapter,
        source_uri,
        requested,
        requested_count,
        0,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_breakpoints(
        response.json,
        &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * Preserve each original breakpoint's condition/logpoint/user identity and
     * only merge adapter verification/location fields by request order.
     */
    for (index = 0U;
         index < requested_count && index < result.count;
         ++index) {
        requested[index].verified = result.items[index].verified;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (result.items[index].line != 0U) {
            requested[index].line = result.items[index].line;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (result.items[index].column != 0U) {
            requested[index].column = result.items[index].column;
        }

        requested[index].revision += 1U;

        status = umi_debug_breakpoint_registry_upsert(
            registry,
            &requested[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    platform->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug runtime platform refresh threads operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_refresh_threads(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    UmiDebugRuntimeThreadList result;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_request_threads(
        platform->adapter,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_threads(
        response.json,
        &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (platform->active_thread_id == 0U && result.count > 0U) {
        platform->active_thread_id = result.items[0].id;
    }

    status = umi_debug_runtime_publish_threads(
        &platform->bridge,
        &result,
        platform->active_thread_id,
        platform->paused);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug runtime platform refresh stack operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_refresh_stack(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    UmiDebugRuntimeStackTrace result;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        thread_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_request_stack_trace(
        platform->adapter,
        thread_id,
        0U,
        UMI_DEBUG_RUNTIME_MAX_ITEMS,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_stack_trace(
        response.json,
        &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    platform->active_thread_id = thread_id;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (result.count > 0U) {
        platform->active_frame_id = result.items[0].id;
    }

    status = umi_debug_runtime_publish_stack(
        &platform->bridge,
        thread_id,
        &result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug runtime platform refresh scopes operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_refresh_scopes(
    UmiDebugRuntimePlatform *platform,
    uint64_t frame_id,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    UmiDebugRuntimeScopeList result;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        frame_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_request_scopes(
        platform->adapter,
        frame_id,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_scopes(response.json, &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    platform->active_frame_id = frame_id;
    status = umi_debug_runtime_publish_scopes(
        &platform->bridge,
        frame_id,
        &result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug runtime platform refresh variables operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_platform_refresh_variables(
    UmiDebugRuntimePlatform *platform,
    const char *scope_id,
    uint64_t variables_reference,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    UmiDebugRuntimeVariableList result;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        scope_id == NULL || scope_id[0] == '\0' ||
        variables_reference == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_request_variables(
        platform->adapter,
        variables_reference,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_variables(
        response.json,
        &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_publish_variables(
        &platform->bridge,
        scope_id,
        &result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug runtime platform evaluate watch operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_evaluate_watch(
    UmiDebugRuntimePlatform *platform,
    const char *watch_id,
    const char *expression,
    uint64_t frame_id,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    UmiDebugRuntimeEvaluateResult result;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        watch_id == NULL || expression == NULL ||
        expression[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_request_evaluate(
        platform->adapter,
        expression,
        frame_id,
        "watch",
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_evaluate(
        response.json,
        &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_publish_watch(
        &platform->bridge,
        watch_id,
        expression,
        &result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}


/* Provide the thread control operation used by this module and its client applications. */
static UmiStatus thread_control(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms,
    UmiStatus (*request)(
        UmiDebugRuntimeAdapter *,
        uint64_t,
        uint64_t *),
    UmiDebugRuntimeAdapterState state_after_response)
{
    UmiDebugRuntimeEnvelope response;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        thread_id == 0U || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = request(platform->adapter, thread_id, &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    platform->active_thread_id = thread_id;

    /* Apply this branch only when its contract condition is satisfied. */
    if (state_after_response == UMI_DEBUG_RUNTIME_ADAPTER_RUNNING) {
        platform->paused = 0;
        (void)umi_debug_runtime_adapter_set_state(
            platform->adapter,
            UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
        (void)umi_debug_inspection_session_set_state(
            umi_debug_advanced_platform_inspection(platform->advanced),
            UMI_DEBUG_INSPECTION_RUNNING);
    }

    platform->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug runtime platform continue operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_platform_continue(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms)
{
    return thread_control(
        platform,
        thread_id,
        timeout_ms,
        umi_debug_runtime_request_continue,
        UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
}

/*
 * Provide the debug runtime platform pause operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_platform_pause(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms)
{
    return thread_control(
        platform,
        thread_id,
        timeout_ms,
        umi_debug_runtime_request_pause,
        UMI_DEBUG_RUNTIME_ADAPTER_PAUSED);
}

/*
 * Provide the debug runtime platform step over operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_step_over(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms)
{
    return thread_control(
        platform,
        thread_id,
        timeout_ms,
        umi_debug_runtime_request_next,
        UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
}

/*
 * Provide the debug runtime platform step into operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_step_into(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms)
{
    return thread_control(
        platform,
        thread_id,
        timeout_ms,
        umi_debug_runtime_request_step_in,
        UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
}

/*
 * Provide the debug runtime platform step out operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_platform_step_out(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms)
{
    return thread_control(
        platform,
        thread_id,
        timeout_ms,
        umi_debug_runtime_request_step_out,
        UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
}

/*
 * Provide the debug runtime platform refresh modules operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_refresh_modules(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    UmiDebugRuntimeModuleList result;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_modules_request) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_debug_runtime_request_modules(
        platform->adapter,
        0U,
        UMI_DEBUG_RUNTIME_MAX_ITEMS,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_modules(response.json, &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_publish_modules(
        &platform->bridge,
        &result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug runtime platform read memory operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_read_memory(
    UmiDebugRuntimePlatform *platform,
    const char *memory_reference,
    int64_t offset,
    uint32_t count,
    uint32_t timeout_ms,
    UmiDebugRuntimeMemoryResult *out_result)
{
    UmiDebugRuntimeEnvelope response;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        memory_reference == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_read_memory_request) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_debug_runtime_request_read_memory(
        platform->adapter,
        memory_reference,
        offset,
        count,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_memory(
        response.json,
        out_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = project_memory_view(
        platform,
        memory_reference,
        offset,
        count,
        out_result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug runtime platform disassemble operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_disassemble(
    UmiDebugRuntimePlatform *platform,
    const char *memory_reference,
    int64_t offset,
    int64_t instruction_offset,
    uint32_t instruction_count,
    uint32_t timeout_ms,
    UmiDebugRuntimeDisassembly *out_result)
{
    UmiDebugRuntimeEnvelope response;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        memory_reference == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_disassemble_request) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_debug_runtime_request_disassemble(
        platform->adapter,
        memory_reference,
        offset,
        instruction_offset,
        instruction_count,
        1,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_disassembly(
        response.json,
        out_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = project_disassembly_view(platform, out_result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug runtime platform snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_platform_snapshot(
    UmiDebugRuntimePlatform *platform,
    UmiDebugRuntimePlatformSnapshot *out_snapshot)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));

    status = umi_debug_service_snapshot(
        platform->service,
        &out_snapshot->service);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_advanced_platform_snapshot(
        platform->advanced,
        &out_snapshot->advanced);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform->adapter != NULL) {
        status = umi_debug_runtime_adapter_snapshot(
            platform->adapter,
            &out_snapshot->adapter);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    out_snapshot->capabilities = platform->capabilities;
    (void)snprintf(
        out_snapshot->active_session_id,
        sizeof(out_snapshot->active_session_id),
        "%s",
        platform->active_session_id);
    (void)snprintf(
        out_snapshot->active_configuration_id,
        sizeof(out_snapshot->active_configuration_id),
        "%s",
        platform->active_configuration_id);
    (void)snprintf(
        out_snapshot->active_profile_id,
        sizeof(out_snapshot->active_profile_id),
        "%s",
        platform->active_profile_id);
    out_snapshot->active_thread_id = platform->active_thread_id;
    out_snapshot->active_frame_id = platform->active_frame_id;
    out_snapshot->capability_bits = platform->capability_bits;
    out_snapshot->revision = platform->revision;
    out_snapshot->active = platform->active;
    out_snapshot->initialized = platform->initialized;
    out_snapshot->attached = platform->attached;
    out_snapshot->paused = platform->paused;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug runtime platform service operation used by this module and its client
 * applications.
 */
UmiDebugService *umi_debug_runtime_platform_service(
    UmiDebugRuntimePlatform *platform)
{
    return platform != NULL ? platform->service : NULL;
}

/*
 * Provide the debug runtime platform advanced operation used by this module and its client
 * applications.
 */
UmiDebugAdvancedPlatform *umi_debug_runtime_platform_advanced(
    UmiDebugRuntimePlatform *platform)
{
    return platform != NULL ? platform->advanced : NULL;
}

/*
 * Provide the debug runtime platform adapter operation used by this module and its client
 * applications.
 */
UmiDebugRuntimeAdapter *umi_debug_runtime_platform_adapter(
    UmiDebugRuntimePlatform *platform)
{
    return platform != NULL ? platform->adapter : NULL;
}


/* Provide the contains folded operation used by this module and its client applications. */
static int contains_folded(const char *text, const char *needle)
{
    const char *start;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || needle == NULL || needle[0] == '\0') return 0;

    /* Visit each bounded item once so every record receives the same rule. */
    for (start = text; *start != '\0'; ++start) {
        const char *left = start;
        const char *right = needle;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*left != '\0' && *right != '\0') {
            char a = *left;
            char b = *right;

            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');

            /* Apply this branch only when its contract condition is satisfied. */
            if (a != b) break;
            ++left;
            ++right;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*right == '\0') return 1;
    }

    return 0;
}

/* Provide the parse address operation used by this module and its client applications. */
static uint64_t parse_address(const char *text)
{
    char *end = NULL;
    unsigned long long value;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0') return 0U;

    value = strtoull(text, &end, 0);
    return end != text && *end == '\0'
        ? (uint64_t)value
        : 0U;
}

/*
 * Provide the project memory view operation used by this module and its client
 * applications.
 */
static UmiStatus project_memory_view(
    UmiDebugRuntimePlatform *platform,
    const char *memory_reference,
    int64_t offset,
    uint32_t requested_count,
    const UmiDebugRuntimeMemoryResult *result)
{
    UmiDebugMemoryView *view;
    size_t cursor = 0U;
    size_t block_index = 0U;
    uint64_t base_address;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || memory_reference == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    view = umi_debug_inspection_session_memory(
        umi_debug_advanced_platform_inspection(platform->advanced));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return UMI_STATUS_INVALID_STATE;

    base_address = parse_address(result->address);

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor < result->byte_count || (result->byte_count == 0U && block_index == 0U)) {
        UmiDebugMemoryBlock block;
        const size_t remaining =
            result->byte_count > cursor ? result->byte_count - cursor : 0U;
        const size_t chunk =
            remaining > UMI_DEBUG_MEMORY_BLOCK_BYTE_CAPACITY
                ? UMI_DEBUG_MEMORY_BLOCK_BYTE_CAPACITY
                : remaining;
        int written;

        (void)memset(&block, 0, sizeof(block));
        block.struct_size = (uint32_t)sizeof(block);
        block.api_version = UMI_DEBUG_MEMORY_VIEW_API_VERSION;

        written = snprintf(
            block.id,
            sizeof(block.id),
            "memory.%zu",
            block_index);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(block.id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)snprintf(
            block.session_id,
            sizeof(block.session_id),
            "%s",
            platform->active_session_id);
        (void)snprintf(
            block.memory_reference,
            sizeof(block.memory_reference),
            "%s",
            memory_reference);

        block.address =
            base_address != 0U ? base_address + cursor : 0U;
        block.reference_offset = offset + (int64_t)cursor;
        block.requested_length =
            requested_count > cursor
                ? (size_t)requested_count - cursor
                : chunk;
        /* Apply this branch only when its contract condition is satisfied. */
        if (block.requested_length > UMI_DEBUG_MEMORY_BLOCK_BYTE_CAPACITY) {
            block.requested_length = UMI_DEBUG_MEMORY_BLOCK_BYTE_CAPACITY;
        }
        block.length = chunk;
        block.unreadable_bytes =
            cursor + chunk >= result->byte_count
                ? result->unreadable_bytes
                : 0U;
        block.sequence = platform->revision + block_index + 1U;
        block.revision = block.sequence;
        block.writable = platform->capabilities.supports_write_memory_request;
        block.stale = 0;
        block.complete = 1;

        /* Apply this branch only when its contract condition is satisfied. */
        if (chunk > 0U) {
            (void)memcpy(
                block.bytes,
                result->bytes + cursor,
                chunk);
        }

        status = umi_debug_memory_view_upsert(view, &block);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Apply this branch only when its contract condition is satisfied. */
        if (block_index == 0U) {
            (void)umi_debug_memory_view_select(view, block.id);
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (chunk == 0U) break;
        cursor += chunk;
        block_index += 1U;
    }

    return UMI_STATUS_OK;
}

/* Provide the split instruction operation used by this module and its client applications. */
static void split_instruction(
    const char *text,
    char *mnemonic,
    size_t mnemonic_capacity,
    char *operands,
    size_t operands_capacity)
{
    const char *space;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mnemonic != NULL && mnemonic_capacity > 0U) mnemonic[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operands != NULL && operands_capacity > 0U) operands[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return;

    space = text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*space != '\0' && *space != ' ' && *space != '\t') ++space;

    length = (size_t)(space - text);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mnemonic != NULL && mnemonic_capacity > 0U) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= mnemonic_capacity) length = mnemonic_capacity - 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length > 0U) (void)memcpy(mnemonic, text, length);
        mnemonic[length] = '\0';
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*space == ' ' || *space == '\t') ++space;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operands != NULL && operands_capacity > 0U) {
        (void)snprintf(operands, operands_capacity, "%s", space);
    }
}

/*
 * Provide the project disassembly view operation used by this module and its client
 * applications.
 */
static UmiStatus project_disassembly_view(
    UmiDebugRuntimePlatform *platform,
    const UmiDebugRuntimeDisassembly *result)
{
    UmiDebugDisassemblyView *view;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    view = umi_debug_inspection_session_disassembly(
        umi_debug_advanced_platform_inspection(platform->advanced));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_debug_disassembly_view_clear(view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < result->count; ++index) {
        UmiDebugInstruction instruction;
        const uint64_t address = parse_address(result->items[index].address);
        int written;

        (void)memset(&instruction, 0, sizeof(instruction));
        instruction.struct_size = (uint32_t)sizeof(instruction);
        instruction.api_version = UMI_DEBUG_DISASSEMBLY_VIEW_API_VERSION;

        written = snprintf(
            instruction.id,
            sizeof(instruction.id),
            "instruction.%zu",
            index);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(instruction.id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)snprintf(
            instruction.session_id,
            sizeof(instruction.session_id),
            "%s",
            platform->active_session_id);
        (void)snprintf(
            instruction.memory_reference,
            sizeof(instruction.memory_reference),
            "%s",
            result->items[index].address);
        (void)umi_text_copy_truncated(
            instruction.instruction_bytes,
            sizeof(instruction.instruction_bytes),
            result->items[index].instruction_bytes);
        split_instruction(
            result->items[index].instruction,
            instruction.mnemonic,
            sizeof(instruction.mnemonic),
            instruction.operands,
            sizeof(instruction.operands));
        (void)snprintf(
            instruction.symbol,
            sizeof(instruction.symbol),
            "%s",
            result->items[index].symbol);
        (void)umi_text_copy_truncated(
            instruction.source_uri,
            sizeof(instruction.source_uri),
            result->items[index].location.path);

        instruction.address = address;
        instruction.instruction_offset = (int64_t)index;
        instruction.source_line = result->items[index].line;
        instruction.source_column = result->items[index].column;
        instruction.sequence = platform->revision + index + 1U;
        instruction.revision = instruction.sequence;
        instruction.has_source =
            instruction.source_uri[0] != '\0';
        instruction.current = index == 0U;
        instruction.can_breakpoint =
            platform->capabilities.supports_instruction_breakpoints;
        instruction.breakpoint = 0;
        instruction.valid = 1;

        status = umi_debug_disassembly_view_upsert(view, &instruction);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == 0U) {
            (void)umi_debug_disassembly_view_set_current(
                view, instruction.id);
            (void)umi_debug_disassembly_view_select(
                view, instruction.id);
        }
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the debug runtime platform refresh registers operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_platform_refresh_registers(
    UmiDebugRuntimePlatform *platform,
    uint64_t frame_id,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    UmiDebugRuntimeScopeList scopes;
    UmiDebugRegisterBank *bank;
    uint64_t sequence = 0U;
    size_t scope_index;
    size_t group_order = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL || frame_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_request_scopes(
        platform->adapter, frame_id, &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter, sequence, timeout_ms, &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_scopes(response.json, &scopes);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    bank = umi_debug_inspection_session_registers(
        umi_debug_advanced_platform_inspection(platform->advanced));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bank == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_debug_register_bank_clear(bank);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (scope_index = 0U; scope_index < scopes.count; ++scope_index) {
        UmiDebugRuntimeVariableList variables;
        UmiDebugRegisterGroup group;
        UmiDebugRuntimeEnvelope variable_response;
        uint64_t variable_sequence = 0U;
        size_t variable_index;

        /* Apply this branch only when its contract condition is satisfied. */
        if (!contains_folded(scopes.items[scope_index].name, "register")) {
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (scopes.items[scope_index].variables_reference == 0U) {
            continue;
        }

        status = umi_debug_runtime_request_variables(
            platform->adapter,
            scopes.items[scope_index].variables_reference,
            &variable_sequence);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_debug_runtime_adapter_wait_response(
            platform->adapter,
            variable_sequence,
            timeout_ms,
            &variable_response);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_debug_runtime_decode_variables(
            variable_response.json,
            &variables);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        (void)memset(&group, 0, sizeof(group));
        group.struct_size = (uint32_t)sizeof(group);
        group.api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
        (void)snprintf(
            group.id, sizeof(group.id),
            "register-group.%zu",
            scope_index);
        (void)snprintf(
            group.session_id, sizeof(group.session_id),
            "%s",
            platform->active_session_id);
        (void)snprintf(
            group.frame_id, sizeof(group.frame_id),
            "%llu",
            (unsigned long long)frame_id);
        (void)umi_text_copy_truncated(
            group.name, sizeof(group.name), scopes.items[scope_index].name);
        (void)snprintf(
            group.description, sizeof(group.description),
            "%s",
            "Register scope exposed by the active Debug Adapter Protocol provider.");
        group.order = (int32_t)group_order++;
        group.sequence = platform->revision + scope_index + 1U;
        group.revision = group.sequence;
        group.expanded = 1;
        group.available = 1;

        status = umi_debug_register_bank_upsert_group(bank, &group);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Visit each bounded item once so every record receives the same rule. */
        for (variable_index = 0U;
             variable_index < variables.count;
             ++variable_index) {
            UmiDebugRegisterValue register_value;

            (void)memset(&register_value, 0, sizeof(register_value));
            register_value.struct_size = (uint32_t)sizeof(register_value);
            register_value.api_version = UMI_DEBUG_REGISTER_BANK_API_VERSION;
            (void)snprintf(
                register_value.id,
                sizeof(register_value.id),
                "register.%zu.%zu",
                scope_index,
                variable_index);
            (void)snprintf(
                register_value.group_id,
                sizeof(register_value.group_id),
                "%s",
                group.id);
            (void)umi_text_copy_truncated(
                register_value.name, sizeof(register_value.name),
                variables.items[variable_index].name);
            (void)umi_text_copy_truncated(
                register_value.value, sizeof(register_value.value),
                variables.items[variable_index].value);
            (void)umi_text_copy_truncated(
                register_value.raw_value, sizeof(register_value.raw_value),
                variables.items[variable_index].value);
            (void)umi_text_copy_truncated(
                register_value.type, sizeof(register_value.type),
                variables.items[variable_index].type);
            (void)snprintf(
                register_value.format,
                sizeof(register_value.format),
                "%s",
                "adapter");
            register_value.order = (int32_t)variable_index;
            register_value.sequence =
                platform->revision + variable_index + 1U;
            register_value.revision = register_value.sequence;
            register_value.changed = 0;
            register_value.read_only = 1;
            register_value.available = 1;

            status = umi_debug_register_bank_upsert_register(
                bank, &register_value);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }

    platform->revision += 1U;
    return UMI_STATUS_OK;
}


/*
 * Provide the debug runtime platform step back operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_step_back(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_step_back) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return thread_control(
        platform,
        thread_id,
        timeout_ms,
        umi_debug_runtime_request_step_back,
        UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
}

/*
 * Provide the debug runtime platform reverse continue operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_platform_reverse_continue(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_step_back) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return thread_control(
        platform,
        thread_id,
        timeout_ms,
        umi_debug_runtime_request_reverse_continue,
        UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
}

/*
 * Provide the debug runtime platform restart frame operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_restart_frame(
    UmiDebugRuntimePlatform *platform,
    uint64_t frame_id,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL || frame_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_restart_frame) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_debug_runtime_request_restart_frame(
        platform->adapter,
        frame_id,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        platform->active_frame_id = frame_id;
        platform->revision += 1U;
    }

    return status;
}

/*
 * Provide the debug runtime platform set variable operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_set_variable(
    UmiDebugRuntimePlatform *platform,
    uint64_t variables_reference,
    const char *name,
    const char *value,
    uint32_t timeout_ms,
    UmiDebugRuntimeEvaluateResult *out_result)
{
    UmiDebugRuntimeEnvelope response;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        name == NULL || value == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_set_variable) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_debug_runtime_request_set_variable(
        platform->adapter,
        variables_reference,
        name,
        value,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_evaluate(response.json, out_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug runtime platform set expression operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_set_expression(
    UmiDebugRuntimePlatform *platform,
    const char *expression,
    const char *value,
    uint64_t frame_id,
    uint32_t timeout_ms,
    UmiDebugRuntimeEvaluateResult *out_result)
{
    UmiDebugRuntimeEnvelope response;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        expression == NULL || value == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_set_expression) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_debug_runtime_request_set_expression(
        platform->adapter,
        expression,
        value,
        frame_id,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_decode_evaluate(response.json, out_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug runtime platform write memory operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_platform_write_memory(
    UmiDebugRuntimePlatform *platform,
    const char *memory_reference,
    int64_t offset,
    const char *base64_data,
    int allow_partial,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        memory_reference == NULL || base64_data == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_write_memory_request) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_debug_runtime_request_write_memory(
        platform->adapter,
        memory_reference,
        offset,
        base64_data,
        allow_partial,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        /*
         * Existing memory blocks are intentionally marked stale rather than
         * optimisticly patched from base64 text. A subsequent readMemory is the
         * authoritative confirmation from the debuggee.
         */
        UmiDebugMemoryView *view = umi_debug_inspection_session_memory(
            umi_debug_advanced_platform_inspection(platform->advanced));
        UmiDebugMemoryViewSnapshot snapshot;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (view != NULL &&
            umi_debug_memory_view_snapshot(view, &snapshot) == UMI_STATUS_OK &&
            snapshot.has_selection) {
            (void)umi_debug_memory_view_mark_stale(
                view,
                snapshot.selected_block_id,
                1);
        }

        platform->revision += 1U;
    }

    return status;
}


/*
 * Provide the debug runtime platform restart operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_platform_restart(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms)
{
    UmiDebugRuntimeEnvelope response;
    uint64_t sequence = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || platform->adapter == NULL ||
        !platform->active) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!platform->capabilities.supports_restart) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = umi_debug_runtime_request_restart(
        platform->adapter,
        NULL,
        &sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_wait_response(
        platform->adapter,
        sequence,
        timeout_ms,
        &response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    platform->paused = 0;
    platform->active_thread_id = 0U;
    platform->active_frame_id = 0U;
    (void)umi_debug_runtime_adapter_set_state(
        platform->adapter,
        UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
    (void)umi_debug_inspection_session_set_state(
        umi_debug_advanced_platform_inspection(platform->advanced),
        UMI_DEBUG_INSPECTION_RUNNING);

    status = umi_debug_runtime_publish_session(
        &platform->bridge,
        platform->active_configuration_id,
        platform->descriptor.id,
        "running",
        UMI_DEBUG_INSPECTION_RUNNING,
        platform->attached,
        platform->capabilities.supports_restart);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}
