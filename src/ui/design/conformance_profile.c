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
UmiStatus umi_design_conformance_record(UmiDesignConformanceProfile *profile,const char *category,int passed){uint32_t *checks=NULL,*failures=NULL;if(profile==NULL||category==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(strcmp(category,"tokens")==0){checks=&profile->token_checks;failures=&profile->token_failures;}else if(strcmp(category,"accessibility")==0){checks=&profile->accessibility_checks;failures=&profile->accessibility_failures;}else if(strcmp(category,"contrast")==0){checks=&profile->contrast_checks;failures=&profile->contrast_failures;}else if(strcmp(category,"responsive")==0){checks=&profile->responsive_checks;failures=&profile->responsive_failures;}else if(strcmp(category,"components")==0){checks=&profile->component_checks;failures=&profile->component_failures;}else return UMI_STATUS_NOT_FOUND;++(*checks);if(!passed)++(*failures);return UMI_STATUS_OK;}
double umi_design_conformance_score(const UmiDesignConformanceProfile *profile){uint64_t checks,failures;if(profile==NULL)return 0.0;checks=(uint64_t)profile->token_checks+profile->accessibility_checks+profile->contrast_checks+profile->responsive_checks+profile->component_checks;failures=(uint64_t)profile->token_failures+profile->accessibility_failures+profile->contrast_failures+profile->responsive_failures+profile->component_failures;if(checks==0U)return 1.0;return (double)(checks-failures)/(double)checks;}
int umi_design_conformance_passes(const UmiDesignConformanceProfile *profile){return profile!=NULL&&profile->token_failures==0U&&profile->accessibility_failures==0U&&profile->contrast_failures==0U&&profile->responsive_failures==0U&&profile->component_failures==0U?1:0;}
