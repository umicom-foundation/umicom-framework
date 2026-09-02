/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/service.h
 *
 * PURPOSE:
 *   Manage bounded open designer sessions and the active layout across Umicom
 *   Desk and application workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SERVICE_H
#define UMICOM_WORKBENCH_DESIGNER_SERVICE_H

#include "umicom/workbench_designer/events.h"
#include "umicom/workbench_designer/session.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer service data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerService UmiWorkbenchDesignerService;

/**
 * Represent the workbench designer service config data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerServiceConfig {
    size_t maximum_sessions;
    bool close_clean_sessions_automatically;
    bool publish_events;
} UmiWorkbenchDesignerServiceConfig;

/**
 * Provide the workbench designer service config default operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerServiceConfig umi_workbench_designer_service_config_default(void);
/**
 * Initialise workbench designer service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_designer_service_create(const UmiWorkbenchDesignerServiceConfig *config, UmiWorkbenchDesignerService **out_service);
/**
 * Release or reset state held by workbench designer service so the same storage can be
 * reused safely.
 */
void umi_workbench_designer_service_destroy(UmiWorkbenchDesignerService *service);
/**
 * Provide the workbench designer service open operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_service_open(UmiWorkbenchDesignerService *service, const char *session_id, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerSessionConfig *config, UmiWorkbenchDesignerSession **out_session);
/**
 * Provide the workbench designer service close operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_service_close(UmiWorkbenchDesignerService *service, const char *session_id, bool discard_dirty);
/**
 * Provide the workbench designer service activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_service_activate(UmiWorkbenchDesignerService *service, const char *session_id);
/**
 * Find workbench designer service while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiWorkbenchDesignerSession *umi_workbench_designer_service_find(UmiWorkbenchDesignerService *service, const char *session_id);
/**
 * Provide the workbench designer service find const operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerSession *umi_workbench_designer_service_find_const(const UmiWorkbenchDesignerService *service, const char *session_id);
/**
 * Provide the workbench designer service active operation used by this module and its
 * client applications.
 */
UmiWorkbenchDesignerSession *umi_workbench_designer_service_active(UmiWorkbenchDesignerService *service);
/**
 * Return the number of records represented by workbench designer service session without
 * changing their state.
 */
size_t umi_workbench_designer_service_session_count(const UmiWorkbenchDesignerService *service);
/**
 * Return the number of records represented by workbench designer service dirty without
 * changing their state.
 */
size_t umi_workbench_designer_service_dirty_count(const UmiWorkbenchDesignerService *service);
/**
 * Provide the workbench designer service event bus operation used by this module and its
 * client applications.
 */
UmiWorkbenchDesignerEventBus *umi_workbench_designer_service_event_bus(UmiWorkbenchDesignerService *service);
/**
 * Provide the workbench designer service revision operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_designer_service_revision(const UmiWorkbenchDesignerService *service);

#ifdef __cplusplus
}
#endif

#endif
