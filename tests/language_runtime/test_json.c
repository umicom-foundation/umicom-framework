/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_json.c
 *
 * PURPOSE:
 *   Verify a focused language-runtime core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/json.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageRuntimeJsonDocument d;int t;char b[64];assert(umi_language_runtime_json_parse("{\"name\":\"A\\nB\",\"values\":[1,true]}",&d)==UMI_STATUS_OK);t=umi_language_runtime_json_object_get(&d,0,"name");assert(umi_language_runtime_json_string(&d,t,b,sizeof(b))==UMI_STATUS_OK);assert(strcmp(b,"A\nB")==0);return 0;}
