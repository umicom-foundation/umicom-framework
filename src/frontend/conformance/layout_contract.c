/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/layout_contract.c
 *
 * PURPOSE:
 *   toolkit-neutral layout structure expectations used by frontend equivalence checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_contract.h"

/*
 * Provide the fc layout contract satisfied operation used by this module and its client
 * applications.
 */
bool umi_fc_layout_contract_satisfied(const UmiFcLayoutContract *required,const UmiFcLayoutContract *actual){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(required==NULL||actual==NULL)return false;return actual->panel_count>=required->panel_count&&actual->split_count>=required->split_count&&actual->tab_count>=required->tab_count&&(actual->required_features&required->required_features)==required->required_features;}
/*
 * Provide the fc layout contract score operation used by this module and its client
 * applications.
 */
double umi_fc_layout_contract_score(const UmiFcLayoutContract *required,const UmiFcLayoutContract *actual){double sum=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(required==NULL||actual==NULL)return 0.0;sum+=required->panel_count==0U?1.0:umi_fc_clamp_score((double)actual->panel_count/(double)required->panel_count);sum+=required->split_count==0U?1.0:umi_fc_clamp_score((double)actual->split_count/(double)required->split_count);sum+=required->tab_count==0U?1.0:umi_fc_clamp_score((double)actual->tab_count/(double)required->tab_count);sum+=((actual->required_features&required->required_features)==required->required_features)?1.0:0.0;return sum/4.0;}
