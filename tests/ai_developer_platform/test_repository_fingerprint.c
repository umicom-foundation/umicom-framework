/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_repository_fingerprint.c
 *
 * PURPOSE:
 *   Implement the test repository fingerprint behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/repository_fingerprint.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { uint64_t a=umi_ai_dev_repository_fingerprint_text("abc"),b=umi_ai_dev_repository_fingerprint_text("abd"); /* Apply this branch only when its contract condition is satisfied. */ if(a==0U||a==b)return 1; /* Apply this branch only when its contract condition is satisfied. */ if(!umi_ai_dev_repository_fingerprint_same("same","same"))return 2; return 0; }
