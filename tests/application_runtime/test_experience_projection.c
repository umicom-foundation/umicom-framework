/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_experience_projection.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_experience_projection(void){ UmiApplicationPanelProjection p; UmiApplicationLayoutProjection l; UmiApplicationFeatureProjection f; const UmiApplicationExperienceDefinition *e=test_trader_experience(); assert(umi_application_panel_projection_at(e,0U,&p)==UMI_STATUS_OK); assert(umi_application_layout_projection_at(e,0U,&l)==UMI_STATUS_OK); assert(umi_application_feature_projection_at(e,0U,&f)==UMI_STATUS_OK); return 0; }
