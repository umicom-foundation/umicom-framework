/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_types.c
 *
 * PURPOSE:
 *   Regression coverage for define stable constants, enums and utility helpers for the native umicom compiler pipeline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/types.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ char b[8]; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_copy_text(b,sizeof(b),"umicc")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(b,"umicc")!=0) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_hash_text("abc")==0U) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(umi_nc_stage_name(UMI_NC_STAGE_IR),"ir")!=0) return 4; return 0; }
