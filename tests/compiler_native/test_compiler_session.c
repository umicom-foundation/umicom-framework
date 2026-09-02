/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_compiler_session.c
 *
 * PURPOSE:
 *   Regression coverage for coordinate one native compilation session across target, provider, pipeline and deterministic fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/compiler_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeProviderAvailability p={true,true,true};UmiNativeCompilerSession s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_nc_compiler_session_begin(&s,"build-1","riscv64-unknown-umicom",UMI_NC_OPT_O2,&p,true,1U,2U,3U)!=UMI_STATUS_OK)return 1;/* Visit each bounded item once so every record receives the same rule. */ for(int st=(int)UMI_NC_STAGE_SOURCE;st<=(int)UMI_NC_STAGE_LINK;st++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_nc_compiler_session_stage(&s,(UmiNativeCompileStage)st,true)!=UMI_STATUS_OK)return 2;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_nc_compiler_session_finish(&s)!=UMI_STATUS_OK||!s.completed||s.compile_fingerprint==0U)return 3;return 0;}
