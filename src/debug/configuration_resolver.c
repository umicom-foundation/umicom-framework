/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/configuration_resolver.c
 *
 * PURPOSE:
 *   Resolve and validate debugger configurations with bounded workspace token
 *   substitution and adapter capability checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/configuration_resolver.h"

#include <stdlib.h>
#include <string.h>

struct UmiDebugConfigurationResolver {
    UmiDebugResolvedConfiguration result;
    uint64_t revision;
    int has_result;
};

typedef struct TokenValue {
    const char *token;
    const char *value;
} TokenValue;

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(char *output, size_t capacity, size_t *length,
                             const char *text, size_t text_length)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || length == NULL || text == NULL || capacity == 0U ||
        text_length > capacity - 1U || *length > capacity - 1U - text_length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(output + *length, text, text_length);
    *length += text_length;
    output[*length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the expand text operation used by this module and its client applications. */
static UmiStatus expand_text(const char *input, char *output, size_t capacity,
                             const TokenValue *tokens, size_t token_count,
                             size_t *substitution_count)
{
    size_t input_index = 0U;
    size_t output_length = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || output == NULL || capacity == 0U || tokens == NULL ||
        substitution_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    output[0] = '\0';
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (input[input_index] != '\0') {
        size_t token_index;
        int matched = 0;
        /* Visit each bounded item once so every record receives the same rule. */
        for (token_index = 0U; token_index < token_count; ++token_index) {
            size_t token_length = strlen(tokens[token_index].token);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (strncmp(input + input_index, tokens[token_index].token,
                        token_length) == 0) {
                const char *value = tokens[token_index].value != NULL
                    ? tokens[token_index].value : "";
                UmiStatus status = append_text(output, capacity, &output_length,
                                               value, strlen(value));
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
                input_index += token_length;
                ++*substitution_count;
                matched = 1;
                break;
            }
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!matched) {
            UmiStatus status = append_text(output, capacity, &output_length,
                                           input + input_index, 1U);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            ++input_index;
        }
    }
    return UMI_STATUS_OK;
}

/* Provide the file directory operation used by this module and its client applications. */
static void file_directory(const char *path, char *output, size_t capacity)
{
    const char *slash;
    const char *backslash;
    const char *separator;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || capacity == 0U) return;
    output[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0') return;
    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    separator = slash;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (backslash != NULL && (separator == NULL || backslash > separator)) {
        separator = backslash;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (separator == NULL) return;
    length = (size_t)(separator - path);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    (void)memcpy(output, path, length);
    output[length] = '\0';
}

/* Provide the validate request operation used by this module and its client applications. */
static UmiStatus validate_request(const UmiDebugConfigurationRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_DEBUG_CONFIGURATION_RESOLVER_API_VERSION ||
        !terminated(request->configuration_id,
                    sizeof(request->configuration_id)) ||
        request->configuration_id[0] == '\0' ||
        !terminated(request->workspace_folder,
                    sizeof(request->workspace_folder)) ||
        !terminated(request->active_file, sizeof(request->active_file)) ||
        !terminated(request->program_override,
                    sizeof(request->program_override)) ||
        !terminated(request->arguments_override,
                    sizeof(request->arguments_override)) ||
        !terminated(request->working_directory_override,
                    sizeof(request->working_directory_override)) ||
        !terminated(request->environment_override,
                    sizeof(request->environment_override))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise debug configuration resolver from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_configuration_resolver_create(
    UmiDebugConfigurationResolver **out_resolver)
{
    UmiDebugConfigurationResolver *resolver;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_resolver == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_resolver = NULL;
    resolver = (UmiDebugConfigurationResolver *)calloc(1U, sizeof(*resolver));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolver == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    resolver->revision = 1U;
    *out_resolver = resolver;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug configuration resolver so the same storage can be
 * reused safely.
 */
void umi_debug_configuration_resolver_destroy(
    UmiDebugConfigurationResolver *resolver)
{
    free(resolver);
}

/*
 * Release or reset state held by debug configuration resolver so the same storage can be
 * reused safely.
 */
UmiStatus umi_debug_configuration_resolver_clear(
    UmiDebugConfigurationResolver *resolver)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolver == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(&resolver->result, 0, sizeof(resolver->result));
    resolver->has_result = 0;
    resolver->revision = next_revision(resolver->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the debug configuration resolver resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_configuration_resolver_resolve(
    UmiDebugConfigurationResolver *resolver,
    const UmiDebugLaunchConfigurationRegistry *configurations,
    const UmiDebugAdapterProfileRegistry *adapters,
    const UmiDebugConfigurationRequest *request)
{
    UmiDebugLaunchConfigurationSnapshot launch;
    UmiDebugAdapterProfile adapter;
    UmiDebugResolvedConfiguration result;
    char active_directory[1024];
    TokenValue tokens[4];
    const char *program;
    const char *arguments;
    const char *working_directory;
    const char *environment;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolver == NULL || configurations == NULL || adapters == NULL ||
        validate_request(request) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&result, 0, sizeof(result));
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_DEBUG_CONFIGURATION_RESOLVER_API_VERSION;
    status = umi_debug_launch_configuration_registry_find(
        configurations, request->configuration_id, &launch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_debug_adapter_profile_registry_find(adapters,
                                                         launch.adapter,
                                                         &adapter);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        (!adapter.enabled || !adapter.supports_launch)) {
        status = UMI_STATUS_UNAVAILABLE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        result.status = status;
        resolver->revision = next_revision(resolver->revision);
        result.revision = resolver->revision;
        resolver->result = result;
        resolver->has_result = 1;
        return status;
    }

    file_directory(request->active_file, active_directory,
                   sizeof(active_directory));
    tokens[0] = (TokenValue){"${workspaceFolder}", request->workspace_folder};
    tokens[1] = (TokenValue){"${file}", request->active_file};
    tokens[2] = (TokenValue){"${fileDirname}", active_directory};
    tokens[3] = (TokenValue){"${configurationId}", request->configuration_id};
    program = request->program_override[0] != '\0'
        ? request->program_override : launch.program;
    arguments = request->arguments_override[0] != '\0'
        ? request->arguments_override : launch.arguments;
    working_directory = request->working_directory_override[0] != '\0'
        ? request->working_directory_override : launch.working_directory;
    environment = request->environment_override[0] != '\0'
        ? request->environment_override : launch.environment;
    result.launch = launch;
    result.adapter = adapter;
    status = expand_text(program, result.launch.program,
                         sizeof(result.launch.program), tokens, 4U,
                         &result.substituted_token_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = expand_text(arguments, result.launch.arguments,
                             sizeof(result.launch.arguments), tokens, 4U,
                             &result.substituted_token_count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = expand_text(working_directory,
                             result.launch.working_directory,
                             sizeof(result.launch.working_directory), tokens,
                             4U, &result.substituted_token_count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = expand_text(environment, result.launch.environment,
                             sizeof(result.launch.environment), tokens, 4U,
                             &result.substituted_token_count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && result.launch.program[0] == '\0') {
        status = UMI_STATUS_INVALID_STATE;
    }
    result.uses_program_override = request->program_override[0] != '\0';
    result.uses_arguments_override = request->arguments_override[0] != '\0';
    result.uses_working_directory_override =
        request->working_directory_override[0] != '\0';
    result.uses_environment_override =
        request->environment_override[0] != '\0';
    result.status = status;
    result.ready = status == UMI_STATUS_OK;
    resolver->revision = next_revision(resolver->revision);
    result.revision = resolver->revision;
    resolver->result = result;
    resolver->has_result = 1;
    return status;
}

/*
 * Provide the debug configuration resolver result operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_configuration_resolver_result(
    const UmiDebugConfigurationResolver *resolver,
    UmiDebugResolvedConfiguration *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolver == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!resolver->has_result) return UMI_STATUS_NOT_FOUND;
    *out_result = resolver->result;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug configuration resolver revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_configuration_resolver_revision(
    const UmiDebugConfigurationResolver *resolver)
{
    return resolver != NULL ? resolver->revision : 0U;
}
