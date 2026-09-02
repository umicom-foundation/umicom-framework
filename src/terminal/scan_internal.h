/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/scan_internal.h
 *
 * PURPOSE:
 *   Declare the scan internal contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Private terminal transcript scanner ABI. */
#ifndef UMICOM_TERMINAL_SCAN_INTERNAL_H
#define UMICOM_TERMINAL_SCAN_INTERNAL_H
#include <stddef.h>
#include <stdint.h>
/**
 * Provide the terminal scan count byte portable operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_scan_count_byte_portable(const unsigned char *bytes,
                                               size_t byte_count,
                                               unsigned char value);
/**
 * Provide the terminal scan count byte x86 64 operation used by this module and its client
 * applications.
 */
uint64_t umi_terminal_scan_count_byte_x86_64(const unsigned char *bytes,
                                             size_t byte_count,
                                             unsigned char value);
/**
 * Provide the terminal scan count byte aarch64 operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_scan_count_byte_aarch64(const unsigned char *bytes,
                                              size_t byte_count,
                                              unsigned char value);
/**
 * Provide the terminal scan count byte riscv64 operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_scan_count_byte_riscv64(const unsigned char *bytes,
                                              size_t byte_count,
                                              unsigned char value);
#endif
