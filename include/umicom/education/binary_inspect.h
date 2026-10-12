/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/binary_inspect.h
 *
 * PURPOSE:
 *   Describe native object and executable headers without loading or running
 *   their code. Used by Bits2Banking lessons about C, Assembly and the linker.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_BINARY_INSPECT_H
#define UMICOM_EDUCATION_BINARY_INSPECT_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiEducationBinaryFormat {
    UMI_EDUCATION_BINARY_UNKNOWN = 0,
    UMI_EDUCATION_BINARY_ELF = 1,
    UMI_EDUCATION_BINARY_PE = 2,
    UMI_EDUCATION_BINARY_COFF = 3
} UmiEducationBinaryFormat;

/* An inspection result owns no pointers. A caller may copy it by value.
 * PE AddressOfEntryPoint is an RVA, not an absolute virtual address. */
typedef struct UmiEducationBinaryHeader {
    UmiEducationBinaryFormat format;
    uint16_t machine;
    uint16_t fileType;
    uint16_t sections;
    uint8_t addressBits;
    int littleEndian;
    int entryIsRva;
    int sectionCountExtended;
    uint64_t entryPoint;
    uint64_t fileBytes;
} UmiEducationBinaryHeader;

/* Examine a bounded initial byte span. fileBytes is the actual complete file
 * size and must be at least prefixBytes. No executable code is interpreted.
 * All offsets and count*size ranges are checked against fileBytes. On failure,
 * the caller's output is not modified. ELF, PE32/PE32+ and ordinary COFF objects
 * are recognised; extended COFF objects and other formats are not supported.
 * For PE headers beyond the provided prefix, CAPACITY_EXCEEDED is returned.
 * A successful parse describes the header, not the validity of the entire file. */
UmiStatus UmiEducationBinaryInspectBytes(const unsigned char *prefix,
    size_t prefixBytes, uint64_t fileBytes, UmiEducationBinaryHeader *outHeader);

/* Read only the first 64 KiB of one named file. The file is never executed,
 * modified, memory-mapped, recursively scanned or used as a command. The
 * caller chooses the path. Standard C long-size file seeking limits may apply
 * to files larger than LONG_MAX on some hosts. Returns a status and preserves
 * the output on failure. */
/* Publication also waits for a successful file close, so a late close error
 * preserves the caller's previous result. When parsing and closing both fail,
 * the earlier failure is returned. */
UmiStatus UmiEducationBinaryInspectFile(const char *path,
    UmiEducationBinaryHeader *outHeader);

const char *UmiEducationBinaryFormatText(UmiEducationBinaryFormat format);
const char *UmiEducationBinaryMachineText(const UmiEducationBinaryHeader *header);
const char *UmiEducationBinaryTypeText(const UmiEducationBinaryHeader *header);

#ifdef __cplusplus
}
#endif
#endif
