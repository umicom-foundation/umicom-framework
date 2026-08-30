/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/filesystem_semantics.h
 *
 * PURPOSE:
 *   Describe symlink, permission, atomic-replace and file-watch capabilities required by portable Framework storage.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_FILESYSTEM_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_FILESYSTEM_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtFilesystemSemantics { bool symlink; bool posix_permissions; bool atomic_replace; bool recursive_watch; bool sparse_files; bool case_preserving; } UmiCtFilesystemSemantics;
UmiCtFilesystemSemantics umi_ct_filesystem_semantics_default(UmiCtOperatingSystem os);
uint32_t umi_ct_filesystem_semantics_score(const UmiCtFilesystemSemantics *s);

#ifdef __cplusplus
}
#endif

#endif
