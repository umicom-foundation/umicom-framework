/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/workstation_conformance.c
 *
 * PURPOSE:
 *   aggregate workstation parity across layout, interaction, accessibility and rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/workstation_conformance.h"

double umi_fc_workstation_conformance_score(const UmiFcWorkstationConformance *item){if(item==NULL)return 0.0;return umi_fc_clamp_score(item->layout*0.25+item->interaction*0.20+item->accessibility*0.25+item->rendering*0.20+item->persistence*0.10);}
UmiFcOutcome umi_fc_workstation_conformance_outcome(const UmiFcWorkstationConformance *item,double minimum){double score;if(item==NULL||item->blockers>0U)return UMI_FC_FAIL;score=umi_fc_workstation_conformance_score(item);if(score>=minimum)return UMI_FC_PASS;if(score>=minimum*0.85)return UMI_FC_DEGRADED;return UMI_FC_FAIL;}
