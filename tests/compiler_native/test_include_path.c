/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_include_path.c
 *
 * PURPOSE:
 *   Regression coverage for manage portable user/system include roots and construct normalized include candidates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/include_path.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiNativeIncludePathSet p; umi_nc_include_paths_init(&p); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_include_paths_add(&p,"include",false)!=UMI_STATUS_OK) return 1; char b[64]; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_include_candidate(&p.items[0],"umicom/base/status.h",b,sizeof(b))!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(b,"include/umicom/base/status.h")!=0) return 3; return 0; }
