/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/submodule_probe.c
 *
 * PURPOSE:
 *   Implement populate existing submodule maintenance counts from recursive submodule status.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/submodule_probe.h"

/* The former count-only probe treated arbitrary diagnostic lines as initialized
 * children and silently skipped oversized lines. Keep it for review. The active
 * implementation retains exact bytes and validates all records before publishing
 * counts, while keeping the established zero-on-probe-failure behavior. */
#if 0


#include "umicom/repository/submodule_probe.h"

#include "umicom/repository/git_command.h"
#include "umicom/repository/git_output.h"
/* Parse Git's leading submodule state marker and count only configured entries. */
UmiStatus umi_repository_submodule_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositorySubmoduleStatus *out_value)
{
    const char *args[] = {"submodule", "status", "--recursive"}; UmiRepositoryGitCommandResult result; UmiStatus status; size_t count, index; char line[4096];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_repository_submodule_status_init(out_value);
    status = umi_repository_git_command_execute(context, args, 3U, &result); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result.exit_code != 0 && result.output[0] == '\0') return UMI_STATUS_INVALID_STATE;
    count = umi_repository_git_output_line_count(result.output); out_value->configured = count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_repository_git_output_line_at(result.output, index, line, sizeof(line)) != UMI_STATUS_OK || line[0] == '\0') continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (line[0] == '-') ++out_value->missing;
        /* Use this fallback path when the earlier condition does not apply. */
        else { ++out_value->initialised; /* Use the stable identifier comparison to choose the matching record or policy. */ if (line[0] == '+') ++out_value->head_mismatch; /* Use the stable identifier comparison to choose the matching record or policy. */ if (line[0] == 'U') ++out_value->dirty; }
    }
    return UMI_STATUS_OK;
}


#endif

UmiStatus UmiRepositorySubmoduleCapture(const UmiRepositoryInspectionContext *context,
    const UmiCancellationToken *cancellation, UmiRepositoryGitCaptureResult *out)
{
    const char *arguments[] = {"--no-optional-locks", "-c", "core.quotepath=true",
        "-c", "color.ui=false", "submodule", "status", "--recursive"};
    return UmiRepositoryGitCapture(context, arguments,
        sizeof(arguments) / sizeof(arguments[0]), UMI_REPOSITORY_GIT_CAPTURE_LIMIT,
        cancellation, out);
}

static int SubmoduleHex(unsigned char byte)
{
    return (byte >= '0' && byte <= '9') || (byte >= 'a' && byte <= 'f') ||
        (byte >= 'A' && byte <= 'F');
}

UmiStatus UmiRepositorySubmoduleStatusParse(const unsigned char *bytes, size_t length,
    UmiRepositorySubmoduleStatus *out)
{
    if (out == NULL || (bytes == NULL && length != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_REPOSITORY_GIT_CAPTURE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiRepositorySubmoduleStatus next = {0};
    if (length != 0U && bytes[length - 1U] != '\n') return UMI_STATUS_PARSE_ERROR;
    size_t start = 0U;
    while (start < length) {
        size_t end = start;
        while (end < length && bytes[end] != '\n') ++end;
        size_t content_end = end;
        if (content_end > start && bytes[content_end - 1U] == '\r') --content_end;
        if (content_end == start) return UMI_STATUS_PARSE_ERROR;
        unsigned char marker = bytes[start];
        if (marker != ' ' && marker != '-' && marker != '+' && marker != 'U')
            return UMI_STATUS_PARSE_ERROR;
        size_t cursor = start + 1U, hash_start = cursor;
        while (cursor < content_end && SubmoduleHex(bytes[cursor])) ++cursor;
        size_t digits = cursor - hash_start;
        if ((digits != 40U && digits != 64U) || cursor >= content_end ||
            bytes[cursor++] != ' ' || cursor == content_end) return UMI_STATUS_PARSE_ERROR;
        /* Git's human-readable tail may include spaces and a describe suffix.
         * Preserve it as display evidence; do not invent a lossy path parser. */
        for (; cursor < content_end; ++cursor)
            if (bytes[cursor] < 0x20U || bytes[cursor] == 0x7fU) return UMI_STATUS_PARSE_ERROR;
        ++next.configured;
        if (marker == '-') ++next.missing;
        else {
            ++next.initialised;
            if (marker == '+') ++next.head_mismatch;
            if (marker == 'U') ++next.dirty;
        }
        start = end + 1U;
    }
    *out = next;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_submodule_probe_read(const UmiRepositoryInspectionContext *context,
    UmiRepositorySubmoduleStatus *out_value)
{
    if (context == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_repository_submodule_status_init(out_value);
    UmiRepositoryGitCaptureResult captured = {0};
    UmiStatus status = UmiRepositorySubmoduleCapture(context, NULL, &captured);
    if (status == UMI_STATUS_OK)
        status = UmiRepositorySubmoduleStatusParse(captured.bytes, captured.length, out_value);
    UmiRepositoryGitCaptureRelease(&captured);
    return status;
}
