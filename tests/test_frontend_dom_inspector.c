/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/frontend/frontend_studio.h"
int main(void){UmiFrontendDomInspector i;UmiFrontendDomNode n={0},out;(void)strcpy(n.id,"hero");(void)strcpy(n.tag,"main");(void)strcpy(n.selector,"main.hero");n.visible=1;n.width=800U;assert(umi_frontend_dom_inspector_init(&i)==UMI_STATUS_OK);assert(umi_frontend_dom_inspector_upsert(&i,&n)==UMI_STATUS_OK);assert(umi_frontend_dom_inspector_select(&i,"hero")==UMI_STATUS_OK);assert(umi_frontend_dom_inspector_selected(&i,&out)==UMI_STATUS_OK&&out.width==800U);return 0;}
