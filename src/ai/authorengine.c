/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/authorengine.c
 *
 * PURPOSE:
 *   Configure the Umicom AuthorEngine executable and build deterministic CLI arguments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * AuthorEngine is integrated through a process boundary, so Studio can reuse its capabilities without copying publishing logic into the IDE.
 */

#include "umicom/ai/authorengine.h"
#include "../base/value_archive_internal.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Validate AI command configuration before any field is copied or launched. */
UmiStatus umi_ai_authorengine_validate(const UmiAiAuthorEngineConfig *config)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->executable, '\0', sizeof(config->executable)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->workspace, '\0', sizeof(config->workspace)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->provider, '\0', sizeof(config->provider)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* AI operations require an executable, workspace and governed provider. */
    if (config == NULL || config->executable[0] == '\0' || config->workspace[0] == '\0' || config->provider[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Append one complete argument without concatenating or shell-escaping text. */
static UmiStatus invocation_add(UmiAiAuthorEngineInvocation *invocation,
                                const char *argument)
{
    int written;
    /* Reject missing storage and text before either value is dereferenced. */
    if (invocation == NULL || argument == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the fixed boundary so malformed requests cannot overrun argv. */
    if (invocation->argument_count >= UMI_AI_AUTHOR_ENGINE_ARGUMENT_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(
        invocation->arguments[invocation->argument_count],
        sizeof(invocation->arguments[0]),
        "%s",
        argument);
    /* Truncation is an error because a changed path could select another file. */
    if (written < 0 ||
        (size_t)written >= sizeof(invocation->arguments[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++invocation->argument_count;
    return UMI_STATUS_OK;
}

/* Copy executable and working directory into a self-contained launch plan. */
static UmiStatus invocation_begin(
    const UmiAiAuthorEngineConfig *config,
    UmiAiAuthorEngineCommand command,
    UmiAiAuthorEngineInvocation *out_invocation)
{
    int written;
    /* Publication configuration needs a tool and project directory, but no AI provider. */
    if (config == NULL || out_invocation == NULL ||
        config->executable[0] == '\0' || config->workspace[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_invocation, 0, sizeof(*out_invocation));
    written = snprintf(out_invocation->executable,
                       sizeof(out_invocation->executable), "%s",
                       config->executable);
    /* Reject truncated executables because launch identity must remain exact. */
    if (written < 0 ||
        (size_t)written >= sizeof(out_invocation->executable)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(out_invocation->working_directory,
                       sizeof(out_invocation->working_directory), "%s",
                       config->workspace);
    /* Reject truncated workspaces so generated files cannot enter a wrong folder. */
    if (written < 0 ||
        (size_t)written >= sizeof(out_invocation->working_directory)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    out_invocation->command = command;
    return UMI_STATUS_OK;
}

/* Map supported publication operations to the exact command understood by the tool. */
static const char *publication_command_text(UmiAiAuthorEngineCommand command)
{
    /* An explicit switch prevents an unknown enumeration from becoming a command. */
    switch (command) {
        case UMI_AI_AUTHOR_ENGINE_PROJECT_INIT: return "init";
        case UMI_AI_AUTHOR_ENGINE_INGEST: return "ingest";
        case UMI_AI_AUTHOR_ENGINE_BUILD: return "build";
        case UMI_AI_AUTHOR_ENGINE_EXPORT: return "export";
        case UMI_AI_AUTHOR_ENGINE_SERVE_PREVIEW: return "serve";
        case UMI_AI_AUTHOR_ENGINE_OPEN_PREVIEW: return "open";
        case UMI_AI_AUTHOR_ENGINE_RENDER: return "render";
        case UMI_AI_AUTHOR_ENGINE_DOCTOR: return "doctor";
        case UMI_AI_AUTHOR_ENGINE_PUBLISH: return "publish";
        default: return NULL;
    }
}

/* Plan current book/document publication commands without copying their implementation. */
UmiStatus umi_ai_authorengine_plan_publication_invocation(
    const UmiAiAuthorEngineConfig *config,
    UmiAiAuthorEngineCommand command,
    const char *site_path,
    UmiAiAuthorEngineInvocation *out_invocation)
{
    const char *command_text = publication_command_text(command);
    UmiStatus status;
    /* Reject AI and unknown commands at this publication-specific boundary. */
    if (command_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = invocation_begin(config, command, out_invocation);
    /* Stop immediately when executable or working-directory copying failed. */
    if (status != UMI_STATUS_OK) return status;
    status = invocation_add(out_invocation, command_text);
    /* A site override is meaningful only to the preview server command. */
    if (status == UMI_STATUS_OK && site_path != NULL && site_path[0] != '\0') {
        /* Use the shared build helper when it is available from the parent composition. */
        if (command != UMI_AI_AUTHOR_ENGINE_SERVE_PREVIEW) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = invocation_add(out_invocation, "--site");
        /* Preserve the first capacity error instead of appending another value. */
        if (status == UMI_STATUS_OK) {
            status = invocation_add(out_invocation, site_path);
        }
    }
    return status;
}

/* Plan governed AI commands while keeping every argument separate and exact. */
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

    /* This entry point accepts only the three governed AI command kinds. */
    if (umi_ai_authorengine_validate(config) != UMI_STATUS_OK ||
        out_invocation == NULL ||
        command < UMI_AI_AUTHOR_ENGINE_HEALTH ||
        command > UMI_AI_AUTHOR_ENGINE_GENERATE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Generation requires complete session, model and file identities. */
    if (command == UMI_AI_AUTHOR_ENGINE_GENERATE &&
        (session_id == NULL || session_id[0] == '\0' ||
         model_id == NULL || model_id[0] == '\0' ||
         input_path == NULL || input_path[0] == '\0' ||
         output_path == NULL || output_path[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = invocation_begin(config, command, out_invocation);
    /* Do not construct argv when the launch identity could not be copied. */
    if (status != UMI_STATUS_OK) return status;
    /* The nested selection retains the established external command spelling. */
    command_text = command == UMI_AI_AUTHOR_ENGINE_HEALTH
        ? "health"
        : (command == UMI_AI_AUTHOR_ENGINE_CATALOGUE
               ? "providers" : "generate");
    status = invocation_add(out_invocation, command_text);
    /* AI commands pass workspace and provider explicitly for future tool versions. */
    if (status == UMI_STATUS_OK) {
        status = invocation_add(out_invocation, "--workspace");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = invocation_add(out_invocation, config->workspace);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = invocation_add(out_invocation, "--provider");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = invocation_add(out_invocation, config->provider);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, "--session");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, session_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, "--model");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, model_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, "--input");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, input_path);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, "--output");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && command == UMI_AI_AUTHOR_ENGINE_GENERATE) {
        status = invocation_add(out_invocation, output_path);
    }
    return status;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAiAuthorEngineConfigArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcc23aed75cf961ef);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiAuthorEngineConfig *)0)->executable)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiAuthorEngineConfig *)0)->workspace)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiAuthorEngineConfig *)0)->provider)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAiAuthorEngineConfigArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAiAuthorEngineConfig *)0)->executable) - 1U +
        8U + sizeof(((UmiAiAuthorEngineConfig *)0)->workspace) - 1U +
        8U + sizeof(((UmiAiAuthorEngineConfig *)0)->provider) - 1U;
}
static void UmiAiAuthorEngineConfigArchiveWrite(UmiArchiveWriter *writer, const UmiAiAuthorEngineConfig *value)
{
    UmiArchiveWriteText(writer, value->executable, sizeof(value->executable));
    UmiArchiveWriteText(writer, value->workspace, sizeof(value->workspace));
    UmiArchiveWriteText(writer, value->provider, sizeof(value->provider));
}
static void UmiAiAuthorEngineConfigArchiveRead(UmiArchiveReader *reader, UmiAiAuthorEngineConfig *value)
{
    UmiArchiveReadText(reader, value->executable, sizeof(value->executable));
    UmiArchiveReadText(reader, value->workspace, sizeof(value->workspace));
    UmiArchiveReadText(reader, value->provider, sizeof(value->provider));
}
static UmiStatus UmiAiAuthorEngineConfigArchiveValidate(const UmiAiAuthorEngineConfig *value)
{
    return umi_ai_authorengine_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ai_authorengine_archive_encode, umi_ai_authorengine_archive_decode,
    UmiAiAuthorEngineConfig, UmiAiAuthorEngineConfigArchiveSchema, UmiAiAuthorEngineConfigArchiveBound, UmiAiAuthorEngineConfigArchiveWrite, UmiAiAuthorEngineConfigArchiveRead, UmiAiAuthorEngineConfigArchiveValidate)
