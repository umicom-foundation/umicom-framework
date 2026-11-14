/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/pe_imports.c
 *
 * PURPOSE:
 *   Extend canonical binary-header inspection with bounded DLL-name discovery.
 *
 * EDUCATIONAL OVERVIEW:
 *   Never treat the total import-directory byte size as a descriptor count. The
 *   parser maps RVAs to stored ranges, validates names and terminators, and keeps
 *   its candidate private until the complete dependency-name observation is valid.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/pe_imports.h"
#include <stdlib.h>
#include <string.h>

#define PE_SECTION_LIMIT 96U

typedef struct PeSection { uint32_t address, size, offset; } PeSection;
typedef struct PeView {
    const unsigned char *bytes;
    size_t size;
    uint32_t headers;
    uint64_t base;
    PeSection sections[PE_SECTION_LIMIT];
    size_t section_count;
} PeView;

/* Byte reads are used only after their containing header/table span is checked;
 * they do not depend on pointer alignment or the compiler's structure packing. */
static uint16_t PeU16(const unsigned char *p)
{ return (uint16_t)((uint16_t)p[0] | (uint16_t)((uint16_t)p[1] << 8U)); }

/* Promote before shifts so bytes are never interpreted as signed characters. */
static uint32_t PeU32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8U) |
        ((uint32_t)p[2] << 16U) | ((uint32_t)p[3] << 24U);
}

/* The image base is 64-bit in PE32+ even though import name RVAs remain 32-bit. */
static uint64_t PeU64(const unsigned char *p)
{ return (uint64_t)PeU32(p) | ((uint64_t)PeU32(p + 4U) << 32U); }

/* Subtraction-based bounds avoid offset+length overflow before a memory read. */
static int PeSpan(size_t size, uint64_t offset, uint64_t length)
{ return offset <= (uint64_t)size && length <= (uint64_t)size - offset; }

/* Comparisons use ASCII rather than locale-sensitive case mappings: Windows
 * import basenames admitted by this service are deliberately ASCII-only. */
static unsigned char PeLower(unsigned char ch)
{ return ch >= 'A' && ch <= 'Z' ? (unsigned char)(ch + ('a' - 'A')) : ch; }

/* File names are compared independently of the current process locale. */
static int PeEqual(const char *a, const char *b)
{
    size_t i = 0U;
    while (a[i] != '\0' && b[i] != '\0') {
        if (PeLower((unsigned char)a[i]) != PeLower((unsigned char)b[i])) return 0;
        ++i;
    }
    return a[i] == b[i];
}

/* A dependency is a basename, never a drive, stream, shell argument or path.
 * Restricting it also makes later callers' separate path joins unambiguous. */
static int PeName(const char *name, size_t size)
{
    if (size < 5U || size >= UMI_PE_IMPORT_NAME_CAPACITY ||
        !PeEqual(name + size - 4U, ".dll")) return 0;
    for (size_t i = 0U; i < size; ++i) {
        const unsigned char c = (unsigned char)name[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '+' || c == '.' || c == '-')) return 0;
    }
    return 1;
}

/* RVA mapping accepts only actual file-backed bytes, not zero-filled virtual
 * tails. Header data and section data have separate checked address ranges. */
static int PeOffset(const PeView *view, uint64_t address, size_t length, size_t *out)
{
    if (length == 0U || address + (uint64_t)length > UINT64_C(0x100000000)) return 0;
    if (address < view->headers && PeSpan(view->headers, address, length)) {
        *out = (size_t)address; return 1;
    }
    for (size_t i = 0U; i < view->section_count; ++i) {
        const PeSection *section = &view->sections[i];
        if (address >= section->address) {
            const uint64_t delta = address - section->address;
            if (PeSpan(section->size, delta, length)) {
                *out = (size_t)section->offset + (size_t)delta; return 1;
            }
        }
    }
    return 0;
}

/* Read one bounded terminated name, then deduplicate before publishing it into
 * candidate storage. Invalid or duplicate descriptors never grow output counts. */
static UmiStatus PeImportName(const PeView *view, uint64_t address, UmiEducationPeImports *out)
{
    char name[UMI_PE_IMPORT_NAME_CAPACITY] = {0};
    for (size_t i = 0U; i < sizeof(name); ++i) {
        size_t offset;
        if (!PeOffset(view, address + i, 1U, &offset)) return UMI_STATUS_PARSE_ERROR;
        const unsigned char byte = view->bytes[offset];
        if (byte > 127U) return UMI_STATUS_PARSE_ERROR;
        name[i] = (char)byte;
        if (byte != 0U) continue;
        if (!PeName(name, i)) return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0U; j < out->count; ++j)
            if (PeEqual(name, out->names[j])) return UMI_STATUS_OK;
        if (out->count == UMI_PE_IMPORT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(out->names[out->count], name, i + 1U);
        ++out->count;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

/* A directory can contain descriptor records followed by strings and lookup
 * tables. Stop at the first complete zero record, bounded by declared bytes
 * and by the number of records actually visited; unrelated trailing data does
 * not turn a small dependency list into thousands of alleged descriptors. */
static UmiStatus PeDirectory(const PeView *view, uint32_t address,
    uint32_t length, int delayed, UmiEducationPeImports *out)
{
    const size_t width = delayed ? 32U : 20U;
    size_t offset;
    if (address == 0U && length == 0U) return UMI_STATUS_OK;
    if (address == 0U || length < width || !PeOffset(view, address, length, &offset))
        return UMI_STATUS_PARSE_ERROR;
    size_t records = (size_t)length / width;
    if (records > UMI_PE_IMPORT_CAPACITY + 1U) records = UMI_PE_IMPORT_CAPACITY + 1U;
    for (size_t index = 0U; index < records; ++index) {
        const unsigned char *record = view->bytes + offset + index * width;
        int zero = 1;
        for (size_t i = 0U; i < width; ++i) if (record[i] != 0U) zero = 0;
        if (zero) return UMI_STATUS_OK;
        if (index == UMI_PE_IMPORT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        uint64_t name = PeU32(record + (delayed ? 4U : 12U));
        if (delayed) {
            const uint32_t attributes = PeU32(record);
            if ((attributes & ~UINT32_C(1)) != 0U) return UMI_STATUS_PARSE_ERROR;
            if ((attributes & 1U) == 0U) {
                if (name < view->base) return UMI_STATUS_PARSE_ERROR;
                name -= view->base;
            }
        }
        if (name == 0U) return UMI_STATUS_PARSE_ERROR;
        UmiStatus status = PeImportName(view, name, out);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_PARSE_ERROR;
}

/* Reuse the existing Framework header contract, then add directory-specific
 * structure checks. This extends binary inspection without changing its ABI. */
static UmiStatus PeInspect(const unsigned char *bytes, size_t size, UmiEducationPeImports *out)
{
    UmiEducationBinaryHeader header = {0};
    UmiStatus status = UmiEducationBinaryInspectBytes(bytes, size, size, &header);
    if (status != UMI_STATUS_OK) return status;
    if (header.format != UMI_EDUCATION_BINARY_PE) return UMI_STATUS_NOT_IMPLEMENTED;
    if (header.sections == 0U || header.sections > PE_SECTION_LIMIT) return UMI_STATUS_PARSE_ERROR;
    const size_t pe = PeU32(bytes + 60U), optional = pe + 24U;
    const size_t optional_size = PeU16(bytes + pe + 20U);
    const size_t directory = header.addressBits == 64U ? 112U : 96U;
    if (optional_size < directory) return UMI_STATUS_PARSE_ERROR;
    const uint32_t count = PeU32(bytes + optional + directory - 4U);
    if (count > (optional_size - directory) / 8U) return UMI_STATUS_PARSE_ERROR;
    const size_t section_table = optional + optional_size;
    PeView view = {0};
    view.bytes = bytes; view.size = size; view.section_count = header.sections;
    view.headers = PeU32(bytes + optional + 60U);
    view.base = header.addressBits == 64U ? PeU64(bytes + optional + 24U) : PeU32(bytes + optional + 28U);
    if (view.headers > size || view.headers < section_table + (size_t)header.sections * 40U)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < view.section_count; ++i) {
        const unsigned char *record = bytes + section_table + i * 40U;
        PeSection section = {PeU32(record + 12U), PeU32(record + 16U), PeU32(record + 20U)};
        if (!PeSpan(size, section.offset, section.size) ||
            (uint64_t)section.address + section.size > UINT64_C(0x100000000)) return UMI_STATUS_PARSE_ERROR;
        if (section.size != 0U) {
            if (section.offset < view.headers || section.address < view.headers) return UMI_STATUS_PARSE_ERROR;
            /* Overlapping stored/virtual spans make an RVA ambiguous. Refuse
             * the candidate instead of choosing whichever section comes first. */
            for (size_t j = 0U; j < i; ++j) {
                const PeSection *prior = &view.sections[j];
                if (prior->size == 0U) continue;
                if (((uint64_t)section.offset < (uint64_t)prior->offset + prior->size &&
                     (uint64_t)prior->offset < (uint64_t)section.offset + section.size) ||
                    ((uint64_t)section.address < (uint64_t)prior->address + prior->size &&
                     (uint64_t)prior->address < (uint64_t)section.address + section.size)) return UMI_STATUS_PARSE_ERROR;
            }
        }
        view.sections[i] = section;
    }
    out->machine = header.machine; out->address_bits = header.addressBits;
    /* Ordinary imports precede delayed imports, preserving a deterministic first-observation order. */
    const unsigned indices[] = {1U, 13U};
    for (size_t i = 0U; i < 2U; ++i) {
        if (count <= indices[i]) continue;
        const unsigned char *entry = bytes + optional + directory + indices[i] * 8U;
        status = PeDirectory(&view, PeU32(entry), PeU32(entry + 4U), i == 1U, out);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Allocate the large name table off the stack and publish atomically. Invalid
 * input and allocation failures cannot partially replace an earlier observation. */
UmiStatus UmiEducationPeImportsInspectBytes(const unsigned char *bytes,
    size_t size, UmiEducationPeImports *out_imports)
{
    if (bytes == NULL || out_imports == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (size < 64U) return UMI_STATUS_PARSE_ERROR;
    if (size > UMI_PE_IMPORT_IMAGE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiEducationPeImports *candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = PeInspect(bytes, size, candidate);
    if (status == UMI_STATUS_OK) memcpy(out_imports, candidate, sizeof(*out_imports));
    free(candidate);
    return status;
}
