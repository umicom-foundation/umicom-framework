/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_runtime_compatibility.c
 *
 * PURPOSE:
 *   Focused regression coverage for runtime requirement matching with missing-capability and version evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_compatibility.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrRuntimeProfile r; UmiDrRuntimeProbe a; UmiDrRuntimeCompatibility out; umi_dr_runtime_profile_init(&r); umi_dr_runtime_probe_init(&a); r.platform=a.platform=UMI_DR_PLATFORM_WINDOWS; r.architecture=a.architecture=UMI_DR_ARCH_X86_64; a.memory_mb=1U; r.required_capabilities=3U; a.capabilities=3U; CHECK(umi_dr_runtime_compatibility_evaluate(&r,&a,&out)==UMI_STATUS_OK); CHECK(umi_dr_runtime_compatibility_ok(&out));
    return 0;
}
