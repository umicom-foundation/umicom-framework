/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_framing.c
 *
 * PURPOSE:
 *   Verify a focused language-runtime core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/framing.h"
int main(void){UmiLanguageRuntimeFramer f;char fr[128],out[64];size_t n=0,m=0;assert(umi_language_runtime_frame_encode("{\"id\":1}",fr,sizeof(fr),&n)==UMI_STATUS_OK);umi_language_runtime_framer_init(&f);assert(umi_language_runtime_framer_feed(&f,fr,5)==UMI_STATUS_OK);assert(!umi_language_runtime_framer_has_message(&f));assert(umi_language_runtime_framer_feed(&f,fr+5,n-5)==UMI_STATUS_OK);assert(umi_language_runtime_framer_pop(&f,out,sizeof(out),&m)==UMI_STATUS_OK);assert(strcmp(out,"{\"id\":1}")==0);return 0;}
