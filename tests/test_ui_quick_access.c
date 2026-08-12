/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_quick_access.c
 *
 * PURPOSE:
 *   Exercise one Batch 23 unified-workbench contract through its public C API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These tests are intentionally small. They document expected behaviour without starting a graphical application.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static UmiStatus handler(void *user_data,
                         const char *argument,
                         char *out_message,
                         size_t capacity)
{
    (void)user_data;
    (void)argument;
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", "opened");
    }
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiCommandDescriptor descriptor = {0};
    UmiUiQuickAccessResults results;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = "studio.file.quick-open";
    descriptor.title = "Quick Open File";
    descriptor.category = "File";
    descriptor.description = "Fuzzy-open a workspace file";
    descriptor.handler = handler;
    assert(umi_command_registry_register(commands, &descriptor) == UMI_STATUS_OK);

    assert(umi_ui_quick_access_search_commands(commands, "quick",
                                                &results) == UMI_STATUS_OK);
    assert(results.count == 1U);
    assert(strcmp(results.items[0].command_id, "studio.file.quick-open") == 0);

    umi_command_registry_destroy(commands);
    return 0;
}
