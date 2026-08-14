/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_FRONTEND_DOM_INSPECTOR_H
#define UMICOM_FRONTEND_DOM_INSPECTOR_H
#include "umicom/frontend/dev_types.h"
typedef struct UmiFrontendDomNode { char id[UMI_FRONTEND_DEV_ID_CAPACITY]; char parent_id[UMI_FRONTEND_DEV_ID_CAPACITY]; char tag[64]; char selector[UMI_FRONTEND_DEV_NAME_CAPACITY]; char text[UMI_FRONTEND_DEV_TEXT_CAPACITY]; int32_t x; int32_t y; uint32_t width; uint32_t height; int visible; } UmiFrontendDomNode;
typedef struct UmiFrontendDomInspector { UmiFrontendDomNode nodes[UMI_FRONTEND_DEV_MAX_DOM_NODES]; size_t count; char selected_id[UMI_FRONTEND_DEV_ID_CAPACITY]; uint64_t revision; } UmiFrontendDomInspector;
UmiStatus umi_frontend_dom_inspector_init(UmiFrontendDomInspector *inspector);
UmiStatus umi_frontend_dom_inspector_upsert(UmiFrontendDomInspector *inspector,const UmiFrontendDomNode *node);
UmiStatus umi_frontend_dom_inspector_select(UmiFrontendDomInspector *inspector,const char *id);
UmiStatus umi_frontend_dom_inspector_selected(const UmiFrontendDomInspector *inspector,UmiFrontendDomNode *out_node);
#endif
