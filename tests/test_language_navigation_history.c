/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_language_navigation_history.c
 *
 * PURPOSE:
 *   Implement the test language navigation history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework language navigation history test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/language/navigation_history.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageNavigationHistory*h=NULL;UmiLanguageNavigationLocation a={0},b={0},o;strcpy(a.uri,"file:///a.c");strcpy(b.uri,"file:///b.c");assert(umi_language_navigation_history_create(&h)==UMI_STATUS_OK);assert(umi_language_navigation_history_push(h,&a)==UMI_STATUS_OK);assert(umi_language_navigation_history_push(h,&b)==UMI_STATUS_OK);assert(umi_language_navigation_history_back(h,&o)==UMI_STATUS_OK);assert(strcmp(o.uri,"file:///a.c")==0);assert(umi_language_navigation_history_forward(h,&o)==UMI_STATUS_OK);assert(strcmp(o.uri,"file:///b.c")==0);umi_language_navigation_history_destroy(h);return 0;}
