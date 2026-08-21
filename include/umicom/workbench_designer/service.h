/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/service.h
 *
 * PURPOSE:
 *   Manage bounded open designer sessions and the active layout across Umicom
 *   Desk and application workbenches.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SERVICE_H
#define UMICOM_WORKBENCH_DESIGNER_SERVICE_H

#include "umicom/workbench_designer/events.h"
#include "umicom/workbench_designer/session.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerService UmiWorkbenchDesignerService;

typedef struct UmiWorkbenchDesignerServiceConfig {
    size_t maximum_sessions;
    bool close_clean_sessions_automatically;
    bool publish_events;
} UmiWorkbenchDesignerServiceConfig;

UmiWorkbenchDesignerServiceConfig umi_workbench_designer_service_config_default(void);
UmiStatus umi_workbench_designer_service_create(const UmiWorkbenchDesignerServiceConfig *config, UmiWorkbenchDesignerService **out_service);
void umi_workbench_designer_service_destroy(UmiWorkbenchDesignerService *service);
UmiStatus umi_workbench_designer_service_open(UmiWorkbenchDesignerService *service, const char *session_id, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerSessionConfig *config, UmiWorkbenchDesignerSession **out_session);
UmiStatus umi_workbench_designer_service_close(UmiWorkbenchDesignerService *service, const char *session_id, bool discard_dirty);
UmiStatus umi_workbench_designer_service_activate(UmiWorkbenchDesignerService *service, const char *session_id);
UmiWorkbenchDesignerSession *umi_workbench_designer_service_find(UmiWorkbenchDesignerService *service, const char *session_id);
const UmiWorkbenchDesignerSession *umi_workbench_designer_service_find_const(const UmiWorkbenchDesignerService *service, const char *session_id);
UmiWorkbenchDesignerSession *umi_workbench_designer_service_active(UmiWorkbenchDesignerService *service);
size_t umi_workbench_designer_service_session_count(const UmiWorkbenchDesignerService *service);
size_t umi_workbench_designer_service_dirty_count(const UmiWorkbenchDesignerService *service);
UmiWorkbenchDesignerEventBus *umi_workbench_designer_service_event_bus(UmiWorkbenchDesignerService *service);
uint64_t umi_workbench_designer_service_revision(const UmiWorkbenchDesignerService *service);

#ifdef __cplusplus
}
#endif

#endif
