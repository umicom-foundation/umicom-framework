/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/dom_inspector.h
 *
 * PURPOSE:
 *   Publish the public dom inspector contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_DOM_INSPECTOR_H
#define UMICOM_FRONTEND_DOM_INSPECTOR_H
#include "umicom/frontend/dev_types.h"
/**
 * Represent the frontend dom node data shared with callers of this public contract.
 */
typedef struct UmiFrontendDomNode { char id[UMI_FRONTEND_DEV_ID_CAPACITY]; char parent_id[UMI_FRONTEND_DEV_ID_CAPACITY]; char tag[64]; char selector[UMI_FRONTEND_DEV_NAME_CAPACITY]; char text[UMI_FRONTEND_DEV_TEXT_CAPACITY]; int32_t x; int32_t y; uint32_t width; uint32_t height; int visible; } UmiFrontendDomNode;
/**
 * Represent the frontend dom inspector data shared with callers of this public contract.
 */
typedef struct UmiFrontendDomInspector { UmiFrontendDomNode nodes[UMI_FRONTEND_DEV_MAX_DOM_NODES]; size_t count; char selected_id[UMI_FRONTEND_DEV_ID_CAPACITY]; uint64_t revision; } UmiFrontendDomInspector;
/**
 * Initialise frontend dom inspector from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_dom_inspector_init(UmiFrontendDomInspector *inspector);
/**
 * Provide the frontend dom inspector upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_dom_inspector_upsert(UmiFrontendDomInspector *inspector,const UmiFrontendDomNode *node);
/**
 * Provide the frontend dom inspector select operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_dom_inspector_select(UmiFrontendDomInspector *inspector,const char *id);
/**
 * Find frontend dom inspector while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_dom_inspector_selected(const UmiFrontendDomInspector *inspector,UmiFrontendDomNode *out_node);
#endif
