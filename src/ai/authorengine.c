/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/authorengine.c
 *
 * PURPOSE:
 *   Configure the Umicom AuthorEngine executable and build deterministic CLI arguments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * AuthorEngine is integrated through a process boundary, so Studio can reuse its capabilities without copying publishing logic into the IDE.
 */

#include "umicom/ai/authorengine.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_authorengine_validate(const UmiAiAuthorEngineConfig *config)
{
    if (config == NULL || config->executable[0] == '\0' || config->workspace[0] == '\0' || config->provider[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus invocation_add(UmiAiAuthorEngineInvocation *invocation,
                                const char *argument)
{
    int written;
    if (invocation == NULL || argument == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (invocation->argument_count >= UMI_AI_AUTHOR_ENGINE_ARGUMENT_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(
        invocation->arguments[invocation->argument_count],
        sizeof(invocation->arguments[0]),
        "%s",
        argument);
    if (written < 0 ||
        (size_t)written >= sizeof(invocation->arguments[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++invocation->argument_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_authorengine_plan_invocation(
    const UmiAiAuthorEngineConfig *config,
    UmiAiAuthorEngineCommand command,
    const char *session_id,
    const char *model_id,
    const char *input_path,
    const char *output_path,
    UmiAiAuthorEngineInvocation *out_invocation)
{
    const char *command_text;
    UmiStatus status;
    int written;

    if (umi_ai_authorengine_validate(config) != UMI_STATUS_OK ||
        out_invocation == NULL ||
        command < UMI_AI_AUTHOR_ENGINE_HEALTH ||
        command > UMI_AI_AUTHOR_ENGINE_GENERATE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (command == UMI_AI_AUTHOR_ENGINE_GENERATE &&
        (session_id == NULL || session_id[0] == '\0' ||
         model_id == NULL || model_id[0] == '\0' ||
         input_path == NULL || input_path[0] == '\0' ||
         output_path == NULL || output_path[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_invocation, 0, sizeof(*out_invocation));
    written = snprintf(out_invocation->executable,
                       sizeof(out_invocation->executable),
                       "%s",
                       config->executable);
    if (written < 0 ||
        (size_t)written >= sizeof(out_invocation->executable)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    out_invocation->command = command;
    command_text = command == UMI_AI_AUTHOR_ENGINE_HEALTH
        ? "health"
        : (command == UMI_AI_AUTHOR_ENGINE_CATALOGUE
               ? "providers" : "generate");
    status = invocation_add(out_invocation, command_text);
    if (status == UMI_STATUS_OK) {
        status = invocation_add(out_invocation, "--workspace");
    }
    if (status == UMI_STATUS_OK) {
        status = invocation_add(out_invocation, config->workspace);
    }
    if (status == UMI_STATUS_OK) {
        status = invocation_add(out_invocation, "--provider");
    }
    if (status == UMI_STATUS_OK) {
        status = invocation_add(out_invocation, config->provider);
    }
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, "--session");
    }
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, session_id);
    }
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, "--model");
    }
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, model_id);
    }
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, "--input");
    }
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, input_path);
    }
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, "--output");
    }
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, output_path);
    }
    return status;
}
