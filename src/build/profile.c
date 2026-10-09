/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/profile.c
 *
 * PURPOSE:
 *   Implement deterministic defaults, assignment and validation for build profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/profile.h"
#include "umicom/build/configure_definitions.h"
#include "umicom/platform/path.h"
#include "umicom/platform/process_search_path.h"
#include "umicom/platform/process_environment.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    if (destination == NULL || source == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(source) + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, strlen(source) + 1U);
    return UMI_STATUS_OK;
}

void umi_build_profile_init(UmiBuildProfile *profile)
{
    if (profile == NULL) {
        return;
    }
    (void)memset(profile, 0, sizeof(*profile));
    (void)copy_text(profile->profile_id,
                    sizeof(profile->profile_id),
                    "default");
    (void)copy_text(profile->source_directory,
                    sizeof(profile->source_directory),
                    ".");
    (void)copy_text(profile->build_directory,
                    sizeof(profile->build_directory),
                    "build/default");
    (void)copy_text(profile->install_directory,
                    sizeof(profile->install_directory),
                    "build/default/install");
    (void)copy_text(profile->generator,
                    sizeof(profile->generator),
                    "Ninja");
    (void)copy_text(profile->configuration,
                    sizeof(profile->configuration),
                    "Debug");
    profile->parallel_jobs = 1U;
    profile->timeout_ms = 0U;
    profile->build_testing = 1;
    profile->strict_warnings = 1;
}

UmiStatus umi_build_profile_set(UmiBuildProfile *profile,
                                const char *profile_id,
                                const char *source_directory,
                                const char *build_directory)
{
    UmiStatus status;
    if (profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_build_profile_init(profile);
    status = copy_text(profile->profile_id,
                       sizeof(profile->profile_id),
                       profile_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(profile->source_directory,
                           sizeof(profile->source_directory),
                           source_directory);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(profile->build_directory,
                           sizeof(profile->build_directory),
                           build_directory);
    }
    return status;
}

/* Old settings contain a single literal value; only the new field opts into
 * quoted parsing. Centralising this decision prevents Debug from interpreting
 * a saved literal differently from Run. */
UmiStatus UmiBuildProfileArguments(const UmiBuildProfile *profile, UmiArguments *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (profile == NULL ||
        memchr(profile->run_argument, '\0', sizeof(profile->run_argument)) == NULL ||
        memchr(profile->run_arguments, '\0', sizeof(profile->run_arguments)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (profile->run_argument[0] != '\0' && profile->run_arguments[0] != '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (profile->run_arguments[0] == '\0') {
        if (profile->run_argument[0] != '\0') {
            strcpy(out->storage[0], profile->run_argument);
            out->values[0] = out->storage[0]; out->count = 1U;
        }
        return UMI_STATUS_OK;
    }
    UmiStatus status = UmiArgumentsParse(profile->run_arguments, out);
    for (size_t index = 0U; status == UMI_STATUS_OK && index < out->count; ++index)
        if (strlen(out->values[index]) >= UMI_BUILD_ARGUMENT_CAPACITY)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status != UMI_STATUS_OK) memset(out, 0, sizeof(*out));
    return status;
}

/* Profiles now describe an explicit tool folder in addition to the existing source and launch inputs. Preserve the former validation contract for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_profile_validate(const UmiBuildProfile *profile,
                                     char *out_message,
                                     size_t message_capacity)
{
    const char *message = "Build profile is valid";
    UmiStatus status = UMI_STATUS_OK;
    if (profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* The profile can arrive from a plug-in or edited settings. Never use a
     * fixed-size member as a C string until its terminator is inside the field. */
    if (memchr(profile->profile_id, '\0', sizeof(profile->profile_id)) == NULL ||
        memchr(profile->source_directory, '\0', sizeof(profile->source_directory)) == NULL ||
        memchr(profile->build_directory, '\0', sizeof(profile->build_directory)) == NULL ||
        memchr(profile->generator, '\0', sizeof(profile->generator)) == NULL ||
        memchr(profile->compiler, '\0', sizeof(profile->compiler)) == NULL ||
        memchr(profile->configuration, '\0', sizeof(profile->configuration)) == NULL ||
        memchr(profile->preset, '\0', sizeof(profile->preset)) == NULL ||
        memchr(profile->build_target, '\0', sizeof(profile->build_target)) == NULL ||
        memchr(profile->run_program, '\0', sizeof(profile->run_program)) == NULL ||
        memchr(profile->run_argument, '\0', sizeof(profile->run_argument)) == NULL ||
        memchr(profile->install_directory, '\0', sizeof(profile->install_directory)) == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "A build-profile text field is not terminated";
    } else if (profile->profile_id[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build profile identifier is empty";
    } else if (profile->source_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build source directory is empty";
    } else if (profile->build_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build output directory is empty";
    } else if (profile->generator[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build generator is empty";
    } else if (profile->install_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build install directory is empty";
    } else if (profile->configuration[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build configuration is empty";
    } else if (profile->parallel_jobs == 0U) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Parallel job count must be greater than zero";
    }
    if (status == UMI_STATUS_OK &&
        memchr(profile->run_working_directory, '\0', sizeof(profile->run_working_directory)) == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "The program working directory is not terminated";
    }
#ifdef _WIN32
    /* A drive-relative or rooted-without-drive folder depends on process state.
     * Require a full Windows root or an ordinary project-relative path. */
    if (status == UMI_STATUS_OK && profile->run_working_directory[0] != '\0' &&
        !umi_path_is_absolute(profile->run_working_directory) &&
        (profile->run_working_directory[0] == '/' || profile->run_working_directory[0] == '\\' ||
         strchr(profile->run_working_directory, ':') != NULL)) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Use a full drive or network path, or a folder relative to the project";
    }
#endif
    /* Stage selections are explicit values, not executable command fragments.
     * Reject truncated fields and competing legacy/stage choices before a
     * provider can construct an argument vector from this profile. */
    if (status == UMI_STATUS_OK) {
        if (memchr(profile->configure_preset, '\0', sizeof(profile->configure_preset)) == NULL ||
            memchr(profile->build_preset, '\0', sizeof(profile->build_preset)) == NULL ||
            memchr(profile->test_preset, '\0', sizeof(profile->test_preset)) == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            message = "A stage preset name is not terminated";
        } else if (profile->preset[0] != '\0' && (profile->configure_preset[0] != '\0' ||
                   profile->build_preset[0] != '\0' || profile->test_preset[0] != '\0')) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            message = "Clear the shared CMake preset before selecting separate stage presets";
        }
    }
    if (status == UMI_STATUS_OK) {
        UmiArguments arguments;
        status = UmiBuildProfileArguments(profile, &arguments);
        if (status != UMI_STATUS_OK)
            message = "Use either the single literal argument or the quoted argument list; check quotes, argument lengths and count";
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", message);
    }
    return status;
}
#endif
/* CMake definitions now travel with the reviewed profile and must be validated before saving or submitting a job. Retain the preceding profile validator for contract review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_profile_validate(const UmiBuildProfile *profile,
                                     char *out_message,
                                     size_t message_capacity)
{
    const char *message = "Build profile is valid";
    UmiStatus status = UMI_STATUS_OK;
    if (profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* The profile can arrive from a plug-in or edited settings. Never use a
     * fixed-size member as a C string until its terminator is inside the field. */
    if (memchr(profile->profile_id, '\0', sizeof(profile->profile_id)) == NULL ||
        memchr(profile->source_directory, '\0', sizeof(profile->source_directory)) == NULL ||
        memchr(profile->build_directory, '\0', sizeof(profile->build_directory)) == NULL ||
        memchr(profile->generator, '\0', sizeof(profile->generator)) == NULL ||
        memchr(profile->compiler, '\0', sizeof(profile->compiler)) == NULL ||
        memchr(profile->configuration, '\0', sizeof(profile->configuration)) == NULL ||
        memchr(profile->preset, '\0', sizeof(profile->preset)) == NULL ||
        memchr(profile->build_target, '\0', sizeof(profile->build_target)) == NULL ||
        memchr(profile->run_program, '\0', sizeof(profile->run_program)) == NULL ||
        memchr(profile->run_argument, '\0', sizeof(profile->run_argument)) == NULL ||
        memchr(profile->install_directory, '\0', sizeof(profile->install_directory)) == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "A build-profile text field is not terminated";
    } else if (profile->profile_id[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build profile identifier is empty";
    } else if (profile->source_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build source directory is empty";
    } else if (profile->build_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build output directory is empty";
    } else if (profile->generator[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build generator is empty";
    } else if (profile->install_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build install directory is empty";
    } else if (profile->configuration[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build configuration is empty";
    } else if (profile->parallel_jobs == 0U) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Parallel job count must be greater than zero";
    }
    if (status == UMI_STATUS_OK &&
        memchr(profile->run_working_directory, '\0', sizeof(profile->run_working_directory)) == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "The program working directory is not terminated";
    }
#ifdef _WIN32
    /* A drive-relative or rooted-without-drive folder depends on process state.
     * Require a full Windows root or an ordinary project-relative path. */
    if (status == UMI_STATUS_OK && profile->run_working_directory[0] != '\0' &&
        !umi_path_is_absolute(profile->run_working_directory) &&
        (profile->run_working_directory[0] == '/' || profile->run_working_directory[0] == '\\' ||
         strchr(profile->run_working_directory, ':') != NULL)) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Use a full drive or network path, or a folder relative to the project";
    }
#endif
    /* Stage selections are explicit values, not executable command fragments.
     * Reject truncated fields and competing legacy/stage choices before a
     * provider can construct an argument vector from this profile. */
    if (status == UMI_STATUS_OK) {
        if (memchr(profile->configure_preset, '\0', sizeof(profile->configure_preset)) == NULL ||
            memchr(profile->build_preset, '\0', sizeof(profile->build_preset)) == NULL ||
            memchr(profile->test_preset, '\0', sizeof(profile->test_preset)) == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            message = "A stage preset name is not terminated";
        } else if (profile->preset[0] != '\0' && (profile->configure_preset[0] != '\0' ||
                   profile->build_preset[0] != '\0' || profile->test_preset[0] != '\0')) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            message = "Clear the shared CMake preset before selecting separate stage presets";
        }
    }
    if (status == UMI_STATUS_OK) {
        UmiArguments arguments;
        status = UmiBuildProfileArguments(profile, &arguments);
        if (status != UMI_STATUS_OK)
            message = "Use either the single literal argument or the quoted argument list; check quotes, argument lengths and count";
    }
    /* A tool folder changes executable selection and child lookup. Admit it
     * only as one complete absolute path; never accept a command or PATH list. */
    if (status == UMI_STATUS_OK) {
        if (memchr(profile->tool_directory, '\0', sizeof profile->tool_directory) == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
        } else if (profile->tool_directory[0] != '\0') {
            status = UmiProcessSearchDirectoryValidate(profile->tool_directory);
        }
        if (status != UMI_STATUS_OK)
            message = "Use one absolute tool-program folder, or leave it empty for inherited lookup";
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", message);
    }
    return status;
}
#endif
/* Launch environment overrides are now part of the reviewed profile. The shared platform validator keeps Run and Debug syntax consistent; the previous validator is retained for comparison. The previous implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_profile_validate(const UmiBuildProfile *profile,
                                     char *out_message,
                                     size_t message_capacity)
{
    const char *message = "Build profile is valid";
    UmiStatus status = UMI_STATUS_OK;
    if (profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* The profile can arrive from a plug-in or edited settings. Never use a
     * fixed-size member as a C string until its terminator is inside the field. */
    if (memchr(profile->profile_id, '\0', sizeof(profile->profile_id)) == NULL ||
        memchr(profile->source_directory, '\0', sizeof(profile->source_directory)) == NULL ||
        memchr(profile->build_directory, '\0', sizeof(profile->build_directory)) == NULL ||
        memchr(profile->generator, '\0', sizeof(profile->generator)) == NULL ||
        memchr(profile->compiler, '\0', sizeof(profile->compiler)) == NULL ||
        memchr(profile->configuration, '\0', sizeof(profile->configuration)) == NULL ||
        memchr(profile->preset, '\0', sizeof(profile->preset)) == NULL ||
        memchr(profile->build_target, '\0', sizeof(profile->build_target)) == NULL ||
        memchr(profile->run_program, '\0', sizeof(profile->run_program)) == NULL ||
        memchr(profile->run_argument, '\0', sizeof(profile->run_argument)) == NULL ||
        memchr(profile->install_directory, '\0', sizeof(profile->install_directory)) == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "A build-profile text field is not terminated";
    } else if (profile->profile_id[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build profile identifier is empty";
    } else if (profile->source_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build source directory is empty";
    } else if (profile->build_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build output directory is empty";
    } else if (profile->generator[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build generator is empty";
    } else if (profile->install_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build install directory is empty";
    } else if (profile->configuration[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build configuration is empty";
    } else if (profile->parallel_jobs == 0U) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Parallel job count must be greater than zero";
    }
    if (status == UMI_STATUS_OK &&
        memchr(profile->run_working_directory, '\0', sizeof(profile->run_working_directory)) == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "The program working directory is not terminated";
    }
#ifdef _WIN32
    /* A drive-relative or rooted-without-drive folder depends on process state.
     * Require a full Windows root or an ordinary project-relative path. */
    if (status == UMI_STATUS_OK && profile->run_working_directory[0] != '\0' &&
        !umi_path_is_absolute(profile->run_working_directory) &&
        (profile->run_working_directory[0] == '/' || profile->run_working_directory[0] == '\\' ||
         strchr(profile->run_working_directory, ':') != NULL)) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Use a full drive or network path, or a folder relative to the project";
    }
#endif
    /* Stage selections are explicit values, not executable command fragments.
     * Reject truncated fields and competing legacy/stage choices before a
     * provider can construct an argument vector from this profile. */
    if (status == UMI_STATUS_OK) {
        if (memchr(profile->configure_preset, '\0', sizeof(profile->configure_preset)) == NULL ||
            memchr(profile->build_preset, '\0', sizeof(profile->build_preset)) == NULL ||
            memchr(profile->test_preset, '\0', sizeof(profile->test_preset)) == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            message = "A stage preset name is not terminated";
        } else if (profile->preset[0] != '\0' && (profile->configure_preset[0] != '\0' ||
                   profile->build_preset[0] != '\0' || profile->test_preset[0] != '\0')) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            message = "Clear the shared CMake preset before selecting separate stage presets";
        }
    }
    if (status == UMI_STATUS_OK) {
        UmiArguments arguments;
        status = UmiBuildProfileArguments(profile, &arguments);
        if (status != UMI_STATUS_OK)
            message = "Use either the single literal argument or the quoted argument list; check quotes, argument lengths and count";
        if (status == UMI_STATUS_OK) {
            status = UmiBuildConfigureDefinitions(profile, &arguments);
            if (status != UMI_STATUS_OK)
                message = "Use quoted -DNAME[:TYPE]=VALUE definitions; check duplicate names, supported types, lengths and settings already owned by profile fields";
        }
    }
    /* A tool folder changes executable selection and child lookup. Admit it
     * only as one complete absolute path; never accept a command or PATH list. */
    if (status == UMI_STATUS_OK) {
        if (memchr(profile->tool_directory, '\0', sizeof profile->tool_directory) == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
        } else if (profile->tool_directory[0] != '\0') {
            status = UmiProcessSearchDirectoryValidate(profile->tool_directory);
        }
        if (status != UMI_STATUS_OK)
            message = "Use one absolute tool-program folder, or leave it empty for inherited lookup";
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", message);
    }
    return status;
}
#endif
UmiStatus umi_build_profile_validate(const UmiBuildProfile *profile,
                                     char *out_message,
                                     size_t message_capacity)
{
    const char *message = "Build profile is valid";
    UmiStatus status = UMI_STATUS_OK;
    if (profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* The profile can arrive from a plug-in or edited settings. Never use a
     * fixed-size member as a C string until its terminator is inside the field. */
    if (memchr(profile->profile_id, '\0', sizeof(profile->profile_id)) == NULL ||
        memchr(profile->source_directory, '\0', sizeof(profile->source_directory)) == NULL ||
        memchr(profile->build_directory, '\0', sizeof(profile->build_directory)) == NULL ||
        memchr(profile->generator, '\0', sizeof(profile->generator)) == NULL ||
        memchr(profile->compiler, '\0', sizeof(profile->compiler)) == NULL ||
        memchr(profile->configuration, '\0', sizeof(profile->configuration)) == NULL ||
        memchr(profile->preset, '\0', sizeof(profile->preset)) == NULL ||
        memchr(profile->build_target, '\0', sizeof(profile->build_target)) == NULL ||
        memchr(profile->run_program, '\0', sizeof(profile->run_program)) == NULL ||
        memchr(profile->run_argument, '\0', sizeof(profile->run_argument)) == NULL ||
        memchr(profile->install_directory, '\0', sizeof(profile->install_directory)) == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "A build-profile text field is not terminated";
    } else if (profile->profile_id[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build profile identifier is empty";
    } else if (profile->source_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build source directory is empty";
    } else if (profile->build_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build output directory is empty";
    } else if (profile->generator[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build generator is empty";
    } else if (profile->install_directory[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build install directory is empty";
    } else if (profile->configuration[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build configuration is empty";
    } else if (profile->parallel_jobs == 0U) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Parallel job count must be greater than zero";
    }
    if (status == UMI_STATUS_OK &&
        memchr(profile->run_working_directory, '\0', sizeof(profile->run_working_directory)) == NULL) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "The program working directory is not terminated";
    }
#ifdef _WIN32
    /* A drive-relative or rooted-without-drive folder depends on process state.
     * Require a full Windows root or an ordinary project-relative path. */
    if (status == UMI_STATUS_OK && profile->run_working_directory[0] != '\0' &&
        !umi_path_is_absolute(profile->run_working_directory) &&
        (profile->run_working_directory[0] == '/' || profile->run_working_directory[0] == '\\' ||
         strchr(profile->run_working_directory, ':') != NULL)) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Use a full drive or network path, or a folder relative to the project";
    }
#endif
    /* Stage selections are explicit values, not executable command fragments.
     * Reject truncated fields and competing legacy/stage choices before a
     * provider can construct an argument vector from this profile. */
    if (status == UMI_STATUS_OK) {
        if (memchr(profile->configure_preset, '\0', sizeof(profile->configure_preset)) == NULL ||
            memchr(profile->build_preset, '\0', sizeof(profile->build_preset)) == NULL ||
            memchr(profile->test_preset, '\0', sizeof(profile->test_preset)) == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            message = "A stage preset name is not terminated";
        } else if (profile->preset[0] != '\0' && (profile->configure_preset[0] != '\0' ||
                   profile->build_preset[0] != '\0' || profile->test_preset[0] != '\0')) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            message = "Clear the shared CMake preset before selecting separate stage presets";
        }
    }
    if (status == UMI_STATUS_OK) {
        UmiArguments arguments;
        status = UmiBuildProfileArguments(profile, &arguments);
        if (status != UMI_STATUS_OK)
            message = "Use either the single literal argument or the quoted argument list; check quotes, argument lengths and count";
        if (status == UMI_STATUS_OK) {
            status = UmiBuildConfigureDefinitions(profile, &arguments);
            if (status != UMI_STATUS_OK)
                message = "Use quoted -DNAME[:TYPE]=VALUE definitions; check duplicate names, supported types, lengths and settings already owned by profile fields";
        }
    }
    /* A tool folder changes executable selection and child lookup. Admit it
     * only as one complete absolute path; never accept a command or PATH list. */
    if (status == UMI_STATUS_OK) {
        if (memchr(profile->tool_directory, '\0', sizeof profile->tool_directory) == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
        } else if (profile->tool_directory[0] != '\0') {
            status = UmiProcessSearchDirectoryValidate(profile->tool_directory);
        }
        if (status != UMI_STATUS_OK)
            message = "Use one absolute tool-program folder, or leave it empty for inherited lookup";
    }
    /* Validate before persistence and submission; a malformed launch override
     * must not be silently ignored when a program starts under the debugger. */
    if (status == UMI_STATUS_OK) {
        status = memchr(profile->run_environment, '\0', sizeof profile->run_environment) == NULL
            ? UMI_STATUS_INVALID_ARGUMENT : UmiProcessEnvironmentValidate(profile->run_environment);
        if (status != UMI_STATUS_OK)
            message = "Use distinct NAME=VALUE entries; quote spaces, use portable names and valid UTF-8, and keep credentials out of saved settings";
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s", message);
    }
    return status;
}

/* Profile equality includes launch variables so a queued debugger request cannot acquire changed settings. Retain the preceding comparison for review. The previous implementation is retained for engineering review. */
#if 0
int umi_build_profile_equal(const UmiBuildProfile *left,
                            const UmiBuildProfile *right)
{
    if (umi_build_profile_validate(left, NULL, 0U) != UMI_STATUS_OK ||
        umi_build_profile_validate(right, NULL, 0U) != UMI_STATUS_OK) {
        return 0;
    }
    if (strcmp(left->run_working_directory, right->run_working_directory) != 0) return 0;
    /* A different tool source is a different execution profile. */
    if (strcmp(left->tool_directory, right->tool_directory) != 0) return 0;
    /* A different SDK prefix or project option describes a different build. */
    if (strcmp(left->configure_definitions, right->configure_definitions) != 0) return 0;
    if (strcmp(left->configure_preset, right->configure_preset) != 0 ||
        strcmp(left->build_preset, right->build_preset) != 0 ||
        strcmp(left->test_preset, right->test_preset) != 0) return 0;
    return strcmp(left->profile_id, right->profile_id) == 0 &&
           strcmp(left->source_directory, right->source_directory) == 0 &&
           strcmp(left->build_directory, right->build_directory) == 0 &&
           strcmp(left->generator, right->generator) == 0 &&
           strcmp(left->compiler, right->compiler) == 0 &&
           strcmp(left->configuration, right->configuration) == 0 &&
           strcmp(left->preset, right->preset) == 0 &&
           strcmp(left->build_target, right->build_target) == 0 &&
           strcmp(left->run_program, right->run_program) == 0 &&
           strcmp(left->run_argument, right->run_argument) == 0 &&
           strcmp(left->run_arguments, right->run_arguments) == 0 &&
           strcmp(left->install_directory, right->install_directory) == 0 &&
           left->parallel_jobs == right->parallel_jobs &&
           left->timeout_ms == right->timeout_ms &&
           left->build_testing == right->build_testing &&
           left->strict_warnings == right->strict_warnings;
}
#endif
int umi_build_profile_equal(const UmiBuildProfile *left,
                            const UmiBuildProfile *right)
{
    if (umi_build_profile_validate(left, NULL, 0U) != UMI_STATUS_OK ||
        umi_build_profile_validate(right, NULL, 0U) != UMI_STATUS_OK) {
        return 0;
    }
    /* A changed launch environment invalidates pending work using the old settings. */
    if (strcmp(left->run_environment, right->run_environment) != 0) return 0;
    if (strcmp(left->run_working_directory, right->run_working_directory) != 0) return 0;
    /* A different tool source is a different execution profile. */
    if (strcmp(left->tool_directory, right->tool_directory) != 0) return 0;
    /* A different SDK prefix or project option describes a different build. */
    if (strcmp(left->configure_definitions, right->configure_definitions) != 0) return 0;
    if (strcmp(left->configure_preset, right->configure_preset) != 0 ||
        strcmp(left->build_preset, right->build_preset) != 0 ||
        strcmp(left->test_preset, right->test_preset) != 0) return 0;
    return strcmp(left->profile_id, right->profile_id) == 0 &&
           strcmp(left->source_directory, right->source_directory) == 0 &&
           strcmp(left->build_directory, right->build_directory) == 0 &&
           strcmp(left->generator, right->generator) == 0 &&
           strcmp(left->compiler, right->compiler) == 0 &&
           strcmp(left->configuration, right->configuration) == 0 &&
           strcmp(left->preset, right->preset) == 0 &&
           strcmp(left->build_target, right->build_target) == 0 &&
           strcmp(left->run_program, right->run_program) == 0 &&
           strcmp(left->run_argument, right->run_argument) == 0 &&
           strcmp(left->run_arguments, right->run_arguments) == 0 &&
           strcmp(left->install_directory, right->install_directory) == 0 &&
           left->parallel_jobs == right->parallel_jobs &&
           left->timeout_ms == right->timeout_ms &&
           left->build_testing == right->build_testing &&
           left->strict_warnings == right->strict_warnings;
}

/* Resolve once from an explicit project root. This shared operation keeps Run
 * and Debug independent of Studio's own process directory and never creates
 * folders as a side effect of reviewing settings. */
UmiStatus UmiBuildProfileLaunchDirectory(const UmiBuildProfile *profile,
    const char *absoluteProjectDirectory, char *outDirectory, size_t capacity)
{
    char resolved[UMI_BUILD_PATH_CAPACITY];
    if (outDirectory == NULL || capacity == 0U || absoluteProjectDirectory == NULL ||
        !umi_path_is_absolute(absoluteProjectDirectory)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    const char *directory = profile->run_working_directory[0] == '\0'
        ? absoluteProjectDirectory : profile->run_working_directory;
    status = umi_path_absolute(directory, absoluteProjectDirectory, resolved, sizeof(resolved));
    if (status != UMI_STATUS_OK) return status;
    size_t length = strlen(resolved);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outDirectory, resolved, length + 1U);
    return UMI_STATUS_OK;
}
