/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_runtime_selector.c
 *
 * PURPOSE:
 *   Focused regression coverage for best-compatible runtime profile selection using explicit requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_selector.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrRuntimeProfile p[2]; UmiDrRuntimeProbe h; size_t idx=9U; umi_dr_runtime_profile_init(&p[0]); umi_dr_runtime_profile_init(&p[1]); umi_dr_runtime_probe_init(&h); p[0].platform=p[1].platform=h.platform=UMI_DR_PLATFORM_LINUX; p[0].architecture=p[1].architecture=h.architecture=UMI_DR_ARCH_X86_64; p[1].score_bias=5U; h.memory_mb=1U; CHECK(umi_dr_runtime_selector_choose(p,2U,&h,&idx)==UMI_STATUS_OK); CHECK(idx==1U);
    return 0;
}
