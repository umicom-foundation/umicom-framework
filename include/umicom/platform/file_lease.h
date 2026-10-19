/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/file_lease.h
 * Purpose: Declare cooperative native ownership of a caller-selected regular file.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_FILE_LEASE_H
#define UMICOM_PLATFORM_FILE_LEASE_H
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFileLease UmiFileLease;
/** Acquire a nonblocking, exclusive native lease. path must be an absolute
 * ordinary file path accepted by UmiOutputFileValidatePath. Its parent must
 * already exist. outLease must address NULL; every refusal preserves it.
 *
 * A missing file is created; existing bytes are never read, written, truncated
 * or deleted. BUSY means another incompatible handle/lease currently exists.
 * File existence alone does not mean busy, so a crashed owner leaves no stale
 * ownership marker to remove. Final symlinks/reparse points, directories and
 * special files are refused. Ancestors must be trusted and stable.
 *
 * Keep the returned handle alive throughout the protected work. All writers
 * must use the same persistent guard file; do not unlink or replace it, even
 * after release. POSIX locking is advisory. This is not a filesystem snapshot,
 * security boundary, distributed lock, or protection against arbitrary tools.
 * Supported hosts are Windows, Linux and macOS; others return UNAVAILABLE.
 * Handles are not inherited by Windows children or POSIX exec. A raw POSIX
 * fork shares the descriptor until exec/close and must not outlive the owner.
 * Serialize operations on each lease; distinct acquisitions are independent.
 */
UmiStatus UmiFileLeaseTryAcquire(const char *path, UmiFileLease **outLease);
/** Close once, then consume the owned pointer. NULL *lease is a successful
 * no-op. A native close failure is returned, not retried against a potentially
 * reused descriptor. The guard file remains and can be acquired again. */
UmiStatus UmiFileLeaseRelease(UmiFileLease **lease);
#ifdef __cplusplus
}
#endif
#endif
