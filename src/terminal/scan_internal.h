/* Private terminal transcript scanner ABI. */
#ifndef UMICOM_TERMINAL_SCAN_INTERNAL_H
#define UMICOM_TERMINAL_SCAN_INTERNAL_H
#include <stddef.h>
#include <stdint.h>
uint64_t umi_terminal_scan_count_byte_portable(const unsigned char *bytes,
                                               size_t byte_count,
                                               unsigned char value);
uint64_t umi_terminal_scan_count_byte_x86_64(const unsigned char *bytes,
                                             size_t byte_count,
                                             unsigned char value);
uint64_t umi_terminal_scan_count_byte_aarch64(const unsigned char *bytes,
                                              size_t byte_count,
                                              unsigned char value);
uint64_t umi_terminal_scan_count_byte_riscv64(const unsigned char *bytes,
                                              size_t byte_count,
                                              unsigned char value);
#endif
