/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_incremental_compile.c
 *
 * PURPOSE:
 *   Regression coverage for decide translation-unit rebuilds from source, options, dependency and artifact fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/incremental_compile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeIncrementalUnit a,b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_incremental_unit_init(&a,"a.c",1U,2U,3U)!=UMI_STATUS_OK||umi_nc_incremental_unit_init(&b,"a.c",1U,2U,3U)!=UMI_STATUS_OK)return 1;a.artifact_present=true;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_incremental_needs_rebuild(&a,&b))return 2;b.source_hash=9U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_nc_incremental_needs_rebuild(&a,&b))return 3;return 0;}
