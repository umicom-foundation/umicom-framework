/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/theme_equivalence.c
 *
 * PURPOSE:
 *   semantic theme-token equivalence scoring and required-token coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/theme_equivalence.h"

/*
 * Provide the fc theme equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_theme_equivalence_evaluate(const UmiFcThemeEquivalence *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item->required_tokens==0U)return 1.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item->matched_tokens>item->required_tokens)return 0.0;return umi_fc_ratio(item->matched_tokens,item->required_tokens);}
