/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/workstation_conformance.c
 *
 * PURPOSE:
 *   aggregate workstation parity across layout, interaction, accessibility and rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/workstation_conformance.h"

/*
 * Provide the fc workstation conformance score operation used by this module and its
 * client applications.
 */
double umi_fc_workstation_conformance_score(const UmiFcWorkstationConformance *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;return umi_fc_clamp_score(item->layout*0.25+item->interaction*0.20+item->accessibility*0.25+item->rendering*0.20+item->persistence*0.10);}
/*
 * Provide the fc workstation conformance outcome operation used by this module and its
 * client applications.
 */
UmiFcOutcome umi_fc_workstation_conformance_outcome(const UmiFcWorkstationConformance *item,double minimum){double score;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||item->blockers>0U)return UMI_FC_FAIL;score=umi_fc_workstation_conformance_score(item);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(score>=minimum)return UMI_FC_PASS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(score>=minimum*0.85)return UMI_FC_DEGRADED;return UMI_FC_FAIL;}
