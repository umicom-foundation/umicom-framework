/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/elf_symbols.h
 * PURPOSE:
 *   Inspect ELF symbol and relocation tables without executing a file.
 *   This is a bounded, read-only educational view of the existing ELF format,
 *   not a linker, loader, disassembler, or replacement for the native kernel.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_ELF_SYMBOLS_H
#define UMICOM_EDUCATION_ELF_SYMBOLS_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The file reader caps one inspection at 32 MiB and rejects expensive table
 * declarations instead of returning incomplete, successful summaries. */
#define UMI_EDUCATION_ELF_FILE_LIMIT (32U * 1024U * 1024U)
#define UMI_EDUCATION_ELF_MAX_RECORDS 262144U
#define UMI_EDUCATION_ELF_DISPLAY_SYMBOLS 16U
#define UMI_EDUCATION_ELF_SYMBOL_NAME 96U

/** A bounded copy of a symbol name. Section zero means an undefined symbol;
 * reserved section indices retain their exact numeric values. */
typedef struct UmiEducationElfSymbol {
    char name[UMI_EDUCATION_ELF_SYMBOL_NAME];
    uint64_t value;
    uint64_t size;
    uint16_t sectionIndex;
    uint8_t binding;
    uint8_t kind;
    int nameTruncated;
} UmiEducationElfSymbol;

/** Counts include non-empty entries from both SYMTAB and DYNSYM. The mandatory
 * all-zero first entry in each table is excluded. Relocation counts cover REL
 * and RELA, but relocation targets are not resolved or executed. */
typedef struct UmiEducationElfReport {
    uint64_t fileBytes;
    uint64_t symbolCount;
    uint64_t definedCount;
    uint64_t undefinedCount;
    uint64_t localCount;
    uint64_t globalCount;
    uint64_t weakCount;
    uint64_t otherBindingCount;
    uint64_t functionCount;
    uint64_t objectCount;
    uint64_t relocationCount;
    uint64_t relCount;
    uint64_t relaCount;
    uint32_t sectionCount;
    uint32_t staticSymbolTables;
    uint32_t dynamicSymbolTables;
    uint32_t relocationSections;
    uint16_t machine;
    uint16_t elfType;
    uint8_t addressBits;
    uint8_t littleEndian;
    uint32_t displayedCount;
    UmiEducationElfSymbol displayed[UMI_EDUCATION_ELF_DISPLAY_SYMBOLS];
} UmiEducationElfReport;

/** Pure bytes interface suitable for fuzzing. Accept ELF32/64, either byte
 * order, ordinary and extended section counts. Every offset is bounds-checked;
 * malformed tables, unsafe names and excessive record counts are refused.
 * Non-ELF inputs return NOT_IMPLEMENTED. On any failure *out is unchanged. */
UmiStatus UmiEducationElfInspectBytes(const void *bytes, size_t length,
                                      UmiEducationElfReport *out);

/** Read one regular file in bounded memory, then use the same parser. Does not
 * load, execute, modify, extract, link or follow section contents as code.
 * The OS determines path encoding for the standard C file interface.
 * On any failure *out is unchanged. */
UmiStatus UmiEducationElfInspectFile(const char *path, UmiEducationElfReport *out);

#ifdef __cplusplus
}
#endif
#endif
