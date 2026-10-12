/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/binary_inspect.c
 *
 * PURPOSE:
 *   Read ELF, PE and COFF header fields through bounded C23 byte operations.
 *   No casts from an untrusted byte buffer to a packed compiler structure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/binary_inspect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_EDUCATION_BINARY_PREFIX_LIMIT 65536U

static int BinaryContains(size_t available, uint64_t offset, uint64_t bytes)
{
    return offset <= (uint64_t)available &&
           bytes <= (uint64_t)available - offset;
}

/* The check avoids offset+count*itemSize overflow and checks against the
 * complete file, which need not fit in the bounded header prefix. */
static int BinaryFileTableFits(uint64_t fileBytes, uint64_t offset,
                                uint64_t count, uint64_t itemSize)
{
    return offset <= fileBytes && (count == 0U ||
        (itemSize != 0U && count <= (fileBytes - offset) / itemSize));
}

static uint16_t BinaryU16(const unsigned char *bytes, int little)
{
    return little ? (uint16_t)((uint16_t)bytes[0] | (uint16_t)((uint16_t)bytes[1] << 8U)) :
        (uint16_t)((uint16_t)bytes[1] | (uint16_t)((uint16_t)bytes[0] << 8U));
}

static uint32_t BinaryU32(const unsigned char *bytes, int little)
{
    uint32_t number = 0U;
    for (size_t i = 0U; i < 4U; ++i) {
        size_t index = little ? (3U - i) : i;
        number = (number << 8U) | (uint32_t)bytes[index];
    }
    return number;
}

static uint64_t BinaryU64(const unsigned char *bytes, int little)
{
    uint64_t number = 0U;
    for (size_t i = 0U; i < 8U; ++i) {
        size_t index = little ? (7U - i) : i;
        number = (number << 8U) | (uint64_t)bytes[index];
    }
    return number;
}

static UmiStatus BinaryInspectElf(const unsigned char *bytes, size_t prefixBytes,
                                  uint64_t fileBytes, UmiEducationBinaryHeader *result)
{
    const int wordBytes = bytes[4] == 1U ? 4 : bytes[4] == 2U ? 8 : 0;
    const int little = bytes[5] == 1U ? 1 : bytes[5] == 2U ? 0 : -1;
    if (wordBytes == 0 || little < 0 || bytes[6] != 1U)
        return UMI_STATUS_PARSE_ERROR;
    const size_t headerBytes = wordBytes == 8 ? 64U : 52U;
    if (!BinaryContains(prefixBytes, 0U, headerBytes) || fileBytes < headerBytes)
        return UMI_STATUS_PARSE_ERROR;
    const size_t headerSizeOffset = wordBytes == 8 ? 52U : 40U;
    const size_t phSizeOffset = wordBytes == 8 ? 54U : 42U;
    const size_t phCountOffset = wordBytes == 8 ? 56U : 44U;
    const size_t shSizeOffset = wordBytes == 8 ? 58U : 46U;
    const size_t shCountOffset = wordBytes == 8 ? 60U : 48U;
    const uint16_t declaredHeader = BinaryU16(bytes + headerSizeOffset, little);
    if ((size_t)declaredHeader < headerBytes || (uint64_t)declaredHeader > fileBytes)
        return UMI_STATUS_PARSE_ERROR;
    const uint64_t phOffset = wordBytes == 8 ? BinaryU64(bytes + 32U, little) :
                                                    (uint64_t)BinaryU32(bytes + 28U, little);
    const uint64_t shOffset = wordBytes == 8 ? BinaryU64(bytes + 40U, little) :
                                                    (uint64_t)BinaryU32(bytes + 32U, little);
    const uint16_t phCount = BinaryU16(bytes + phCountOffset, little);
    const uint16_t shCount = BinaryU16(bytes + shCountOffset, little);
    const uint16_t phSize = BinaryU16(bytes + phSizeOffset, little);
    const uint16_t shSize = BinaryU16(bytes + shSizeOffset, little);
    if (phOffset > fileBytes || shOffset > fileBytes)
        return UMI_STATUS_PARSE_ERROR;
    if ((phCount != 0U && (phSize < (wordBytes == 8 ? 56U : 32U) ||
        !BinaryFileTableFits(fileBytes, phOffset, phCount, phSize))) ||
        (shCount != 0U && (shSize < (wordBytes == 8 ? 64U : 40U) ||
        !BinaryFileTableFits(fileBytes, shOffset, shCount, shSize))))
        return UMI_STATUS_PARSE_ERROR;
    /* Zero section count with a non-zero table offset signifies the extended
     * numbering convention. Do not invent a section count from missing data. */
    if (shCount == 0U && shOffset != 0U &&
        !BinaryFileTableFits(fileBytes, shOffset, 1U, shSize))
        return UMI_STATUS_PARSE_ERROR;
    *result = (UmiEducationBinaryHeader){
        .format = UMI_EDUCATION_BINARY_ELF,
        .machine = BinaryU16(bytes + 18U, little),
        .fileType = BinaryU16(bytes + 16U, little),
        .sections = shCount,
        .addressBits = (uint8_t)(wordBytes * 8),
        .littleEndian = little,
        .entryIsRva = 0,
        .sectionCountExtended = shCount == 0U && shOffset != 0U,
        .entryPoint = wordBytes == 8 ? BinaryU64(bytes + 24U, little) :
                                      (uint64_t)BinaryU32(bytes + 24U, little),
        .fileBytes = fileBytes
    };
    return UMI_STATUS_OK;
}

static UmiStatus BinaryInspectPe(const unsigned char *bytes, size_t prefixBytes,
                                 uint64_t fileBytes, UmiEducationBinaryHeader *result)
{
    if (!BinaryContains(prefixBytes, 0U, 64U) || fileBytes < 64U)
        return UMI_STATUS_PARSE_ERROR;
    const uint32_t offset = BinaryU32(bytes + 0x3cU, 1);
    if (offset < 64U || !BinaryFileTableFits(fileBytes, offset, 1U, 24U))
        return UMI_STATUS_PARSE_ERROR;
    if (!BinaryContains(prefixBytes, offset, 24U))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memcmp(bytes + offset, "PE\0\0", 4U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    const uint16_t machine = BinaryU16(bytes + offset + 4U, 1);
    const uint16_t sections = BinaryU16(bytes + offset + 6U, 1);
    const uint16_t optSize = BinaryU16(bytes + offset + 20U, 1);
    const uint64_t optionalOffset = (uint64_t)offset + 24U;
    if (optSize < 20U || !BinaryFileTableFits(fileBytes, optionalOffset, 1U, optSize))
        return UMI_STATUS_PARSE_ERROR;
    if (!BinaryContains(prefixBytes, optionalOffset, 20U))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const uint16_t magic = BinaryU16(bytes + optionalOffset, 1);
    if (magic != 0x10bU && magic != 0x20bU)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (!BinaryFileTableFits(fileBytes, optionalOffset + optSize, sections, 40U))
        return UMI_STATUS_PARSE_ERROR;
    *result = (UmiEducationBinaryHeader){
        .format = UMI_EDUCATION_BINARY_PE, .machine = machine,
        .fileType = magic, .sections = sections,
        .addressBits = (uint8_t)(magic == 0x20bU ? 64 : 32),
        .littleEndian = 1, .entryIsRva = 1,
        .entryPoint = (uint64_t)BinaryU32(bytes + optionalOffset + 16U, 1),
        .fileBytes = fileBytes
    };
    return UMI_STATUS_OK;
}

static int BinaryCoffMachine(uint16_t machine)
{
    return machine == 0x014cU || machine == 0x8664U || machine == 0xaa64U ||
           machine == 0x5032U || machine == 0x5064U;
}

static UmiStatus BinaryInspectCoff(const unsigned char *bytes, size_t prefixBytes,
                                   uint64_t fileBytes, UmiEducationBinaryHeader *result)
{
    if (!BinaryContains(prefixBytes, 0U, 20U) || fileBytes < 20U)
        return UMI_STATUS_PARSE_ERROR;
    const uint16_t machine = BinaryU16(bytes, 1);
    const uint16_t sections = BinaryU16(bytes + 2U, 1);
    const uint16_t optionalSize = BinaryU16(bytes + 16U, 1);
    if (!BinaryCoffMachine(machine)) return UMI_STATUS_NOT_IMPLEMENTED;
    if (optionalSize != 0U || sections == 0U ||
        !BinaryFileTableFits(fileBytes, 20U, sections, 40U))
        return UMI_STATUS_PARSE_ERROR;
    *result = (UmiEducationBinaryHeader){
        .format = UMI_EDUCATION_BINARY_COFF, .machine = machine,
        .sections = sections,
        .addressBits = (uint8_t)(machine == 0x014cU || machine == 0x5032U ? 32 : 64),
        .littleEndian = 1, .fileBytes = fileBytes
    };
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationBinaryInspectBytes(const unsigned char *prefix,
    size_t prefixBytes, uint64_t fileBytes, UmiEducationBinaryHeader *outHeader)
{
    if (outHeader == NULL || (prefix == NULL && prefixBytes != 0U) ||
        (uint64_t)prefixBytes > fileBytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (prefixBytes < 4U) return UMI_STATUS_PARSE_ERROR;
    UmiEducationBinaryHeader proposed = {0};
    UmiStatus status;
    if (memcmp(prefix, "\x7f" "ELF", 4U) == 0) {
        if (prefixBytes < 16U) return UMI_STATUS_PARSE_ERROR;
        status = BinaryInspectElf(prefix, prefixBytes, fileBytes, &proposed);
    } else if (prefix[0] == 'M' && prefix[1] == 'Z') {
        status = BinaryInspectPe(prefix, prefixBytes, fileBytes, &proposed);
    } else {
        status = BinaryInspectCoff(prefix, prefixBytes, fileBytes, &proposed);
    }
    if (status == UMI_STATUS_OK) *outHeader = proposed;
    return status;
}

UmiStatus UmiEducationBinaryInspectFile(const char *path, UmiEducationBinaryHeader *outHeader)
{
    if (path == NULL || path[0] == '\0' || outHeader == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_IO_ERROR;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    /* Keep the parsed header private while this function still owns the file.
     * A later close error must leave the caller's previous result intact, just
     * as a read or parse error does. Future inspection stages should also work
     * on this candidate until all fallible work has finished. */
    UmiEducationBinaryHeader candidate = {0};
    if (fseek(stream, 0L, SEEK_END) != 0) goto finished;
    const long end = ftell(stream);
    if (end < 0L || fseek(stream, 0L, SEEK_SET) != 0) goto finished;
    const size_t prefixBytes = (unsigned long)end < UMI_EDUCATION_BINARY_PREFIX_LIMIT ?
        (size_t)end : UMI_EDUCATION_BINARY_PREFIX_LIMIT;
    unsigned char *bytes = malloc(prefixBytes == 0U ? 1U : prefixBytes);
    if (bytes == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto finished; }
    /* The direct publication path below is retained for engineering review.
     * It was superseded by parsing into a candidate because fclose can fail
     * after parsing succeeds, before the caller may safely use a new result. */
#if 0
    if (fread(bytes, 1U, prefixBytes, stream) == prefixBytes)
        status = UmiEducationBinaryInspectBytes(bytes, prefixBytes,
            (uint64_t)(unsigned long)end, outHeader);
#endif
    if (fread(bytes, 1U, prefixBytes, stream) == prefixBytes)
        status = UmiEducationBinaryInspectBytes(bytes, prefixBytes,
            (uint64_t)(unsigned long)end, &candidate);
    free(bytes);
finished:
    if (fclose(stream) != 0 && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    /* Publish once, after releasing every owned resource. Earlier failures
     * retain their original status even when closing also reports an error. */
    if (status == UMI_STATUS_OK)
        *outHeader = candidate;
    return status;
}

const char *UmiEducationBinaryFormatText(UmiEducationBinaryFormat format)
{
    switch (format) {
        case UMI_EDUCATION_BINARY_ELF: return "ELF";
        case UMI_EDUCATION_BINARY_PE: return "PE";
        case UMI_EDUCATION_BINARY_COFF: return "COFF";
        default: return "Unknown";
    }
}

const char *UmiEducationBinaryMachineText(const UmiEducationBinaryHeader *header)
{
    if (header == NULL) return "Unknown";
    switch (header->format) {
        case UMI_EDUCATION_BINARY_ELF:
            switch (header->machine) {
                case 3U: return "x86 (32-bit)";
                case 40U: return "ARM";
                case 62U: return "x86-64";
                case 183U: return "AArch64";
                case 243U: return "RISC-V";
                default: return "Other ELF machine";
            }
        case UMI_EDUCATION_BINARY_PE:
        case UMI_EDUCATION_BINARY_COFF:
            switch (header->machine) {
                case 0x014cU: return "x86 (32-bit)";
                case 0x8664U: return "x86-64";
                case 0xaa64U: return "ARM64";
                case 0x5032U: return "RISC-V32";
                case 0x5064U: return "RISC-V64";
                default: return "Other COFF machine";
            }
        default: return "Unknown";
    }
}

const char *UmiEducationBinaryTypeText(const UmiEducationBinaryHeader *header)
{
    if (header == NULL) return "Unknown";
    if (header->format == UMI_EDUCATION_BINARY_PE)
        return "Portable Executable image";
    if (header->format == UMI_EDUCATION_BINARY_COFF)
        return "Relocatable object";
    if (header->format == UMI_EDUCATION_BINARY_ELF) {
        switch (header->fileType) {
            case 1U: return "Relocatable object";
            case 2U: return "Executable image";
            case 3U: return "Shared object or position-independent executable";
            case 4U: return "Core file";
            default: return "Other ELF file type";
        }
    }
    return "Unknown";
}
