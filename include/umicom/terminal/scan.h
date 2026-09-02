/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/scan.h
 *
 * PURPOSE:
 *   Provide one portable C ABI for terminal transcript byte scanning, with an
 *   optional audited assembly implementation selected by CMake.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_SCAN_H
#define UMICOM_TERMINAL_SCAN_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named terminal scan backend values accepted by this public contract.
 */
typedef enum UmiTerminalScanBackend {
    UMI_TERMINAL_SCAN_PORTABLE_C = 0,
    UMI_TERMINAL_SCAN_X86_64_ASSEMBLY = 1,
    UMI_TERMINAL_SCAN_AARCH64_ASSEMBLY = 2,
    UMI_TERMINAL_SCAN_RISCV64_ASSEMBLY = 3
} UmiTerminalScanBackend;

/**
 * Provide the terminal scan count byte operation used by this module and its client
 * applications.
 */
uint64_t umi_terminal_scan_count_byte(const unsigned char *bytes,
                                      size_t byte_count,
                                      unsigned char value);
/**
 * Return the number of records represented by terminal scan line without changing their
 * state.
 */
uint64_t umi_terminal_scan_line_count(const char *text, size_t byte_count);
/**
 * Provide the terminal scan backend operation used by this module and its client
 * applications.
 */
UmiTerminalScanBackend umi_terminal_scan_backend(void);
/**
 * Provide the terminal scan backend name operation used by this module and its client
 * applications.
 */
const char *umi_terminal_scan_backend_name(UmiTerminalScanBackend backend);

#ifdef __cplusplus
}
#endif
#endif
