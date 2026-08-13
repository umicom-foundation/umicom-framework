/*-----------------------------------------------------------------------------
 * Umicom Framework terminal link navigation projection.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/navigation.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_terminal_ui_link_navigation(const UmiTerminalLink *link,
                                          UmiUiNavigationEntrySnapshot *out_entry)
{
    size_t target_length;
    size_t label_length;
    if (link == NULL || out_entry == NULL || link->target[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    target_length = strlen(link->target);
    if (target_length + 1U > sizeof(out_entry->uri)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(out_entry, 0, sizeof(*out_entry));
    out_entry->struct_size = (uint32_t)sizeof(*out_entry);
    out_entry->api_version = UMI_UI_NAVIGATION_STACK_API_VERSION;
    (void)snprintf(out_entry->id, sizeof(out_entry->id),
                   "terminal-link-%zu", link->byte_offset);
    label_length = target_length < sizeof(out_entry->label) - 1U
        ? target_length : sizeof(out_entry->label) - 1U;
    (void)memcpy(out_entry->label, link->target, label_length);
    out_entry->label[label_length] = '\0';
    (void)memcpy(out_entry->uri, link->target, target_length + 1U);
    out_entry->line = link->line;
    out_entry->column = link->column;
    return UMI_STATUS_OK;
}
