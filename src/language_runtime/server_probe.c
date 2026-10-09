/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/server_probe.c
 * PURPOSE: Keep native language connection checks and cleanup in Framework rather than application callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/server_probe.h"
#include "server_profile_internal.h"
#include <string.h>

/* The explicit connection check now accepts the same copied tool installation used by other project jobs. Initialization, cancellation and shutdown reporting remain in one Framework implementation. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageRuntimeProbe(const UmiLanguageServerProfile *profile, const char *rootUri,
                                  const char *workingDirectory, uint32_t timeoutMs,
                                  const UmiCancellationToken *cancel, UmiLanguageRuntimeProbeResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    out->initializationStatus = UMI_STATUS_INVALID_ARGUMENT;
    out->shutdownStatus = UMI_STATUS_OK;
    if (LanguageProfileText(profile) != UMI_STATUS_OK || profile->executable[0] == '\0' || rootUri == NULL ||
        rootUri[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = !profile->enabled                             ? UMI_STATUS_UNAVAILABLE
                       : umi_cancellation_token_is_requested(cancel) ? UMI_STATUS_CANCELLED
                                                                     : UMI_STATUS_OK;
    UmiLanguageRuntimeServer *server = NULL;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_start("language-connection-check", profile, rootUri,
                                                   workingDirectory, &server);
    if (status == UMI_STATUS_OK)
    {
        out->launched = 1;
        status = UmiLanguageRuntimeServerInitialize(server, rootUri, timeoutMs, cancel, &out->capabilities);
        out->initialized = status == UMI_STATUS_OK;
        (void)umi_language_runtime_server_snapshot(server, &out->connection);
        if (out->initialized)
            out->shutdownStatus = UmiLanguageRuntimeServerShutdown(server, 250U);
        else
            out->shutdownStatus = umi_language_runtime_server_stop(server, 0U);
        umi_language_runtime_server_destroy(server);
    }
    out->initializationStatus = status;
    return status == UMI_STATUS_OK ? out->shutdownStatus : status;
}
#endif
UmiStatus UmiLanguageRuntimeProbeWithToolDirectory(const UmiLanguageServerProfile *profile, const char *rootUri,
                                  const char *workingDirectory, const char *toolDirectory, uint32_t timeoutMs,
                                  const UmiCancellationToken *cancel, UmiLanguageRuntimeProbeResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    out->initializationStatus = UMI_STATUS_INVALID_ARGUMENT;
    out->shutdownStatus = UMI_STATUS_OK;
    if (LanguageProfileText(profile) != UMI_STATUS_OK || profile->executable[0] == '\0' || rootUri == NULL ||
        rootUri[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = !profile->enabled                             ? UMI_STATUS_UNAVAILABLE
                       : umi_cancellation_token_is_requested(cancel) ? UMI_STATUS_CANCELLED
                                                                     : UMI_STATUS_OK;
    UmiLanguageRuntimeServer *server = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageRuntimeServerStartWithToolDirectory("language-connection-check", profile, rootUri,
                                                   workingDirectory, toolDirectory, &server);
    if (status == UMI_STATUS_OK)
    {
        out->launched = 1;
        status = UmiLanguageRuntimeServerInitialize(server, rootUri, timeoutMs, cancel, &out->capabilities);
        out->initialized = status == UMI_STATUS_OK;
        (void)umi_language_runtime_server_snapshot(server, &out->connection);
        if (out->initialized)
            out->shutdownStatus = UmiLanguageRuntimeServerShutdown(server, 250U);
        else
            out->shutdownStatus = umi_language_runtime_server_stop(server, 0U);
        umi_language_runtime_server_destroy(server);
    }
    out->initializationStatus = status;
    return status == UMI_STATUS_OK ? out->shutdownStatus : status;
}

/* Existing probes retain inherited tool selection. */
UmiStatus UmiLanguageRuntimeProbe(const UmiLanguageServerProfile *profile, const char *rootUri,
    const char *workingDirectory, uint32_t timeoutMs,
    const UmiCancellationToken *cancel, UmiLanguageRuntimeProbeResult *out)
{
    return UmiLanguageRuntimeProbeWithToolDirectory(profile, rootUri, workingDirectory,
        NULL, timeoutMs, cancel, out);
}
