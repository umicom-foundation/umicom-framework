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

void umi_fc_responsive_policy_default(UmiFcResponsivePolicy *policy){if(policy!=NULL)*policy=(UmiFcResponsivePolicy){767,1279,1919};}
UmiFcViewportClass umi_fc_responsive_policy_classify(const UmiFcResponsivePolicy *policy,int32_t width){UmiFcResponsivePolicy d;if(policy==NULL){umi_fc_responsive_policy_default(&d);policy=&d;}if(width<=policy->compact_max)return UMI_FC_VIEW_COMPACT;if(width<=policy->standard_max)return UMI_FC_VIEW_STANDARD;if(width<=policy->wide_max)return UMI_FC_VIEW_WIDE;return UMI_FC_VIEW_ULTRAWIDE;}
