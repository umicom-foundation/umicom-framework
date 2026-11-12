/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/image_inspection/gpt.c
 *
 * PURPOSE:
 *   Validate bounded GPT geometry before allocating and publish only a complete observation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/image_inspection/gpt.h"
#include <stdlib.h>
#include <string.h>

static uint32_t Read32(const unsigned char *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8U |
        (uint32_t)p[2] << 16U | (uint32_t)p[3] << 24U;
}
static uint64_t Read64(const unsigned char *p)
{
    return (uint64_t)Read32(p) | (uint64_t)Read32(p + 4U) << 32U;
}
static int IsZero(const unsigned char *p, size_t count)
{
    for (size_t i = 0U; i < count; ++i) if (p[i] != 0U) return 0;
    return 1;
}
static uint32_t Checksum(const unsigned char *bytes, size_t count)
{
    uint32_t crc = UINT32_MAX;
    for (size_t i = 0U; i < count; ++i) {
        crc ^= bytes[i];
        for (unsigned bit = 0U; bit < 8U; ++bit)
            crc = (crc >> 1U) ^ (UINT32_C(0xedb88320) & (0U - (crc & 1U)));
    }
    return ~crc;
}
static UmiStatus Header(UmiGptReadAt readAt, void *context, uint64_t lba,
    uint64_t sectors, uint32_t block, unsigned char *header)
{
    if (lba >= sectors) return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = readAt(context, lba * block, header, block);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(header, "EFI PART", 8U) != 0) return UMI_STATUS_PARSE_ERROR;
    if (Read32(header + 8U) != UINT32_C(0x10000)) return UMI_STATUS_UNAVAILABLE;
    uint32_t size = Read32(header + 12U), expected = Read32(header + 16U);
    if (size < 92U || size > block || Read32(header + 20U) != 0U ||
        Read64(header + 24U) != lba || !IsZero(header + 92U, block - 92U))
        return UMI_STATUS_PARSE_ERROR;
    /* Only the scratch header is changed. The caller's bytes remain borrowed.
     * Restore the checksum field so primary/backup comparison sees full data. */
    memset(header + 16U, 0, 4U);
    uint32_t actual = Checksum(header, size);
    for (unsigned i = 0U; i < 4U; ++i)
        header[16U + i] = (unsigned char)(expected >> (i * 8U));
    return actual == expected ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus UmiGptImageInspect(UmiGptReadAt readAt, void *context,
    uint64_t bytes, uint32_t block, UmiGptImageInfo *out)
{
    if (readAt == NULL || out == NULL || (block != 512U && block != 4096U) ||
        bytes == 0U || bytes % block != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t sectors = bytes / block;
    if (sectors < 3U) return UMI_STATUS_PARSE_ERROR;
    unsigned char primary[4096], backupHeader[4096];
    UmiStatus status = Header(readAt, context, 1U, sectors, block, primary);
    if (status != UMI_STATUS_OK) return status;
    uint64_t backup = Read64(primary + 32U);
    uint64_t first = Read64(primary + 40U), last = Read64(primary + 48U);
    uint64_t table = Read64(primary + 72U);
    uint32_t count = Read32(primary + 80U), width = Read32(primary + 84U);
    if (backup != sectors - 1U || first < 3U || first > last || last >= backup ||
        table < 2U || table >= first || count == 0U || width < 128U ||
        (width & (width - 1U)) != 0U || IsZero(primary + 56U, 16U))
        return UMI_STATUS_PARSE_ERROR;
    if (count > 256U || width > 512U) return UMI_STATUS_UNAVAILABLE;
    size_t length = (size_t)count * width;
    /* A small advertised array still needs its format-mandated reservation.
     * Subtraction after ordered-bound checks prevents wrapped LBA arithmetic. */
    uint64_t reservedBytes = length < 16384U ? 16384U : (uint64_t)length;
    uint64_t reservedBlocks = (reservedBytes + block - 1U) / block;
    if (reservedBlocks > first - table) return UMI_STATUS_PARSE_ERROR;
    status = Header(readAt, context, backup, sectors, block, backupHeader);
    if (status != UMI_STATUS_OK) return status;
    uint64_t other = Read64(backupHeader + 72U);
    if (Read64(backupHeader + 32U) != 1U ||
        Read32(backupHeader + 12U) != Read32(primary + 12U) ||
        Read64(backupHeader + 40U) != first || Read64(backupHeader + 48U) != last ||
        memcmp(primary + 56U, backupHeader + 56U, 16U) != 0 ||
        Read32(backupHeader + 80U) != count || Read32(backupHeader + 84U) != width ||
        Read32(backupHeader + 88U) != Read32(primary + 88U) ||
        other <= last || other >= backup || reservedBlocks > backup - other)
        return UMI_STATUS_PARSE_ERROR;
    unsigned char *entries = malloc(length), *copy = malloc(length);
    if (entries == NULL || copy == NULL) {
        free(entries); free(copy);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = readAt(context, table * block, entries, length);
    if (status == UMI_STATUS_OK) status = readAt(context, other * block, copy, length);
    if (status == UMI_STATUS_OK &&
        (Checksum(entries, length) != Read32(primary + 88U) ||
         memcmp(entries, copy, length) != 0))
        status = UMI_STATUS_PARSE_ERROR;
    UmiGptImageInfo candidate = { .logicalBlockBytes = block,
        .firstUsableLba = first, .lastUsableLba = last };
    for (uint32_t i = 0U; status == UMI_STATUS_OK && i < count; ++i) {
        const unsigned char *entry = entries + (size_t)i * width;
        /* An unused type GUID makes legacy identity/range fields irrelevant.
         * Retain the over-strict draft for review; only reserved extension bytes
         * have a zero requirement independent of whether the slot is active. */
#if 0
        if (IsZero(entry, 16U)) {
            if (!IsZero(entry, width)) status = UMI_STATUS_PARSE_ERROR;
            continue;
        }
#endif
        if (!IsZero(entry + 128U, width - 128U)) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        if (IsZero(entry, 16U)) continue;
        uint64_t begin = Read64(entry + 32U), end = Read64(entry + 40U);
        if (begin < first || end > last || begin > end || IsZero(entry + 16U, 16U) ||
            !IsZero(entry + 128U, width - 128U) ||
            (Read64(entry + 48U) & UINT64_C(0x0000fffffffffff8)) != 0U) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        for (uint32_t j = 0U; j < i; ++j) {
            const unsigned char *previous = entries + (size_t)j * width;
            if (!IsZero(previous, 16U) &&
                ((begin <= Read64(previous + 40U) && end >= Read64(previous + 32U)) ||
                 memcmp(entry + 16U, previous + 16U, 16U) == 0)) {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
        }
        if (status == UMI_STATUS_OK) ++candidate.partitions;
    }
    free(entries); free(copy);
    if (status == UMI_STATUS_OK) *out = candidate;
    return status;
}
