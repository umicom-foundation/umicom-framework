/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/job_identity.c
 * PURPOSE: Bind saved job outcomes to canonical project and profile settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/sha256.h"
#include "umicom/build/job_history.h"
#include "umicom/platform/path.h"
#include <string.h>

/* Length framing distinguishes ["ab", "c"] from ["a", "bc"]. Fixed byte order
 * also keeps evidence stable across machine word sizes and struct padding.
 * Extend the labelled schema whenever a new profile setting changes execution. */
static UmiStatus Number(UmiSha256 *hash, uint64_t value)
{
    unsigned char bytes[8];
    for (size_t index = 0U; index < sizeof(bytes); ++index)
        bytes[sizeof(bytes) - 1U - index] = (unsigned char)(value >> (index * 8U));
    return UmiSha256Update(hash, bytes, sizeof(bytes));
}
static UmiStatus Text(UmiSha256 *hash, const char *text)
{
    size_t length = strlen(text);
    UmiStatus status = Number(hash, (uint64_t)length);
    return status == UMI_STATUS_OK ? UmiSha256Update(hash, text, length) : status;
}
static UmiStatus Digest(UmiSha256 *hash, char out[UMI_JOB_IDENTITY_DIGEST_CAPACITY])
{
    unsigned char bytes[UMI_SHA256_BYTES];
    UmiStatus status = UmiSha256Final(hash, bytes);
    if (status == UMI_STATUS_OK)
        UmiSha256Hex(bytes, out);
    return status;
}
UmiStatus UmiBuildProfileJobIdentity(const UmiBuildProfile *profile, UmiJobIdentity *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK)
        return status;
    char root[UMI_BUILD_PATH_CAPACITY];
    status = UmiBuildProfileSourceDirectory(profile, root, sizeof(root));
    if (status != UMI_STATUS_OK)
        return status;
    UmiJobIdentity identity = {0};
    UmiSha256 subject, configuration;
    UmiSha256Init(&subject);
    UmiSha256Init(&configuration);
    status = Text(&subject, "umicom.build.project");
    if (status == UMI_STATUS_OK)
        status = Text(&subject, root);
    if (status == UMI_STATUS_OK)
        status = Digest(&subject, identity.subject);
    /* This schema describes reviewed profile values, not files, environment,
     * compiler binaries or mutable preset contents. A settings match is useful
     * context, but cannot qualify today's source or authorise another launch. */
    const char *fields[] = {"umicom.build.profile",
                            identity.subject,
                            profile->profile_id,
                            profile->build_directory,
                            profile->generator,
                            profile->compiler,
                            profile->configuration,
                            profile->preset,
                            profile->build_target,
                            profile->run_program,
                            profile->run_argument,
                            profile->install_directory,
                            profile->run_arguments,
                            profile->configure_preset,
                            profile->build_preset,
                            profile->test_preset,
                            profile->run_working_directory};
    for (size_t index = 0U; status == UMI_STATUS_OK && index < sizeof(fields) / sizeof(fields[0]);
         ++index)
        status = Text(&configuration, fields[index]);
    /* An explicit executable source must change saved-job identity. Empty
     * keeps the established identity for older profiles using inherited lookup. */
    if (status == UMI_STATUS_OK && profile->tool_directory[0] != '\0') {
        status = Text(&configuration, "tool-directory");
        if (status == UMI_STATUS_OK) status = Text(&configuration, profile->tool_directory);
    }
    /* Keep old empty-option identities stable, but never equate jobs configured
     * with different reviewed SDK prefixes or project feature selections. */
    if (status == UMI_STATUS_OK && profile->configure_definitions[0] != '\0') {
        status = Text(&configuration, "configure-definitions");
        if (status == UMI_STATUS_OK) status = Text(&configuration, profile->configure_definitions);
    }
    /* Keep empty legacy identities stable while binding explicit run settings
     * to the reviewed job. This digest does not capture the inherited environment. */
    if (status == UMI_STATUS_OK && profile->run_environment[0] != '\0') {
        status = Text(&configuration, "run-environment");
        if (status == UMI_STATUS_OK) status = Text(&configuration, profile->run_environment);
    }
    if (status == UMI_STATUS_OK)
        status = Number(&configuration, profile->parallel_jobs);
    if (status == UMI_STATUS_OK)
        status = Number(&configuration, profile->timeout_ms);
    if (status == UMI_STATUS_OK)
        status = Number(&configuration, (uint64_t)(profile->build_testing != 0));
    if (status == UMI_STATUS_OK)
        status = Number(&configuration, (uint64_t)(profile->strict_warnings != 0));
    if (status == UMI_STATUS_OK)
        status = Digest(&configuration, identity.configuration);
    if (status == UMI_STATUS_OK)
        *out = identity;
    return status;
}
