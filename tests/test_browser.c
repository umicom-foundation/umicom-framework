/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_browser.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <string.h>
static void sink(const UmiBrowserEvent *e,void *d){int *count=(int*)d;if(e!=NULL)++*count;}
int main(void){UmiBrowser b;int count=0;assert(umi_browser_headless_create(sink,&count,&b)==UMI_STATUS_OK);assert(umi_browser_validate(&b)==UMI_STATUS_OK);assert(b.navigate(b.instance,"https://example.com")==UMI_STATUS_OK);assert(strcmp(b.current_url(b.instance),"https://example.com")==0);assert(count==1);b.destroy(b.instance);return 0;}
