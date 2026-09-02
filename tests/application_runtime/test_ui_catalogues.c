/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_ui_catalogues.c
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
/*
 * Exercise test ui catalogues and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ui_catalogues(void){ UmiApplicationPanelProjection p; UmiApplicationLayoutProjection l; UmiApplicationFeatureProjection f; const UmiApplicationExperienceDefinition *e=test_trader_experience(); assert(umi_application_ui_panel_row(e,0U,&p)==UMI_STATUS_OK); assert(umi_application_ui_layout_row(e,0U,&l)==UMI_STATUS_OK); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_application_ui_feature_backlog_count(e)>0U) assert(umi_application_ui_feature_backlog_row(e,0U,&f)==UMI_STATUS_OK); return 0; }
