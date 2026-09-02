/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/operation_types.h
 *
 * PURPOSE:
 *   Define operation-scoped native tool discovery vocabulary and stable IDs.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_TOOLCHAIN_OPERATION_TYPES_H
#define INCLUDE_UMICOM_TOOLCHAIN_OPERATION_TYPES_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_OPERATION_ID_CAPACITY 64U
#define UMI_TOOLCHAIN_OPERATION_TEXT_CAPACITY 256U

/**
 * List the named toolchain operation kind values accepted by this public contract.
 */
typedef enum UmiToolchainOperationKind {
    UMI_TOOLCHAIN_OPERATION_UNKNOWN = 0,
    UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ = 1,
    UMI_TOOLCHAIN_OPERATION_REPOSITORY_WRITE = 2,
    UMI_TOOLCHAIN_OPERATION_REPOSITORY_REMOTE = 3,
    UMI_TOOLCHAIN_OPERATION_SCAFFOLD_LOCAL = 4,
    UMI_TOOLCHAIN_OPERATION_SCAFFOLD_REMOTE = 5,
    UMI_TOOLCHAIN_OPERATION_BUILD_CONFIGURE = 6,
    UMI_TOOLCHAIN_OPERATION_BUILD_COMPILE = 7,
    UMI_TOOLCHAIN_OPERATION_BUILD_TEST = 8,
    UMI_TOOLCHAIN_OPERATION_RUN = 9,
    UMI_TOOLCHAIN_OPERATION_DEBUG = 10,
    UMI_TOOLCHAIN_OPERATION_LANGUAGE = 11,
    UMI_TOOLCHAIN_OPERATION_FORMAT = 12,
    UMI_TOOLCHAIN_OPERATION_PACKAGE = 13
} UmiToolchainOperationKind;

/**
 * Provide the toolchain operation kind text operation used by this module and its client
 * applications.
 */
const char *umi_toolchain_operation_kind_text(UmiToolchainOperationKind kind);
/**
 * Provide the toolchain operation is repository operation used by this module and its
 * client applications.
 */
int umi_toolchain_operation_is_repository(UmiToolchainOperationKind kind);

#ifdef __cplusplus
}
#endif
#endif
