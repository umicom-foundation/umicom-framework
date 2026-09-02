/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_frontend_dom_inspector.c
 *
 * PURPOSE:
 *   Implement the test frontend dom inspector behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/frontend/frontend_studio.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
