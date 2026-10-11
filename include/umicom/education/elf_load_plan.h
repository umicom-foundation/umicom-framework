/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/elf_load_plan.h
 * PURPOSE:
 *   Inspect the PT_LOAD programme headers of ELF32/ELF64 files and produce a
 *   bounded, read-only educational load plan. No address is mapped, no file
 *   bytes are executed, and no program/kernel image is installed.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_ELF_LOAD_PLAN_H
#define UMICOM_EDUCATION_ELF_LOAD_PLAN_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_ELF_LOAD_MAX_SEGMENTS 64U
#define UMI_EDUCATION_ELF_LOAD_MAX_PROGRAM_HEADERS 4096U
/* Inspection reads at most this prefix, even for a much larger input file. */
#define UMI_EDUCATION_ELF_LOAD_MAX_HEADER_PREFIX (1024U * 1024U)

#define UMI_EDUCATION_ELF_SEGMENT_EXECUTE 1U
#define UMI_EDUCATION_ELF_SEGMENT_WRITE 2U
#define UMI_EDUCATION_ELF_SEGMENT_READ 4U

typedef struct UmiEducationElfLoadSegment {
    uint64_t fileOffset;
    uint64_t virtualAddress;
    uint64_t physicalAddressHint;
    uint64_t fileBytes;
    uint64_t memoryBytes;
    uint64_t alignment;
    uint32_t flags;
} UmiEducationElfLoadSegment;

typedef struct UmiEducationElfLoadPlan {
    uint64_t inputFileBytes;
    uint64_t entryAddress;
    uint64_t minimumVirtualAddress;
    uint64_t maximumVirtualAddressExclusive;
    uint64_t zeroFillBytes;
    uint64_t fileBackedBytes;
    uint16_t elfType;
    uint16_t machine;
    uint16_t programHeaderCount;
    uint8_t addressBits;
    uint8_t littleEndian;
    uint8_t entryInExecutableSegment;
    uint32_t segmentCount;
    uint32_t executableSegmentCount;
    UmiEducationElfLoadSegment segments[UMI_EDUCATION_ELF_LOAD_MAX_SEGMENTS];
} UmiEducationElfLoadPlan;

/* Inspect an ELF header and its complete programme-header table. 'bytes' may
 * be only a prefix of a larger file: 'actualFileBytes' describes the real
 * entire file for checking PT_LOAD file ranges. Callers must NOT pass an
 * invented larger size when making security decisions about a real file.
 *
 * Supports both byte orders, ELF32/ELF64 and ordinary non-extended header
 * counts. Rejects malformed ranges, overlapping memory spans, invalid PT_LOAD
 * alignment and a nonzero entry outside executable loadable memory.
 * An extended PN_XNUM programme-header count is not implemented.
 * On any error the caller's report is unchanged. */
UmiStatus UmiEducationElfLoadPlanInspectBytes(const void *bytes, size_t length,
    uint64_t actualFileBytes, UmiEducationElfLoadPlan *outPlan);

/* Read-only file version. Examines at most one MiB of the beginning of a
 * regular ELF file and never reads a whole executable into memory. Reports
 * a file-size validation failure if any segment references bytes beyond EOF.
 * The path must name a readable file. No writes or execution are performed. */
UmiStatus UmiEducationElfLoadPlanInspectFile(const char *path,
    UmiEducationElfLoadPlan *outPlan);

#ifdef __cplusplus
}
#endif
#endif
