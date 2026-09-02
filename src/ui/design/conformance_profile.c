/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/conformance_profile.c
 *
 * PURPOSE:
 *   Aggregate design-token, accessibility, contrast, responsive and semantic-component conformance evidence.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/conformance_profile.h"

#include <string.h>
/*
 * Provide the design conformance record operation used by this module and its client
 * applications.
 */
UmiStatus umi_design_conformance_record(UmiDesignConformanceProfile *profile,const char *category,int passed){uint32_t *checks=NULL,*failures=NULL;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile==NULL||category==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(category,"tokens")==0){checks=&profile->token_checks;failures=&profile->token_failures;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(category,"accessibility")==0){checks=&profile->accessibility_checks;failures=&profile->accessibility_failures;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(category,"contrast")==0){checks=&profile->contrast_checks;failures=&profile->contrast_failures;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(category,"responsive")==0){checks=&profile->responsive_checks;failures=&profile->responsive_failures;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(category,"components")==0){checks=&profile->component_checks;failures=&profile->component_failures;}/* Use this fallback path when the earlier condition does not apply. */ else return UMI_STATUS_NOT_FOUND;++(*checks);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!passed)++(*failures);return UMI_STATUS_OK;}
/*
 * Provide the design conformance score operation used by this module and its client
 * applications.
 */
double umi_design_conformance_score(const UmiDesignConformanceProfile *profile){uint64_t checks,failures;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile==NULL)return 0.0;checks=(uint64_t)profile->token_checks+profile->accessibility_checks+profile->contrast_checks+profile->responsive_checks+profile->component_checks;failures=(uint64_t)profile->token_failures+profile->accessibility_failures+profile->contrast_failures+profile->responsive_failures+profile->component_failures;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(checks==0U)return 1.0;return (double)(checks-failures)/(double)checks;}
/*
 * Provide the design conformance passes operation used by this module and its client
 * applications.
 */
int umi_design_conformance_passes(const UmiDesignConformanceProfile *profile){return profile!=NULL&&profile->token_failures==0U&&profile->accessibility_failures==0U&&profile->contrast_failures==0U&&profile->responsive_failures==0U&&profile->component_failures==0U?1:0;}
