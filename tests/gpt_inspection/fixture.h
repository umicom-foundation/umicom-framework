/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gpt_inspection/fixture.h
 *
 * PURPOSE:
 *   Build synthetic partition-table bytes and observe bounded reads without file or device access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_GPT_FIXTURE_H
#define UMICOM_TEST_GPT_FIXTURE_H
#include "umicom/image_inspection/gpt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GPT_FIXTURE_BYTES (512U * 1024U)
typedef struct GptFixture {
    unsigned char *data;
    uint32_t block, width, count;
    uint64_t sectors, first, last, backupTable;
    unsigned calls, failAt, outside;
} GptFixture;
static void Fixture32(unsigned char *p, uint32_t value)
{
    for (unsigned i = 0U; i < 4U; ++i) p[i] = (unsigned char)(value >> (i * 8U));
}
static void Fixture64(unsigned char *p, uint64_t value)
{
    for (unsigned i = 0U; i < 8U; ++i) p[i] = (unsigned char)(value >> (i * 8U));
}
static uint32_t FixtureChecksum(const unsigned char *data, size_t count)
{
    uint32_t crc = UINT32_MAX;
    for (size_t i = 0U; i < count; ++i) {
        crc ^= data[i];
        for (unsigned bit = 0U; bit < 8U; ++bit)
            crc = (crc & 1U) ? (crc >> 1U) ^ UINT32_C(0xedb88320) : crc >> 1U;
    }
    return ~crc;
}
static unsigned char *FixtureHeader(GptFixture *f, unsigned backup)
{
    return f->data + (backup ? f->sectors - 1U : 1U) * f->block;
}
static unsigned char *FixtureEntry(GptFixture *f, unsigned index)
{
    return f->data + 2U * f->block + (size_t)index * f->width;
}
static void FixtureHeaderChecksum(GptFixture *f, unsigned backup, uint32_t size)
{
    unsigned char *header = FixtureHeader(f, backup);
    Fixture32(header + 16U, 0U);
    Fixture32(header + 16U, FixtureChecksum(header, size));
}
static void FixtureTables(GptFixture *f)
{
    size_t count = (size_t)f->count * f->width;
    unsigned char *entries = FixtureEntry(f, 0U);
    uint32_t crc = FixtureChecksum(entries, count);
    memcpy(f->data + f->backupTable * f->block, entries, count);
    for (unsigned b = 0U; b < 2U; ++b) {
        Fixture32(FixtureHeader(f, b) + 88U, crc);
        FixtureHeaderChecksum(f, b, 92U);
    }
}
static int FixtureCreate(GptFixture *f, uint32_t block, uint32_t count, uint32_t width)
{
    memset(f, 0, sizeof(*f));
    f->data = calloc(1U, GPT_FIXTURE_BYTES);
    if (f->data == NULL) return 0;
    f->block = block; f->count = count; f->width = width;
    f->sectors = GPT_FIXTURE_BYTES / block;
    uint64_t reserved = (uint64_t)count * width;
    if (reserved < 16384U) reserved = 16384U;
    reserved = (reserved + block - 1U) / block;
    f->first = 2U + reserved;
    f->backupTable = f->sectors - 1U - reserved;
    f->last = f->backupTable - 1U;
    for (unsigned b = 0U; b < 2U; ++b) {
        unsigned char *header = FixtureHeader(f, b);
        memcpy(header, "EFI PART", 8U);
        Fixture32(header + 8U, UINT32_C(0x10000));
        Fixture32(header + 12U, 92U);
        Fixture64(header + 24U, b ? f->sectors - 1U : 1U);
        Fixture64(header + 32U, b ? 1U : f->sectors - 1U);
        Fixture64(header + 40U, f->first);
        Fixture64(header + 48U, f->last);
        header[56U] = 1U;
        Fixture64(header + 72U, b ? f->backupTable : 2U);
        Fixture32(header + 80U, count);
        Fixture32(header + 84U, width);
    }
    unsigned char *entry = FixtureEntry(f, 0U);
    entry[0U] = 1U; entry[16U] = 2U;
    Fixture64(entry + 32U, f->first);
    Fixture64(entry + 40U, f->first + 10U);
    FixtureTables(f);
    return 1;
}
static UmiStatus FixtureRead(void *context, uint64_t offset, void *destination, size_t length)
{
    GptFixture *f = context;
    ++f->calls;
    if (f->failAt == f->calls) return UMI_STATUS_IO_ERROR;
    if (offset > GPT_FIXTURE_BYTES || length > GPT_FIXTURE_BYTES - offset) {
        ++f->outside;
        return UMI_STATUS_IO_ERROR;
    }
    memcpy(destination, f->data + (size_t)offset, length);
    return UMI_STATUS_OK;
}
#endif
