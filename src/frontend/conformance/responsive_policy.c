/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/responsive_policy.c
 *
 * PURPOSE:
 *   responsive viewport policy selecting compact, standard and wide workstation modes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/responsive_policy.h"

/*
 * Provide the fc responsive policy default operation used by this module and its client
 * applications.
 */
void umi_fc_responsive_policy_default(UmiFcResponsivePolicy *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy!=NULL)*policy=(UmiFcResponsivePolicy){767,1279,1919};}
/*
 * Provide the fc responsive policy classify operation used by this module and its client
 * applications.
 */
UmiFcViewportClass umi_fc_responsive_policy_classify(const UmiFcResponsivePolicy *policy,int32_t width){UmiFcResponsivePolicy d;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL){umi_fc_responsive_policy_default(&d);policy=&d;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(width<=policy->compact_max)return UMI_FC_VIEW_COMPACT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(width<=policy->standard_max)return UMI_FC_VIEW_STANDARD;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(width<=policy->wide_max)return UMI_FC_VIEW_WIDE;return UMI_FC_VIEW_ULTRAWIDE;}
