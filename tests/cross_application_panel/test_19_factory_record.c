/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cross_application_panel/test_19_factory_record.c
 *
 * PURPOSE:
 *   Validate cross-application panel factory record contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/cross_application_panel/factory_record.h"
#include "umicom/cross_application_panel/types.h"
int main(void)
{
    UmiPanelIdentity identity={0};
    (void)umi_context_copy_text(identity.panel_id,sizeof(identity.panel_id),"panel.test");
    (void)umi_context_copy_text(identity.application_id,sizeof(identity.application_id),"application.test");
    (void)umi_context_copy_text(identity.instance_id,sizeof(identity.instance_id),"instance.test");
    (void)umi_context_copy_text(identity.component_id,sizeof(identity.component_id),"component.test");
    assert(umi_panel_identity_valid(&identity));
    assert(strcmp(umi_panel_lifecycle_state_text(UMI_PANEL_VISIBLE),"visible")==0);
    assert(strcmp(umi_panel_placement_text(UMI_PANEL_PLACE_FLOATING),"floating")==0);
    return 0;
}
