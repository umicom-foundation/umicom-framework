/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/frontend/frontend_studio.h"

int main(void)
{
    UmiFrontendDomInspector *inspector;
    UmiFrontendDomNode node = {0};
    UmiFrontendDomNode result;

    inspector = (UmiFrontendDomInspector *)malloc(sizeof(*inspector));
    assert(inspector != NULL);
    (void)strcpy(node.id, "hero");
    (void)strcpy(node.tag, "main");
    (void)strcpy(node.selector, "main.hero");
    node.visible = 1;
    node.width = 800U;
    assert(umi_frontend_dom_inspector_init(inspector) == UMI_STATUS_OK);
    assert(umi_frontend_dom_inspector_upsert(inspector, &node) == UMI_STATUS_OK);
    assert(umi_frontend_dom_inspector_select(inspector, "hero") == UMI_STATUS_OK);
    assert(umi_frontend_dom_inspector_selected(inspector, &result) == UMI_STATUS_OK);
    assert(result.width == 800U);
    free(inspector);
    return 0;
}
