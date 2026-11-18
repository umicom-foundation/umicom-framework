/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_completion.inc
 *
 * PURPOSE:
 *   Format bounded terminal process evidence without hiding loader or child exit codes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdbool.h>

UmiStatus UmiProcessJobFormatCompletion(const UmiProcessJobSnapshot *snapshot,
    char *outText, size_t capacity)
{
    char message[512];
    const char *hint = "";
    const char *state;
    if (snapshot == NULL || outText == NULL || capacity == 0U ||
        memchr(snapshot->output, '\0', sizeof(snapshot->output)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (snapshot->state < UMI_PROCESS_JOB_SUCCEEDED ||
        snapshot->state > UMI_PROCESS_JOB_TIMED_OUT)
        return UMI_STATUS_INVALID_STATE;
    state = umi_process_job_state_text(snapshot->state);
    switch ((uint32_t)snapshot->exit_code) {
    case UINT32_C(0xc0000135): hint = " Windows reports a missing DLL."; break;
    case UINT32_C(0xc000007b): hint = " Windows reports an invalid executable/DLL format."; break;
    case UINT32_C(0xc0000142): hint = " Windows reports DLL initialization failure."; break;
    case UINT32_C(0xc0000005): hint = " Windows reports an access violation."; break;
    default: break;
    }
    int written = snprintf(message, sizeof(message),
        "Process %s; exit %d (0x%08lX).%s", state, snapshot->exit_code,
        (unsigned long)(uint32_t)snapshot->exit_code, hint);
    if (written < 0 || (size_t)written >= sizeof(message)) return UMI_STATUS_INTERNAL_ERROR;
    size_t used = (size_t)written;
    size_t length = strlen(snapshot->output);
    if (length != 0U) {
        const char *notice = " [diagnostic tail truncated]";
        size_t available = sizeof(message) - used - strlen(notice) - 3U;
        bool truncated = snapshot->output_truncated || length > available;
        const char *tail = snapshot->output + (length > available ? length - available : 0U);
        (void)snprintf(message + used, sizeof(message) - used, "\n%s%s",
            truncated ? notice : "", tail);
    }
    size_t required = strlen(message) + 1U;
    if (required > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(outText, message, required);
    return UMI_STATUS_OK;
}
