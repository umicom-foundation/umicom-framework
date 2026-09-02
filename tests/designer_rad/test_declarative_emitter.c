/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_declarative_emitter.c
 *
 * PURPOSE:
 *   Validate build inspectable declarative application text into a bounded buffer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/declarative_emitter.h"
#include <string.h>
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDeclarativeEmitter e;CHECK(umi_rad_declarative_emitter_init(&e)==UMI_STATUS_OK);CHECK(umi_rad_declarative_emitter_append(&e,"page { ")==UMI_STATUS_OK);CHECK(umi_rad_declarative_emitter_append(&e,"}")==UMI_STATUS_OK);CHECK(strcmp(umi_rad_declarative_emitter_text(&e),"page { }")==0);return 0;}
