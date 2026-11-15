/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/toolchain.c
 *
 * PURPOSE:
 *   Record compiler and architecture evidence associated with an SDK installation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/toolchain.h"
#include <stddef.h>
#include <string.h>
/* Check that sdk toolchain satisfies its contract before another service relies on it. */
/* Reject empty declarations and keep exact comparison in Framework without claiming binary compatibility. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_sdk_toolchain_validate(const UmiSdkToolchainEvidence *t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||t->compiler_id==NULL||t->compiler_version==NULL||t->architecture==NULL||t->pointer_size==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
#endif
/* Nonempty declarations are the minimum useful evidence. They remain
 * caller-supplied text: accepting them neither detects a compiler nor proves
 * that an SDK archive contains code for the declared architecture. */
UmiStatus umi_sdk_toolchain_validate(const UmiSdkToolchainEvidence *toolchain)
{
    if (toolchain == NULL || toolchain->compiler_id == NULL ||
        toolchain->compiler_version == NULL || toolchain->architecture == NULL ||
        toolchain->compiler_id[0] == '\0' || toolchain->compiler_version[0] == '\0' ||
        toolchain->architecture[0] == '\0' || toolchain->pointer_size == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* Keep this pure comparison in Framework so applications do not turn different
 * aliases, version spellings or pointer widths into an undocumented pass.
 * Exact declarations are compared without guessing binary compatibility. */
UmiStatus umi_sdk_toolchain_compare(const UmiSdkToolchainEvidence *producer,
    const UmiSdkToolchainEvidence *consumer, UmiSdkToolchainComparison *out_comparison)
{
    if (out_comparison == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_sdk_toolchain_validate(producer);
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_toolchain_validate(consumer);
    if (status != UMI_STATUS_OK) return status;

    UmiSdkToolchainComparison comparison = {0};
    if (strcmp(producer->compiler_id, consumer->compiler_id) != 0)
        comparison.differing_fields |= UMI_SDK_TOOLCHAIN_DIFFERENT_COMPILER;
    if (strcmp(producer->compiler_version, consumer->compiler_version) != 0)
        comparison.differing_fields |= UMI_SDK_TOOLCHAIN_DIFFERENT_VERSION;
    if (strcmp(producer->architecture, consumer->architecture) != 0)
        comparison.differing_fields |= UMI_SDK_TOOLCHAIN_DIFFERENT_ARCHITECTURE;
    if (producer->pointer_size != consumer->pointer_size)
        comparison.differing_fields |= UMI_SDK_TOOLCHAIN_DIFFERENT_POINTER_SIZE;
    comparison.declarations_match = comparison.differing_fields == 0U;
    *out_comparison = comparison;
    return UMI_STATUS_OK;
}
