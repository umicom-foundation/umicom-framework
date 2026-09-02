/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/scaffold/scaffold.h
 *
 * PURPOSE:
 *   Declare the scaffold contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SCAFFOLD_SCAFFOLD_H
#define UMICOM_SCAFFOLD_SCAFFOLD_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named scaffold frontend values accepted by this public contract.
 */
typedef enum UmiScaffoldFrontend {
    UMI_SCAFFOLD_FRONTEND_NONE = 0,
    UMI_SCAFFOLD_FRONTEND_CONSOLE = 1 << 0,
    UMI_SCAFFOLD_FRONTEND_GTK4 = 1 << 1,
    UMI_SCAFFOLD_FRONTEND_WEB = 1 << 2
} UmiScaffoldFrontend;

/**
 * Represent the scaffold request data shared with callers of this public contract.
 */
typedef struct UmiScaffoldRequest {
    const char *workspace_root;
    const char *application_name;
    unsigned frontends;
} UmiScaffoldRequest;

/**
 * Represent the scaffold report data shared with callers of this public contract.
 */
typedef struct UmiScaffoldReport {
    char application_id[128];
    char application_slug[128];
    char application_directory[1024];
    size_t directories_created;
    size_t files_created;
} UmiScaffoldReport;

/**
 * Provide the scaffold copy workspace template operation used by this module and its
 * client applications.
 */
UmiStatus umi_scaffold_copy_workspace_template(
    const char *template_root,
    const char *destination_root
);

/**
 * Provide the scaffold create application operation used by this module and its client
 * applications.
 */
UmiStatus umi_scaffold_create_application(
    const UmiScaffoldRequest *request,
    UmiScaffoldReport *out_report
);

/**
 * Provide the scaffold is workspace operation used by this module and its client
 * applications.
 */
int umi_scaffold_is_workspace(const char *path);

#ifdef __cplusplus
}
#endif

#endif
