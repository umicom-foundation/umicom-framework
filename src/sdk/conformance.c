/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/conformance.c
 *
 * PURPOSE:
 *   Record conformance checks for an SDK consumer build.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/conformance.h"
#include <stddef.h>
void umi_sdk_conformance_record(UmiSdkConformance *s,int passed){if(s==NULL)return;++s->checks_run;if(passed)++s->checks_passed;else ++s->checks_failed;}
int umi_sdk_conformance_passed(const UmiSdkConformance *s){return s!=NULL&&s->checks_run>0U&&s->checks_failed==0U;}
