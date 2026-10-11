/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/elf_symbols.c
 * PURPOSE:
 *   Read ELF symbol/relocation metadata using checked byte ranges and explicit
 *   byte-order conversion. No host ELF structs, pointer casts or file loading.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/elf_symbols.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EDUCATION_ELF_SECTION_LIMIT 65536U
#define EDUCATION_ELF_SHT_SYMTAB 2U
#define EDUCATION_ELF_SHT_STRTAB 3U
#define EDUCATION_ELF_SHT_RELA 4U
#define EDUCATION_ELF_SHT_REL 9U
#define EDUCATION_ELF_SHT_DYNSYM 11U

typedef struct ElfView {
    const unsigned char *bytes;
    uint64_t length;
    uint64_t sectionOffset;
    uint32_t sectionCount;
    uint32_t sectionEntrySize;
    unsigned bits;
    int little;
} ElfView;

typedef struct ElfSection {
    uint64_t offset;
    uint64_t length;
    uint64_t entrySize;
    uint32_t type;
    uint32_t link;
} ElfSection;

/* Avoid overflow in offset + size before indexing an untrusted byte array. */
static int ElfFits(uint64_t offset, uint64_t count, uint64_t total)
{
    return offset <= total && count <= total - offset;
}

static uint64_t ElfUnsigned(const unsigned char *bytes, size_t count, int little)
{
    uint64_t value = 0U;
    if (little) {
        for (size_t i = count; i > 0U; --i)
            value = (value << 8U) | bytes[i - 1U];
    } else {
        for (size_t i = 0U; i < count; ++i)
            value = (value << 8U) | bytes[i];
    }
    return value;
}

static uint64_t ElfNumber(const ElfView *view, uint64_t offset, size_t size)
{
    /* All call sites have checked their complete containing entry. */
    return ElfUnsigned(view->bytes + (size_t)offset, size, view->little);
}

static int ElfReadSection(const ElfView *view, uint32_t index, ElfSection *out)
{
    const uint64_t position = view->sectionOffset +
                              (uint64_t)index * view->sectionEntrySize;
    const uint64_t base = view->bits == 64U ? 64U : 40U;
    if (index >= view->sectionCount ||
        !ElfFits(position, base, view->length)) return 0;
    ElfSection section = {0};
    section.type = (uint32_t)ElfNumber(view, position + 4U, 4U);
    if (view->bits == 64U) {
        section.offset = ElfNumber(view, position + 24U, 8U);
        section.length = ElfNumber(view, position + 32U, 8U);
        section.link = (uint32_t)ElfNumber(view, position + 40U, 4U);
        section.entrySize = ElfNumber(view, position + 56U, 8U);
    } else {
        section.offset = ElfNumber(view, position + 16U, 4U);
        section.length = ElfNumber(view, position + 20U, 4U);
        section.link = (uint32_t)ElfNumber(view, position + 24U, 4U);
        section.entrySize = ElfNumber(view, position + 36U, 4U);
    }
    *out = section;
    return 1;
}

static int ElfReadHeader(const unsigned char *bytes, size_t length,
                         ElfView *view, UmiEducationElfReport *report)
{
    if (length < 16U || bytes[0] != 0x7fU || bytes[1] != 'E' ||
        bytes[2] != 'L' || bytes[3] != 'F') return 0;
    unsigned bits = bytes[4] == 1U ? 32U : bytes[4] == 2U ? 64U : 0U;
    if (bits == 0U || (bytes[5] != 1U && bytes[5] != 2U) || bytes[6] != 1U)
        return 0;
    const int little = bytes[5] == 1U;
    const uint64_t headerSize = bits == 64U ? 64U : 52U;
    if (length < headerSize) return 0;
    ElfView local = {.bytes = bytes, .length = length, .bits = bits, .little = little};
    if (ElfNumber(&local, 20U, 4U) != 1U) return 0;
    const uint64_t declaredHeader = ElfNumber(&local, bits == 64U ? 52U : 40U, 2U);
    if (declaredHeader < headerSize || declaredHeader > length) return 0;
    local.sectionOffset = ElfNumber(&local, bits == 64U ? 40U : 32U,
                                    bits == 64U ? 8U : 4U);
    local.sectionEntrySize = (uint32_t)ElfNumber(&local, bits == 64U ? 58U : 46U, 2U);
    uint64_t sections = ElfNumber(&local, bits == 64U ? 60U : 48U, 2U);
    if (local.sectionOffset == 0U || local.sectionEntrySize < (bits == 64U ? 64U : 40U))
        return 0;
    if (sections == 0U) {
        /* Section zero carries an extended section count in sh_size. */
        if (!ElfFits(local.sectionOffset, local.sectionEntrySize, local.length)) return 0;
        sections = ElfNumber(&local, local.sectionOffset + (bits == 64U ? 32U : 20U),
                             bits == 64U ? 8U : 4U);
    }
    if (sections == 0U || sections > EDUCATION_ELF_SECTION_LIMIT ||
        !ElfFits(local.sectionOffset, sections * local.sectionEntrySize, local.length))
        return 0;
    local.sectionCount = (uint32_t)sections;
    report->fileBytes = length;
    report->addressBits = (uint8_t)bits;
    report->littleEndian = (uint8_t)little;
    report->machine = (uint16_t)ElfNumber(&local, 18U, 2U);
    report->elfType = (uint16_t)ElfNumber(&local, 16U, 2U);
    report->sectionCount = (uint32_t)sections;
    *view = local;
    return 1;
}

static int ElfReadName(const ElfView *view, const ElfSection *strings,
                       uint32_t offset, char *name, size_t capacity,
                       int *truncated)
{
    if ((uint64_t)offset >= strings->length) return 0;
    const unsigned char *text = view->bytes + (size_t)strings->offset + offset;
    const size_t left = (size_t)(strings->length - offset);
    const unsigned char *nul = memchr(text, '\0', left);
    if (nul == NULL) return 0;
    const size_t length = (size_t)(nul - text);
    const size_t copied = length < capacity - 1U ? length : capacity - 1U;
    for (size_t i = 0U; i < copied; ++i) {
        const unsigned char value = text[i];
        /* Escape terminal control characters in this human-readable preview.
         * The parser never sends raw binary names to the terminal. */
        name[i] = (value < 0x20U || value == 0x7fU) ? '?' : (char)value;
    }
    name[copied] = '\0';
    *truncated = length >= capacity;
    return 1;
}

static int ElfParseSymbols(const ElfView *view, const ElfSection *section,
                           UmiEducationElfReport *report, uint64_t *recordBudget)
{
    if (section->link >= view->sectionCount) return 0;
    ElfSection strings;
    if (!ElfReadSection(view, section->link, &strings) ||
        strings.type != EDUCATION_ELF_SHT_STRTAB ||
        !ElfFits(strings.offset, strings.length, view->length)) return 0;
    const uint64_t minimum = view->bits == 64U ? 24U : 16U;
    if (section->entrySize < minimum || section->length % section->entrySize != 0U ||
        !ElfFits(section->offset, section->length, view->length)) return 0;
    uint64_t records = section->length / section->entrySize;
    if (records > UMI_EDUCATION_ELF_MAX_RECORDS - *recordBudget) return 0;
    *recordBudget += records;
    for (uint64_t i = 0U; i < records; ++i) {
        /* A symbol table's first entry is the prescribed zero/undefined slot. */
        if (i == 0U) continue;
        const uint64_t start = section->offset + i * section->entrySize;
        const uint32_t nameIndex = (uint32_t)ElfNumber(view, start, 4U);
        const uint8_t information = (uint8_t)ElfNumber(view,
                start + (view->bits == 64U ? 4U : 12U), 1U);
        const uint16_t sectionIndex = (uint16_t)ElfNumber(view,
                start + (view->bits == 64U ? 6U : 14U), 2U);
        const uint64_t value = ElfNumber(view,
                start + (view->bits == 64U ? 8U : 4U), view->bits == 64U ? 8U : 4U);
        const uint64_t size = ElfNumber(view,
                start + (view->bits == 64U ? 16U : 8U), view->bits == 64U ? 8U : 4U);
        char name[UMI_EDUCATION_ELF_SYMBOL_NAME] = {0};
        int truncated = 0;
        if (!ElfReadName(view, &strings, nameIndex, name, sizeof(name), &truncated)) return 0;
        UmiEducationElfSymbol symbol = {0};
        memcpy(symbol.name, name, sizeof(name));
        symbol.value = value;
        symbol.size = size;
        symbol.sectionIndex = sectionIndex;
        symbol.binding = information >> 4U;
        symbol.kind = information & 15U;
        symbol.nameTruncated = truncated;
        report->symbolCount++;
        if (sectionIndex == 0U) report->undefinedCount++;
        else report->definedCount++;
        switch (symbol.binding) {
            case 0U: report->localCount++; break;
            case 1U: report->globalCount++; break;
            case 2U: report->weakCount++; break;
            default: report->otherBindingCount++; break;
        }
        if (symbol.kind == 2U) report->functionCount++;
        if (symbol.kind == 1U) report->objectCount++;
        if (report->displayedCount < UMI_EDUCATION_ELF_DISPLAY_SYMBOLS)
            report->displayed[report->displayedCount++] = symbol;
    }
    return 1;
}

static int ElfParseRelocations(const ElfView *view, const ElfSection *section,
                               UmiEducationElfReport *report, uint64_t *recordBudget)
{
    const uint64_t minimum = view->bits == 64U ?
        (section->type == EDUCATION_ELF_SHT_RELA ? 24U : 16U) :
        (section->type == EDUCATION_ELF_SHT_RELA ? 12U : 8U);
    if (section->entrySize < minimum || section->length % section->entrySize != 0U ||
        !ElfFits(section->offset, section->length, view->length) ||
        section->link >= view->sectionCount) return 0;
    ElfSection symbols;
    if (!ElfReadSection(view, section->link, &symbols) ||
        (symbols.type != EDUCATION_ELF_SHT_SYMTAB &&
         symbols.type != EDUCATION_ELF_SHT_DYNSYM)) return 0;
    const uint64_t records = section->length / section->entrySize;
    if (records > UMI_EDUCATION_ELF_MAX_RECORDS - *recordBudget) return 0;
    *recordBudget += records;
    report->relocationCount += records;
    if (section->type == EDUCATION_ELF_SHT_RELA) report->relaCount += records;
    else report->relCount += records;
    report->relocationSections++;
    return 1;
}

UmiStatus UmiEducationElfInspectBytes(const void *bytes, size_t length,
                                      UmiEducationElfReport *out)
{
    if (out == NULL || (bytes == NULL && length != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_EDUCATION_ELF_FILE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < 4U || memcmp(bytes, "\177ELF", 4U) != 0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiEducationElfReport report = {0};
    ElfView view = {0};
    if (!ElfReadHeader(bytes, length, &view, &report)) return UMI_STATUS_PARSE_ERROR;
    uint64_t recordBudget = 0U;
    for (uint32_t i = 0U; i < view.sectionCount; ++i) {
        ElfSection section;
        if (!ElfReadSection(&view, i, &section)) return UMI_STATUS_PARSE_ERROR;
        if (section.type == EDUCATION_ELF_SHT_SYMTAB ||
            section.type == EDUCATION_ELF_SHT_DYNSYM) {
            if (!ElfParseSymbols(&view, &section, &report, &recordBudget))
                return UMI_STATUS_PARSE_ERROR;
            if (section.type == EDUCATION_ELF_SHT_SYMTAB) report.staticSymbolTables++;
            else report.dynamicSymbolTables++;
        } else if (section.type == EDUCATION_ELF_SHT_REL ||
                   section.type == EDUCATION_ELF_SHT_RELA) {
            if (!ElfParseRelocations(&view, &section, &report, &recordBudget))
                return UMI_STATUS_PARSE_ERROR;
        }
    }
    *out = report; /* No partial publication on an invalid file or table. */
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationElfInspectFile(const char *path, UmiEducationElfReport *out)
{
    if (path == NULL || path[0] == '\0' || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *file = fopen(path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    UmiEducationElfReport pending = {0};
    if (fseek(file, 0, SEEK_END) == 0) {
        long end = ftell(file);
        if (end >= 0L && (uint64_t)end > UMI_EDUCATION_ELF_FILE_LIMIT)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (end >= 0L && fseek(file, 0, SEEK_SET) == 0) {
            const size_t length = (size_t)end;
            unsigned char *bytes = malloc(length != 0U ? length : 1U);
            if (bytes == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                if (fread(bytes, 1U, length, file) == length)
                    status = UmiEducationElfInspectBytes(bytes, length, &pending);
                free(bytes);
            }
        }
    }
    if (fclose(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) *out = pending;
    return status;
}
