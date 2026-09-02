/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/frontend_studio.h
 *
 * PURPOSE:
 *   Publish the public frontend studio contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_FRONTEND_STUDIO_H
#define UMICOM_FRONTEND_FRONTEND_STUDIO_H
#include "umicom/frontend/build_profile.h"
#include "umicom/frontend/console_log.h"
#include "umicom/frontend/dev_server.h"
#include "umicom/frontend/document_store.h"
#include "umicom/frontend/dom_inspector.h"
#include "umicom/frontend/hot_reload.h"
#include "umicom/frontend/network_log.h"
#include "umicom/frontend/preview_session.h"
#include "umicom/frontend/quality_audit.h"
/**
 * Represent the frontend studio data shared with callers of this public contract.
 */
typedef struct UmiFrontendStudio { UmiFrontendDeviceRegistry devices; UmiFrontendDocumentStore documents; UmiFrontendDevServer server; UmiFrontendPreviewSession preview; UmiFrontendDomInspector dom; UmiFrontendNetworkLog network; UmiFrontendConsoleLog console; UmiFrontendBuildProfiles builds; UmiFrontendHotReload reload; UmiFrontendQualityBudget budget; uint64_t revision; } UmiFrontendStudio;
/**
 * Represent the frontend studio snapshot data shared with callers of this public contract.
 */
typedef struct UmiFrontendStudioSnapshot { size_t device_count; size_t document_count; size_t dom_node_count; size_t network_entry_count; size_t console_entry_count; size_t build_profile_count; size_t pending_changes; uint64_t revision; int server_running; char preview_url[UMI_FRONTEND_DEV_URL_CAPACITY]; char active_device[UMI_FRONTEND_DEV_ID_CAPACITY]; } UmiFrontendStudioSnapshot;
/**
 * Initialise frontend studio from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_frontend_studio_init(UmiFrontendStudio *studio,const UmiFrontendDevServerConfig *config);
/**
 * Provide the frontend studio snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_studio_snapshot(const UmiFrontendStudio *studio,UmiFrontendStudioSnapshot *out_snapshot);
/**
 * Provide the frontend studio open document operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_studio_open_document(UmiFrontendStudio *studio,const UmiFrontendDocument *document);
/**
 * Provide the frontend studio publish changes operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_studio_publish_changes(UmiFrontendStudio *studio,uint64_t *out_generation);
#endif
