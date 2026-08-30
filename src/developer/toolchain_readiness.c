/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/toolchain_readiness.c
 *
 * PURPOSE:
 *   Implement installed-tool readiness over existing language toolchain bindings and safe command parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/toolchain_readiness.h"

#include <ctype.h>
#include <string.h>

static void copy_candidate(
    char *destination,
    size_t capacity,
    const char *begin,
    size_t length)
{
    if (destination == NULL || capacity == 0U) {
        return;
    }

    if (length >= capacity) {
        length = capacity - 1U;
    }

    if (length > 0U) {
        (void)memcpy(destination, begin, length);
    }
    destination[length] = '\0';
}

static int default_program_probe(const char *program, void *user_data)
{
    char path[UMI_DEVELOPER_PATH_CAPACITY];
    (void)user_data;

    if (program == NULL || program[0] == '\0') {
        return 0;
    }

    return umi_toolchain_find_on_path(
               program,
               path,
               sizeof(path)) == UMI_STATUS_OK;
}

static int candidate_available(
    const char *candidate,
    UmiDeveloperToolAvailabilityProbe probe,
    void *user_data)
{
    UmiDeveloperCommandLine command;

    if (candidate == NULL || candidate[0] == '\0') {
        return 0;
    }

    /*
     * "native" denotes the built executable itself rather than a runtime
     * program that must be discovered on PATH.
     */
    if (strcmp(candidate, "native") == 0) {
        return 1;
    }

    if (umi_developer_command_line_parse(
            candidate,
            &command) != UMI_STATUS_OK) {
        return 0;
    }

    if (probe == NULL) {
        return default_program_probe(command.program, NULL);
    }

    return probe(command.program, user_data) != 0;
}

int umi_developer_toolchain_command_available(
    const char *command_candidates,
    UmiDeveloperToolAvailabilityProbe probe,
    void *user_data)
{
    const char *cursor;

    if (command_candidates == NULL) {
        return 0;
    }

    cursor = command_candidates;

    while (*cursor != '\0') {
        const char *begin;
        const char *end;
        char candidate[UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY];
        size_t length;

        while (*cursor == ';' ||
               isspace((unsigned char)*cursor)) {
            ++cursor;
        }

        if (*cursor == '\0') {
            break;
        }

        begin = cursor;
        while (*cursor != '\0' && *cursor != ';') {
            ++cursor;
        }
        end = cursor;

        while (end > begin &&
               isspace((unsigned char)end[-1])) {
            --end;
        }

        length = (size_t)(end - begin);
        if (length == 0U) {
            continue;
        }

        copy_candidate(
            candidate,
            sizeof(candidate),
            begin,
            length);

        if (candidate_available(
                candidate,
                probe,
                user_data)) {
            return 1;
        }
    }

    return 0;
}

static int operation_readiness(
    const UmiDeveloperToolchainBindingSnapshot *binding,
    UmiLanguageCapabilityFlags capability,
    const char *command_candidates,
    UmiDeveloperToolAvailabilityProbe probe,
    void *user_data,
    size_t *supported_count,
    size_t *ready_count,
    size_t *missing_count)
{
    if ((binding->capabilities & capability) != capability) {
        return 0;
    }

    *supported_count += 1U;

    if (umi_developer_toolchain_command_available(
            command_candidates,
            probe,
            user_data)) {
        *ready_count += 1U;
        return 1;
    }

    *missing_count += 1U;
    return 0;
}

UmiStatus umi_developer_toolchain_binding_readiness(
    const UmiDeveloperToolchainBindingSnapshot *binding,
    UmiDeveloperToolAvailabilityProbe probe,
    void *user_data,
    UmiDeveloperToolchainReadiness *out_readiness)
{
    if (binding == NULL ||
        out_readiness == NULL ||
        binding->id[0] == '\0' ||
        binding->language_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_readiness, 0, sizeof(*out_readiness));
    out_readiness->struct_size =
        (uint32_t)sizeof(*out_readiness);

    (void)strncpy(
        out_readiness->binding_id,
        binding->id,
        sizeof(out_readiness->binding_id) - 1U);
    (void)strncpy(
        out_readiness->language_id,
        binding->language_id,
        sizeof(out_readiness->language_id) - 1U);

#define EVALUATE(field, capability, command) \
    out_readiness->field = operation_readiness( \
        binding, \
        capability, \
        command, \
        probe, \
        user_data, \
        &out_readiness->supported_operation_count, \
        &out_readiness->ready_operation_count, \
        &out_readiness->missing_operation_count)

    EVALUATE(
        build_ready,
        UMI_LANGUAGE_CAPABILITY_BUILD,
        binding->compiler);
    EVALUATE(
        run_ready,
        UMI_LANGUAGE_CAPABILITY_RUN,
        binding->runtime);
    EVALUATE(
        debug_ready,
        UMI_LANGUAGE_CAPABILITY_DEBUG,
        binding->debugger);
    EVALUATE(
        test_ready,
        UMI_LANGUAGE_CAPABILITY_TEST,
        binding->test_runner);
    EVALUATE(
        format_ready,
        UMI_LANGUAGE_CAPABILITY_FORMAT,
        binding->formatter);
    EVALUATE(
        lint_ready,
        UMI_LANGUAGE_CAPABILITY_LINT,
        binding->linter);
    EVALUATE(
        package_ready,
        UMI_LANGUAGE_CAPABILITY_PACKAGE,
        binding->package_tool);

#undef EVALUATE

    return UMI_STATUS_OK;
}
