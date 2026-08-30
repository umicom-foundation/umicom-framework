/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/command_catalogue.c
 *
 * PURPOSE:
 *   Implement the stable HTTP/API/Web/Cloud workbench command catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/command_catalogue.h"

#include <string.h>

#define COMMAND(id, label, category, trust, external)                         \
    {                                                                          \
        (uint32_t)sizeof(UmiWebWorkbenchCommandDescriptor),                    \
        UMI_WEB_WORKBENCH_API_VERSION, id, label, category, trust, external    \
    }

static const UmiWebWorkbenchCommandDescriptor COMMANDS[] = {
    COMMAND("web.workbench.request.new", "New HTTP Request", "HTTP", false, false),
    COMMAND("web.workbench.request.save", "Save HTTP Request", "HTTP", false, false),
    COMMAND("web.workbench.request.duplicate", "Duplicate HTTP Request", "HTTP", false, false),
    COMMAND("web.workbench.request.delete", "Delete HTTP Request", "HTTP", false, false),
    COMMAND("web.workbench.request.send", "Send HTTP Request", "HTTP", true, true),
    COMMAND("web.workbench.request.cancel", "Cancel HTTP Request", "HTTP", false, false),
    COMMAND("web.workbench.environment.new", "New API Environment", "Environment", false, false),
    COMMAND("web.workbench.environment.edit", "Edit API Environment", "Environment", false, false),
    COMMAND("web.workbench.environment.select", "Select API Environment", "Environment", false, false),
    COMMAND("web.workbench.auth.new", "New Authentication Profile", "Authentication", true, false),
    COMMAND("web.workbench.auth.edit", "Edit Authentication Profile", "Authentication", true, false),
    COMMAND("web.workbench.collection.import", "Import API Collection", "Collections", true, false),
    COMMAND("web.workbench.collection.export", "Export API Collection", "Collections", false, false),
    COMMAND("web.workbench.collection.run", "Run API Collection", "Collections", true, true),
    COMMAND("web.workbench.history.clear", "Clear API History", "History", false, false),
    COMMAND("web.workbench.history.replay", "Replay API Request", "History", true, true),
    COMMAND("web.workbench.openapi.import", "Import OpenAPI Document", "OpenAPI", true, false),
    COMMAND("web.workbench.openapi.refresh", "Refresh OpenAPI Document", "OpenAPI", true, true),
    COMMAND("web.workbench.openapi.filter", "Filter OpenAPI Operations", "OpenAPI", false, false),
    COMMAND("web.workbench.openapi.generate-request", "Generate Request from OpenAPI", "OpenAPI", false, false),
    COMMAND("web.workbench.soap.new", "New SOAP Request", "XML and SOAP", false, false),
    COMMAND("web.workbench.soap.send", "Send SOAP Request", "XML and SOAP", true, true),
    COMMAND("web.workbench.websocket.connect", "Connect WebSocket", "WebSocket", true, true),
    COMMAND("web.workbench.websocket.disconnect", "Disconnect WebSocket", "WebSocket", true, true),
    COMMAND("web.workbench.websocket.send", "Send WebSocket Message", "WebSocket", true, true),
    COMMAND("web.workbench.websocket.clear", "Clear WebSocket Messages", "WebSocket", false, false),
    COMMAND("web.workbench.sse.connect", "Connect Event Stream", "Server-Sent Events", true, true),
    COMMAND("web.workbench.sse.disconnect", "Disconnect Event Stream", "Server-Sent Events", true, true),
    COMMAND("web.workbench.sse.clear", "Clear Event Stream", "Server-Sent Events", false, false),
    COMMAND("web.workbench.server.open", "Open Web Server Diagnostics", "Web Server", false, false),
    COMMAND("web.workbench.server.refresh", "Refresh Web Server Diagnostics", "Web Server", false, false),
    COMMAND("web.workbench.server.start", "Start Web Server", "Web Server", true, true),
    COMMAND("web.workbench.server.stop", "Stop Web Server", "Web Server", true, true),
    COMMAND("web.workbench.cloud.profile.new", "New Cloud Profile", "Cloud", true, false),
    COMMAND("web.workbench.cloud.profile.edit", "Edit Cloud Profile", "Cloud", true, false),
    COMMAND("web.workbench.cloud.objects.refresh", "Refresh Cloud Objects", "Cloud Objects", true, true),
    COMMAND("web.workbench.cloud.objects.upload", "Upload Cloud Object", "Cloud Objects", true, true),
    COMMAND("web.workbench.cloud.objects.download", "Download Cloud Object", "Cloud Objects", true, true),
    COMMAND("web.workbench.cloud.objects.delete", "Delete Cloud Object", "Cloud Objects", true, true),
    COMMAND("web.workbench.cloud.queues.refresh", "Refresh Cloud Queues", "Cloud Queues", true, true),
    COMMAND("web.workbench.cloud.queues.send", "Send Queue Message", "Cloud Queues", true, true),
    COMMAND("web.workbench.cloud.queues.peek", "Peek Queue Messages", "Cloud Queues", true, true),
    COMMAND("web.workbench.cloud.queues.purge", "Purge Cloud Queue", "Cloud Queues", true, true),
    COMMAND("web.workbench.deployment.open", "Open Web Deployment Targets", "Deployment", false, false),
    COMMAND("web.workbench.deployment.deploy", "Deploy Web Application", "Deployment", true, true),
    COMMAND("web.workbench.deployment.rollback", "Rollback Web Deployment", "Deployment", true, true)
};

#undef COMMAND

size_t umi_web_workbench_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiWebWorkbenchCommandDescriptor *umi_web_workbench_command_at(
    size_t index)
{
    return index < umi_web_workbench_command_count() ? &COMMANDS[index] : NULL;
}

const UmiWebWorkbenchCommandDescriptor *umi_web_workbench_command_find(
    const char *command_id)
{
    size_t index;
    if (command_id == NULL) return NULL;
    for (index = 0U; index < umi_web_workbench_command_count(); ++index) {
        if (strcmp(COMMANDS[index].command_id, command_id) == 0) return &COMMANDS[index];
    }
    return NULL;
}
