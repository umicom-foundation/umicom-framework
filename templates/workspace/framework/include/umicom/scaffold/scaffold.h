#ifndef UMICOM_SCAFFOLD_SCAFFOLD_H
#define UMICOM_SCAFFOLD_SCAFFOLD_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiScaffoldFrontend {
    UMI_SCAFFOLD_FRONTEND_NONE = 0,
    UMI_SCAFFOLD_FRONTEND_CONSOLE = 1 << 0,
    UMI_SCAFFOLD_FRONTEND_GTK4 = 1 << 1,
    UMI_SCAFFOLD_FRONTEND_WEB = 1 << 2
} UmiScaffoldFrontend;

typedef struct UmiScaffoldRequest {
    const char *workspace_root;
    const char *application_name;
    unsigned frontends;
} UmiScaffoldRequest;

typedef struct UmiScaffoldReport {
    char application_id[128];
    char application_slug[128];
    char application_directory[1024];
    size_t directories_created;
    size_t files_created;
} UmiScaffoldReport;

UmiStatus umi_scaffold_copy_workspace_template(
    const char *template_root,
    const char *destination_root
);

UmiStatus umi_scaffold_create_application(
    const UmiScaffoldRequest *request,
    UmiScaffoldReport *out_report
);

int umi_scaffold_is_workspace(const char *path);

#ifdef __cplusplus
}
#endif

#endif
