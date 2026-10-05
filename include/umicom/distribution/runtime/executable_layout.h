/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/executable_layout.h
 *
 * PURPOSE:
 *   executable placement and launch-entry validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_EXECUTABLE_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_EXECUTABLE_LAYOUT_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr executable layout data shared with callers of this public contract.
 */
typedef struct UmiDrExecutableLayout { char id[UMI_DR_ID_CAPACITY]; char entrypoint[UMI_DR_PATH_CAPACITY]; char bin_dir[UMI_DR_PATH_CAPACITY]; bool console; bool gui; } UmiDrExecutableLayout;
/**
 * Initialise dr executable layout from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_executable_layout_init(UmiDrExecutableLayout *value);
/**
 * Check that dr executable layout satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_executable_layout_valid(const UmiDrExecutableLayout *value);
/**
 * Provide the dr executable layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_executable_layout_fingerprint(const UmiDrExecutableLayout *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_executable_layout_archive_encode(const UmiDrExecutableLayout *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_executable_layout_archive_decode(const void *bytes, size_t byte_count,
    UmiDrExecutableLayout *value);

#ifdef __cplusplus
}
#endif
#endif
