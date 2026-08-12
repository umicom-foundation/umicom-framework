/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_contract_version.c
 *
 * PURPOSE:
 *   Verify that the runtime compatibility contract reports the same Framework
 *   semantic version exposed by the public version header.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/contract.h"

#include <assert.h>

int main(void)
{
    UmiRuntimeContractSnapshot snapshot;
    UmiVersion current;
    UmiVersion minimum;
    UmiVersion future;

    assert(umi_runtime_contract_snapshot(&snapshot) == UMI_STATUS_OK);
    assert(snapshot.api_version == UMI_RUNTIME_CONTRACT_API_VERSION);
    assert(snapshot.framework_version.major == UMICOM_FRAMEWORK_VERSION_MAJOR);
    assert(snapshot.framework_version.minor == UMICOM_FRAMEWORK_VERSION_MINOR);
    assert(snapshot.framework_version.patch == UMICOM_FRAMEWORK_VERSION_PATCH);

    current = snapshot.framework_version;
    minimum = (UmiVersion){0U, 0U, 0U};
    future = current;
    ++future.major;
    assert(umi_runtime_contract_version_at_least(&current, &minimum));
    assert(umi_runtime_contract_version_at_least(&current, &current));
    assert(!umi_runtime_contract_version_at_least(&current, &future));
    assert(!umi_runtime_contract_version_at_least(NULL, &minimum));
    return 0;
}
