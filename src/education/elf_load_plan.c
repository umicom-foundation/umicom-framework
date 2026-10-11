/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/elf_load_plan.c
 * PURPOSE:
 *   Explain ELF loadable memory segments safely from file metadata alone.
 *   Deliberately does not become a user-space or kernel executable loader.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/education/elf_load_plan.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/types.h>
#endif

#define ELF_PROGRAM_LOAD 1U
#define ELF_TYPE_EXECUTABLE 2U
#define ELF_TYPE_SHARED 3U
#define ELF_HEADER_EXTENDED_COUNT 0xffffU

typedef struct EducationElfPrelude {
    uint64_t tableOffset;
    uint64_t tableBytes;
    uint64_t entryPoint;
    uint16_t entrySize;
    uint16_t headerCount;
    uint16_t machine;
    uint16_t elfType;
    uint8_t addressBits;
    uint8_t littleEndian;
} EducationElfPrelude;

/* Subtract before adding so even malicious UINT64_MAX offsets are rejected
 * without wrapping into a seemingly legitimate region of memory or file. */
static int EducationFits(uint64_t offset, uint64_t length, uint64_t extent)
{
    return offset <= extent && length <= extent - offset;
}

static uint64_t EducationNumber(const unsigned char *bytes, size_t position,
                                unsigned width, int little)
{
    uint64_t value = 0U;
    if (little) {
        for (unsigned index = width; index > 0U; --index)
            value = (value << 8U) | bytes[position + (size_t)index - 1U];
    } else {
        for (unsigned index = 0U; index < width; ++index)
            value = (value << 8U) | bytes[position + index];
    }
    return value;
}

/* Only the byte-order-independent ELF identification fields may be examined
 * before the complete 52/64-byte ELF header has been verified. */
static UmiStatus EducationPrelude(const unsigned char *bytes, size_t supplied,
                                  uint64_t fileBytes, EducationElfPrelude *out)
{
    if (bytes == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (supplied < 4U || memcmp(bytes, "\177ELF", 4U) != 0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (supplied < 16U) return UMI_STATUS_PARSE_ERROR;
    const int bits = bytes[4] == 1U ? 32 : bytes[4] == 2U ? 64 : 0;
    const int little = bytes[5] == 1U;
    if (bits == 0 || (bytes[5] != 1U && bytes[5] != 2U) || bytes[6] != 1U)
        return UMI_STATUS_PARSE_ERROR;
    const uint64_t header = bits == 64 ? 64U : 52U;
    if (supplied < header || fileBytes < header || (uint64_t)supplied > fileBytes)
        return UMI_STATUS_PARSE_ERROR;
    if (EducationNumber(bytes, 20U, 4U, little) != 1U)
        return UMI_STATUS_PARSE_ERROR;
    const uint64_t declared = EducationNumber(bytes, bits == 64 ? 52U : 40U, 2U, little);
    if (declared != header) return UMI_STATUS_PARSE_ERROR;
    EducationElfPrelude plan = {0};
    plan.addressBits = (uint8_t)bits;
    plan.littleEndian = (uint8_t)little;
    plan.elfType = (uint16_t)EducationNumber(bytes, 16U, 2U, little);
    plan.machine = (uint16_t)EducationNumber(bytes, 18U, 2U, little);
    if (plan.elfType != ELF_TYPE_EXECUTABLE && plan.elfType != ELF_TYPE_SHARED)
        return UMI_STATUS_NOT_IMPLEMENTED;
    plan.entryPoint = EducationNumber(bytes, bits == 64 ? 24U : 24U,
                                       bits == 64 ? 8U : 4U, little);
    plan.tableOffset = EducationNumber(bytes, bits == 64 ? 32U : 28U,
                                       bits == 64 ? 8U : 4U, little);
    plan.entrySize = (uint16_t)EducationNumber(bytes, bits == 64 ? 54U : 42U, 2U, little);
    plan.headerCount = (uint16_t)EducationNumber(bytes, bits == 64 ? 56U : 44U, 2U, little);
    if (plan.headerCount == ELF_HEADER_EXTENDED_COUNT)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (plan.headerCount == 0U || plan.tableOffset == 0U ||
        plan.entrySize != (bits == 64 ? 56U : 32U) ||
        plan.tableOffset < header) return UMI_STATUS_PARSE_ERROR;
    if (plan.headerCount > UMI_EDUCATION_ELF_LOAD_MAX_PROGRAM_HEADERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    plan.tableBytes = (uint64_t)plan.entrySize * plan.headerCount;
    if (!EducationFits(plan.tableOffset, plan.tableBytes, fileBytes))
        return UMI_STATUS_PARSE_ERROR;
    if (plan.tableOffset + plan.tableBytes > UMI_EDUCATION_ELF_LOAD_MAX_HEADER_PREFIX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *out = plan;
    return UMI_STATUS_OK;
}

static int EducationIsPowerOfTwo(uint64_t number)
{
    return number != 0U && (number & (number - 1U)) == 0U;
}

static int EducationRangesOverlap(uint64_t first, uint64_t firstEnd,
                                  uint64_t second, uint64_t secondEnd)
{
    return first < secondEnd && second < firstEnd;
}

UmiStatus UmiEducationElfLoadPlanInspectBytes(const void *input, size_t suppliedBytes,
    uint64_t actualFileBytes, UmiEducationElfLoadPlan *outPlan)
{
    if (outPlan == NULL || input == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const unsigned char *bytes = input;
    EducationElfPrelude header;
    const UmiStatus preliminary = EducationPrelude(bytes, suppliedBytes, actualFileBytes, &header);
    if (preliminary != UMI_STATUS_OK) return preliminary;
    if (!EducationFits(header.tableOffset, header.tableBytes, (uint64_t)suppliedBytes))
        return UMI_STATUS_PARSE_ERROR;
    UmiEducationElfLoadPlan plan = {0};
    plan.inputFileBytes = actualFileBytes;
    plan.entryAddress = header.entryPoint;
    plan.elfType = header.elfType;
    plan.machine = header.machine;
    plan.programHeaderCount = header.headerCount;
    plan.addressBits = header.addressBits;
    plan.littleEndian = header.littleEndian;
    plan.minimumVirtualAddress = UINT64_MAX;

    for (uint32_t index = 0U; index < header.headerCount; ++index) {
        const uint64_t where = header.tableOffset + (uint64_t)index * header.entrySize;
        const size_t position = (size_t)where;
        const int little = header.littleEndian != 0U;
        if (EducationNumber(bytes, position, 4U, little) != ELF_PROGRAM_LOAD) continue;
        if (plan.segmentCount >= UMI_EDUCATION_ELF_LOAD_MAX_SEGMENTS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        UmiEducationElfLoadSegment segment = {0};
        if (header.addressBits == 64U) {
            segment.flags = (uint32_t)EducationNumber(bytes, position + 4U, 4U, little);
            segment.fileOffset = EducationNumber(bytes, position + 8U, 8U, little);
            segment.virtualAddress = EducationNumber(bytes, position + 16U, 8U, little);
            segment.physicalAddressHint = EducationNumber(bytes, position + 24U, 8U, little);
            segment.fileBytes = EducationNumber(bytes, position + 32U, 8U, little);
            segment.memoryBytes = EducationNumber(bytes, position + 40U, 8U, little);
            segment.alignment = EducationNumber(bytes, position + 48U, 8U, little);
        } else {
            segment.fileOffset = EducationNumber(bytes, position + 4U, 4U, little);
            segment.virtualAddress = EducationNumber(bytes, position + 8U, 4U, little);
            segment.physicalAddressHint = EducationNumber(bytes, position + 12U, 4U, little);
            segment.fileBytes = EducationNumber(bytes, position + 16U, 4U, little);
            segment.memoryBytes = EducationNumber(bytes, position + 20U, 4U, little);
            segment.flags = (uint32_t)EducationNumber(bytes, position + 24U, 4U, little);
            segment.alignment = EducationNumber(bytes, position + 28U, 4U, little);
        }
        if (segment.fileBytes > segment.memoryBytes ||
            !EducationFits(segment.fileOffset, segment.fileBytes, actualFileBytes) ||
            !EducationFits(segment.virtualAddress, segment.memoryBytes, UINT64_MAX) ||
            (segment.alignment > 1U &&
             (!EducationIsPowerOfTwo(segment.alignment) ||
              segment.fileOffset % segment.alignment != segment.virtualAddress % segment.alignment)))
            return UMI_STATUS_PARSE_ERROR;
        const uint64_t end = segment.virtualAddress + segment.memoryBytes;
        if (header.addressBits == 32U && end > UINT32_MAX + UINT64_C(1))
            return UMI_STATUS_PARSE_ERROR;
        for (uint32_t prior = 0U; prior < plan.segmentCount; ++prior) {
            const UmiEducationElfLoadSegment *earlier = &plan.segments[prior];
            const uint64_t earlierEnd = earlier->virtualAddress + earlier->memoryBytes;
            if (EducationRangesOverlap(earlier->virtualAddress, earlierEnd,
                                       segment.virtualAddress, end))
                return UMI_STATUS_PARSE_ERROR;
        }
        if (!EducationFits(plan.zeroFillBytes,
                           segment.memoryBytes - segment.fileBytes, UINT64_MAX) ||
            !EducationFits(plan.fileBackedBytes, segment.fileBytes, UINT64_MAX))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        plan.zeroFillBytes += segment.memoryBytes - segment.fileBytes;
        plan.fileBackedBytes += segment.fileBytes;
        if (segment.memoryBytes > 0U) {
            if (segment.virtualAddress < plan.minimumVirtualAddress)
                plan.minimumVirtualAddress = segment.virtualAddress;
            if (end > plan.maximumVirtualAddressExclusive)
                plan.maximumVirtualAddressExclusive = end;
        }
        if ((segment.flags & UMI_EDUCATION_ELF_SEGMENT_EXECUTE) != 0U) {
            ++plan.executableSegmentCount;
            if (header.entryPoint >= segment.virtualAddress && header.entryPoint < end)
                plan.entryInExecutableSegment = 1U;
        }
        plan.segments[plan.segmentCount++] = segment;
    }
    if (plan.segmentCount == 0U || plan.minimumVirtualAddress == UINT64_MAX ||
        (plan.entryAddress != 0U && !plan.entryInExecutableSegment))
        return UMI_STATUS_PARSE_ERROR;
    *outPlan = plan;
    return UMI_STATUS_OK;
}

/* Large binaries are not allocated. Only enough bytes for the ELF header and
 * program-header table are read. Other file contents remain untouched. */
UmiStatus UmiEducationElfLoadPlanInspectFile(const char *path,
    UmiEducationElfLoadPlan *outPlan)
{
    if (path == NULL || path[0] == '\0' || outPlan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *file = fopen(path, "rb");
    if (file == NULL) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    UmiEducationElfLoadPlan completed;
#if defined(_WIN32)
    if (_fseeki64(file, 0, SEEK_END) != 0) goto finish;
    const __int64 signedSize = _ftelli64(file);
#else
    if (fseeko(file, 0, SEEK_END) != 0) goto finish;
    const off_t signedSize = ftello(file);
#endif
    if (signedSize < 0) goto finish;
    const uint64_t fileSize = (uint64_t)signedSize;
    if (fileSize < 64U) { status = UMI_STATUS_PARSE_ERROR; goto finish; }
#if defined(_WIN32)
    if (_fseeki64(file, 0, SEEK_SET) != 0) goto finish;
#else
    if (fseeko(file, 0, SEEK_SET) != 0) goto finish;
#endif
    unsigned char header[64];
    if (fread(header, 1U, sizeof header, file) != sizeof header) goto finish;
    EducationElfPrelude plan;
    status = EducationPrelude(header, sizeof header, fileSize, &plan);
    if (status != UMI_STATUS_OK) goto finish;
    const uint64_t needed = plan.tableOffset + plan.tableBytes;
    const size_t prefix = (size_t)(needed > 64U ? needed : 64U);
    unsigned char *bytes = malloc(prefix);
    if (bytes == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto finish; }
#if defined(_WIN32)
    if (_fseeki64(file, 0, SEEK_SET) != 0)
#else
    if (fseeko(file, 0, SEEK_SET) != 0)
#endif
        status = UMI_STATUS_IO_ERROR;
    else if (fread(bytes, 1U, prefix, file) != prefix)
        status = UMI_STATUS_IO_ERROR;
    else
        status = UmiEducationElfLoadPlanInspectBytes(bytes, prefix, fileSize, &completed);
    free(bytes);
finish:
    if (fclose(file) != 0 && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) *outPlan = completed;
    return status;
}
