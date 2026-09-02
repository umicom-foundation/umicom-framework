/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/conformance.c
 *
 * PURPOSE:
 *   Record conformance checks for an SDK consumer build.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/conformance.h"
#include <stddef.h>
/*
 * Provide the sdk conformance record operation used by this module and its client
 * applications.
 */
void umi_sdk_conformance_record(UmiSdkConformance *s,int passed){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;++s->checks_run;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(passed)++s->checks_passed;/* Use this fallback path when the earlier condition does not apply. */ else ++s->checks_failed;}
/*
 * Provide the sdk conformance passed operation used by this module and its client
 * applications.
 */
int umi_sdk_conformance_passed(const UmiSdkConformance *s){return s!=NULL&&s->checks_run>0U&&s->checks_failed==0U;}
