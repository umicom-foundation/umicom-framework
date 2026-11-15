/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/toolchain.h
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
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_TOOLCHAIN_H
#define UMICOM_SDK_TOOLCHAIN_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk toolchain evidence data shared with callers of this public contract.
 */
typedef struct UmiSdkToolchainEvidence { const char *compiler_id; const char *compiler_version; const char *architecture; size_t pointer_size; } UmiSdkToolchainEvidence;
/**
 * Check that sdk toolchain satisfies its contract before another service relies on it.
 */
/* Require non-null evidence, nonempty readable C strings and a nonzero declared
 * pointer size. This structural check does not identify or qualify a toolchain. */
UmiStatus umi_sdk_toolchain_validate(const UmiSdkToolchainEvidence *toolchain);

/* Differences in explicit declarations, not binary compatibility verdicts. */
typedef enum UmiSdkToolchainDifference {
    UMI_SDK_TOOLCHAIN_DIFFERENT_COMPILER = 1U << 0,
    UMI_SDK_TOOLCHAIN_DIFFERENT_VERSION = 1U << 1,
    UMI_SDK_TOOLCHAIN_DIFFERENT_ARCHITECTURE = 1U << 2,
    UMI_SDK_TOOLCHAIN_DIFFERENT_POINTER_SIZE = 1U << 3
} UmiSdkToolchainDifference;

typedef struct UmiSdkToolchainComparison {
    uint32_t differing_fields;
    int declarations_match;
} UmiSdkToolchainComparison;

/* Compare valid declarations synchronously, borrowing readable NUL-terminated
 * input strings only during this call. Strings compare exactly, including
 * case and version/architecture aliases. Pointer sizes are declared byte counts.
 * Invalid input leaves out_comparison unchanged. OK means comparison completed,
 * including when fields differ. No files, compiler, archive or process is read.
 * Equal declarations do not establish ABI, instruction-set or runtime support. */
UmiStatus umi_sdk_toolchain_compare(const UmiSdkToolchainEvidence *producer,
    const UmiSdkToolchainEvidence *consumer, UmiSdkToolchainComparison *out_comparison);

#ifdef __cplusplus
}
#endif
#endif
