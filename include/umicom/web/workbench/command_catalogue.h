/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/command_catalogue.h
 *
 * PURPOSE:
 *   Publish stable commands for REST, OpenAPI, SOAP, streaming, server, cloud
 *   and deployment tooling. Applications only choose placement and shortcuts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_COMMAND_CATALOGUE_H
#define UMICOM_WEB_WORKBENCH_COMMAND_CATALOGUE_H

#include "umicom/web/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebWorkbenchCommandDescriptor {
    uint32_t structure_size;
    uint32_t api_version;
    const char *command_id;
    const char *label;
    const char *category;
    bool requires_workspace_trust;
    bool mutates_external_state;
} UmiWebWorkbenchCommandDescriptor;

size_t umi_web_workbench_command_count(void);
const UmiWebWorkbenchCommandDescriptor *umi_web_workbench_command_at(
    size_t index);
const UmiWebWorkbenchCommandDescriptor *umi_web_workbench_command_find(
    const char *command_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_COMMAND_CATALOGUE_H */
