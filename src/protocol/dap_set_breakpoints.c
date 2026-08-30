/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_set_breakpoints.c
 *
 * PURPOSE:
 *   Implement the dap set breakpoints behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework DAP setBreakpoints builder.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "dap_request_internal.h"
#include <stdio.h>
#include <string.h>

UmiStatus umi_dap_set_breakpoints(UmiDapClient *client,
                                  const char *source_path,
                                  const UmiDapSourceBreakpoint *items,
                                  size_t count,
                                  int64_t *out_request_id)
{
    char path[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char arguments[UMI_PROTOCOL_MESSAGE_CAPACITY];
    size_t used = 0U;
    size_t index;
    int written;
    UmiStatus status;
    if (source_path == NULL || (count > 0U && items == NULL) || count > 256U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_dap_escape(source_path, path, sizeof(path));
    if (status != UMI_STATUS_OK) return status;
    written = snprintf(arguments, sizeof(arguments),
                       "{\"source\":{\"path\":\"%s\"},\"breakpoints\":[", path);
    if (written < 0 || (size_t)written >= sizeof(arguments)) return UMI_STATUS_CAPACITY_EXCEEDED;
    used = (size_t)written;
    for (index = 0U; index < count; ++index) {
        char condition[1024], hit[512], log_message[1024];
        status = umi_dap_escape(items[index].condition != NULL ? items[index].condition : "", condition, sizeof(condition));
        if (status == UMI_STATUS_OK) status = umi_dap_escape(items[index].hit_condition != NULL ? items[index].hit_condition : "", hit, sizeof(hit));
        if (status == UMI_STATUS_OK) status = umi_dap_escape(items[index].log_message != NULL ? items[index].log_message : "", log_message, sizeof(log_message));
        if (status != UMI_STATUS_OK) return status;
        written = snprintf(arguments + used, sizeof(arguments) - used,
                           "%s{\"line\":%d,\"column\":%d,\"condition\":\"%s\","
                           "\"hitCondition\":\"%s\",\"logMessage\":\"%s\"}",
                           index == 0U ? "" : ",", items[index].line,
                           items[index].column, condition, hit, log_message);
        if (written < 0 || (size_t)written >= sizeof(arguments) - used) return UMI_STATUS_CAPACITY_EXCEEDED;
        used += (size_t)written;
    }
    if (used + 3U > sizeof(arguments)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(arguments + used, "]}", 3U);
    return umi_dap_request(client, "setBreakpoints", arguments, out_request_id);
}
