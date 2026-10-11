/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom-education/main.c
 *
 * PURPOSE:
 *   Provide one explicit C23 command for preparing, checking, building and
 *   serving the Bits2Banking learning website. The tool is standalone during
 *   qualification; a future `umicom education` routing adapter may call the
 *   same Framework-owned services without copying their implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/education/site.h"
#include "umicom/education/publication.h"
#include "umicom/education/nav_audit.h"
#include "umicom/education/binary_inspect.h"
#include "umicom/education/archive_inspect.h"
#include "umicom/education/elf_symbols.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
# include <windows.h>
#else
# include <limits.h>
# include <unistd.h>
#endif

static void EducationPrintHelp(void)
{
    puts("Umicom Education — C23 learning-site tools\n"
         "\n"
         "umicom-education doctor  --repo REPOSITORY [--brand BRAND_DIRECTORY]\n"
         "umicom-education prepare --repo REPOSITORY [--brand BRAND_DIRECTORY] [--refresh]\n"
         "umicom-education verify  --repo REPOSITORY\n"
         "umicom-education audit   --repo REPOSITORY   (native navigation preflight)\n"
         "umicom-education binary --file OBJECT_OR_EXECUTABLE\n"
         "umicom-education archive --file STATIC_LIBRARY\n"
         "umicom-education symbols --file ELF_OBJECT_OR_IMAGE\n"
         "umicom-education build   --repo REPOSITORY [--python PYTHON_EXECUTABLE]\n"
         "umicom-education serve   --repo REPOSITORY [--port 8765] [--directory SITE_DIRECTORY]\n"
         "umicom-education serve   --repo REPOSITORY --build [--python PYTHON_EXECUTABLE] [--port 8765]\n"
         "umicom-education publish manifest --repo REPOSITORY [--refresh]\n"
         "umicom-education publish verify --repo REPOSITORY\n"
         "umicom-education book list --repo REPOSITORY\n"
         "umicom-education book check --repo REPOSITORY --volume SLUG\n"
         "umicom-education book assemble --repo REPOSITORY --volume SLUG [--refresh]\n"
         "\n"
         "The original book manuscripts and Python/PowerShell build tools remain available.\n"
         "The native preview is read-only and binds only to 127.0.0.1.\n"
         "Stop it with Ctrl+C. No public hosting, TLS, or PDF/DOCX generation.\n"
         "\n"
         "For deterministic smoke tests, serve also accepts --once.\n"
         "Build searches .venv first, then a Python installation with MkDocs and Material.\n"
         "Use a repository-local Python virtual environment to avoid MSYS2 PATH conflicts.\n"
         "Serve never builds implicitly. Choose serve --build to build then preview;\n"
         "--build leaves historical book sources untouched and retains MkDocs as renderer.\n");
}

static int EducationIsDirectory(const char *path)
{
    struct stat info;
#if defined(_WIN32)
    return stat(path, &info) == 0 && (info.st_mode & _S_IFMT) == _S_IFDIR;
#else
    return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
#endif
}

static int EducationIsFile(const char *path)
{
    struct stat info;
#if defined(_WIN32)
    return stat(path, &info) == 0 && (info.st_mode & _S_IFMT) == _S_IFREG;
#else
    return stat(path, &info) == 0 && S_ISREG(info.st_mode);
#endif
}

static int EducationPathJoin(char *output, size_t capacity, const char *root, const char *relative)
{
    int count = snprintf(output, capacity, "%s/%s", root, relative);
    return count >= 0 && (size_t)count < capacity;
}

static int EducationAbsolutePath(const char *input, char *output, size_t capacity)
{
    if (input == NULL || output == NULL) return 0;
#if defined(_WIN32)
    DWORD used = GetFullPathNameA(input, (DWORD)capacity, output, NULL);
    return used > 0U && (size_t)used < capacity;
#else
    char *resolved = realpath(input, NULL);
    if (resolved == NULL) return 0;
    size_t length = strlen(resolved);
    if (length + 1U > capacity) {
        free(resolved);
        return 0;
    }
    memcpy(output, resolved, length + 1U);
    free(resolved);
    return 1;
#endif
}

static int EducationParentOf(char *output, size_t capacity, const char *path)
{
    size_t length = strlen(path);
    if (length >= capacity) return 0;
    memcpy(output, path, length + 1U);
    while (length > 0U && output[length - 1U] != '/' && output[length - 1U] != '\\')
        --length;
    if (length == 0U) return 0;
    if (length == 1U) length = 1U;
    else length -= 1U;
    output[length] = '\0';
    return 1;
}

static int EducationFindBrand(const char *repositoryRoot, const char *chosen,
                              char *output, size_t capacity)
{
    if (chosen != NULL) return EducationAbsolutePath(chosen, output, capacity) && EducationIsDirectory(output);
    char parent[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!EducationParentOf(parent, sizeof(parent), repositoryRoot)) return 0;
    static const char *const candidates[] = {
        "Umicom-Applications/framework/resources/brand",
        "umicom-applications/framework/resources/brand",
        "umicomOS/framework/resources/brand"
    };
    for (size_t i = 0U; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
        if (EducationPathJoin(output, capacity, parent, candidates[i]) &&
            EducationIsDirectory(output)) return 1;
    }
    return 0;
}

static int EducationParsePort(const char *text, uint16_t *port)
{
    if (text == NULL || *text == '\0') return 0;
    unsigned value = 0U;
    for (const char *p = text; *p != '\0'; ++p) {
        if (*p < '0' || *p > '9') return 0;
        unsigned digit = (unsigned)(*p - '0');
        if (value > (65535U - digit) / 10U) return 0;
        value = 10U * value + digit;
    }
    if (value < 1024U) return 0;
    *port = (uint16_t)value;
    return 1;
}

static int EducationDoctor(const char *repositoryRoot, const char *brandRoot,
                           const char *selectedPython)
{
    static const char *const necessary[] = {
        "mkdocs.yml", "content/index.md", "dropzone/documents/islam_blueprint_for_life/volume.yml"
    };
    int failed = 0;
    for (size_t i = 0U; i < sizeof(necessary) / sizeof(necessary[0]); ++i) {
        char path[UMICOM_EDUCATION_PATH_CAPACITY];
        if (!EducationPathJoin(path, sizeof(path), repositoryRoot, necessary[i])) return 2;
        int present = EducationIsFile(path);
        printf("[%s] %s\n", present ? "OK" : "MISSING", path);
        if (!present) failed = 1;
    }
    if (brandRoot == NULL) {
        puts("[MISSING] Official Framework brand directory; pass --brand with its path.");
        failed = 1;
    } else {
        printf("[OK] Official Framework brand: %s\n", brandRoot);
    }
    if (UmicomEducationCheckRenderer(repositoryRoot, selectedPython) != 0)
        failed = 1;
    puts("The native C build does not install Python, MkDocs, a C compiler, or a package manager.");
    return failed;
}

int main(int argc, char **argv)
{
    if (argc < 2 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0) {
        EducationPrintHelp();
        return 0;
    }
    /* Read-only native file inspection does not require a Bits2Banking checkout.
     * This independent command never loads executable bytes as a program and
     * cannot accidentally use a build/serve option. The parser is reusable
     * Framework code; the CLI owns only text presentation. */
    if (strcmp(argv[1], "binary") == 0) {
        if (argc != 4 || strcmp(argv[2], "--file") != 0 || argv[3][0] == '\0') {
            fputs("Usage: umicom-education binary --file PATH\n", stderr);
            return 2;
        }
        UmiEducationBinaryHeader header = {0};
        UmiStatus status = UmiEducationBinaryInspectFile(argv[3], &header);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "Cannot inspect binary header (status %d): %s\n",
                    (int)status, argv[3]);
            return 1;
        }
        printf("Format: %s\nType: %s\nArchitecture: %s\n"
               "Address width: %u bits\nEndianness: %s\n"
               "Sections: %u%s\n",
               UmiEducationBinaryFormatText(header.format),
               UmiEducationBinaryTypeText(&header),
               UmiEducationBinaryMachineText(&header),
               (unsigned)header.addressBits,
               header.littleEndian ? "little" : "big",
               (unsigned)header.sections,
               header.sectionCountExtended ? " (extended count)" : "");
        if (header.format != UMI_EDUCATION_BINARY_COFF)
            printf("Entry point (%s): 0x%llx\n",
                   header.entryIsRva ? "RVA" : "address",
                   (unsigned long long)header.entryPoint);
        printf("File size: %llu bytes\n",
               (unsigned long long)header.fileBytes);
        return 0;
    }
    /* A static archive is a container of object-file members, not itself an
     * ELF/PE/COFF image. Keep this separate from binary inspection while using
     * the same read-only native education command and error policy. */
    if (strcmp(argv[1], "archive") == 0) {
        if (argc != 4 || strcmp(argv[2], "--file") != 0 || argv[3][0] == '\0') {
            fputs("Usage: umicom-education archive --file PATH\n", stderr);
            return 2;
        }
        UmiEducationArchiveReport report = {0};
        UmiStatus status = UmiEducationArchiveInspectFile(argv[3], &report);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "Cannot inspect ordinary ar archive (status %d): %s\n",
                    (int)status, argv[3]);
            if (status == UMI_STATUS_NOT_IMPLEMENTED)
                fputs("Thin archives reference other files and are not inspected.\n", stderr);
            return 1;
        }
        printf("Format: ar\nArchive size: %llu bytes\n"
               "Members: %u\nObject members: %u\nIndex/name-table members: %u\n",
               (unsigned long long)report.archiveBytes,
               (unsigned)report.memberCount,
               (unsigned)report.objectCount,
               (unsigned)report.metadataCount);
        for (uint32_t i = 0U; i < report.recordedCount; ++i) {
            const UmiEducationArchiveMember *member = &report.members[i];
            printf("  %s : %llu bytes%s\n", member->name,
                   (unsigned long long)member->bodyBytes,
                   member->objectFile ? " (object)" : "");
        }
        if (report.memberCount > report.recordedCount)
            printf("  ... %u additional members not listed\n",
                   (unsigned)(report.memberCount - report.recordedCount));
        return 0;
    }
    /* ELF symbol and relocation metadata belong to the reusable Framework
     * parser. This read-only educational command is intentionally separate
     * from the executable-header and static-archive commands above; it never
     * asks the operating system to load or execute the inspected file. */
    if (strcmp(argv[1], "symbols") == 0) {
        if (argc != 4 || strcmp(argv[2], "--file") != 0 || argv[3][0] == '\0') {
            fputs("Usage: umicom-education symbols --file ELF_PATH\n", stderr);
            return 2;
        }
        UmiEducationElfReport report = {0};
        const UmiStatus status = UmiEducationElfInspectFile(argv[3], &report);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "Cannot inspect ELF symbols (status %d): %s\n",
                    (int)status, argv[3]);
            if (status == UMI_STATUS_NOT_IMPLEMENTED)
                fputs("This command accepts ELF32/ELF64 only. COFF/PE symbol "
                      "inspection is a separate future feature.\n", stderr);
            return 1;
        }
        printf("Format: ELF%u (%s-endian)\n"
               "Machine: %u\nType: %u\nSections: %u\n"
               "Static symbol tables: %u\nDynamic symbol tables: %u\n"
               "Symbols: %llu\nDefined: %llu\nUndefined: %llu\n"
               "Local: %llu\nGlobal: %llu\nWeak: %llu\nOther binding: %llu\n"
               "Functions: %llu\nObjects: %llu\n"
               "Relocations: %llu (REL %llu, RELA %llu)\n",
               (unsigned)report.addressBits,
               report.littleEndian ? "little" : "big",
               (unsigned)report.machine, (unsigned)report.elfType,
               (unsigned)report.sectionCount, (unsigned)report.staticSymbolTables,
               (unsigned)report.dynamicSymbolTables,
               (unsigned long long)report.symbolCount,
               (unsigned long long)report.definedCount,
               (unsigned long long)report.undefinedCount,
               (unsigned long long)report.localCount,
               (unsigned long long)report.globalCount,
               (unsigned long long)report.weakCount,
               (unsigned long long)report.otherBindingCount,
               (unsigned long long)report.functionCount,
               (unsigned long long)report.objectCount,
               (unsigned long long)report.relocationCount,
               (unsigned long long)report.relCount,
               (unsigned long long)report.relaCount);
        for (uint32_t i = 0U; i < report.displayedCount; ++i) {
            const UmiEducationElfSymbol *symbol = &report.displayed[i];
            printf("  %-34s binding=%u type=%u section=%u%s\n",
                   symbol->name[0] != '\0' ? symbol->name : "(unnamed)",
                   (unsigned)symbol->binding, (unsigned)symbol->kind,
                   (unsigned)symbol->sectionIndex,
                   symbol->nameTruncated ? " (name shortened)" : "");
        }
        if (report.symbolCount > report.displayedCount)
            printf("  ... %llu additional symbol records not displayed\n",
                   (unsigned long long)(report.symbolCount - report.displayedCount));
        return 0;
    }
    const char *command = argv[1];
    const char *bookCommand = NULL;
    int firstOption = 2;
    if (strcmp(command, "publish") == 0) {
        if (argc < 3 || (strcmp(argv[2], "manifest") != 0 &&
                         strcmp(argv[2], "verify") != 0)) {
            fputs("Use `publish manifest` or `publish verify`.\n", stderr);
            return 2;
        }
        bookCommand = argv[2]; /* One bounded subcommand at a time. */
        firstOption = 3;
    }
    if (strcmp(command, "book") == 0) {
        if (argc < 3 || (strcmp(argv[2], "list") != 0 &&
                         strcmp(argv[2], "check") != 0 &&
                         strcmp(argv[2], "assemble") != 0)) {
            fputs("Use `book list`, `book check --volume SLUG`, or `book assemble --volume SLUG`.\n", stderr);
            return 2;
        }
        bookCommand = argv[2];
        firstOption = 3;
    }
    const char *requestedRepository = NULL;
    const char *requestedPython = NULL;
    const char *requestedVolume = NULL;
    const char *requestedBrand = NULL;
    const char *requestedDirectory = NULL;
    uint16_t port = 8765U;
    int refresh = 0;
    int once = 0;
    int buildBeforeServe = 0;
    for (int i = firstOption; i < argc; ++i) {
        if (strcmp(argv[i], "--repo") == 0 && i + 1 < argc) {
            requestedRepository = argv[++i];
        } else if (strcmp(argv[i], "--brand") == 0 && i + 1 < argc) {
            requestedBrand = argv[++i];
        } else if (strcmp(argv[i], "--directory") == 0 && i + 1 < argc) {
            requestedDirectory = argv[++i];
        } else if (strcmp(argv[i], "--python") == 0 && i + 1 < argc) {
            requestedPython = argv[++i];
        } else if (strcmp(argv[i], "--volume") == 0 && i + 1 < argc) {
            requestedVolume = argv[++i];
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            if (!EducationParsePort(argv[++i], &port)) {
                fputs("Port must be between 1024 and 65535.\n", stderr);
                return 2;
            }
        } else if (strcmp(argv[i], "--refresh") == 0) {
            refresh = 1;
        } else if (strcmp(argv[i], "--once") == 0) {
            once = 1;
        } else if (strcmp(argv[i], "--build") == 0) {
            buildBeforeServe = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 2;
        }
    }
    /* A build request is meaningful only for the explicit local preview. The
     * independent `build` command remains the primary publishing operation.
     * Rejecting this early prevents other commands from ignoring --build. */
    if (buildBeforeServe && strcmp(command, "serve") != 0) {
        fputs("--build applies only to serve; use the build command otherwise.\n", stderr);
        return 2;
    }
    if (requestedRepository == NULL) {
        fputs("Specify the Bits2Banking checkout with --repo PATH.\n", stderr);
        return 2;
    }
    char repositoryRoot[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!EducationAbsolutePath(requestedRepository, repositoryRoot, sizeof(repositoryRoot)) ||
        !EducationIsDirectory(repositoryRoot)) {
        fputs("The repository directory does not exist.\n", stderr);
        return 2;
    }
    char brandRoot[UMICOM_EDUCATION_PATH_CAPACITY];
    int brandFound = EducationFindBrand(repositoryRoot, requestedBrand,
                                       brandRoot, sizeof(brandRoot));
    if (strcmp(command, "doctor") == 0) {
        if (requestedVolume != NULL || requestedDirectory != NULL || once || refresh)
            return 2;
        return EducationDoctor(repositoryRoot, brandFound ? brandRoot : NULL,
                               requestedPython);
    }
    if (strcmp(command, "prepare") == 0) {
        if (requestedPython != NULL || requestedVolume != NULL ||
            requestedDirectory != NULL || once) return 2;
        if (!brandFound) {
            fputs("Cannot find Framework branding; use --brand PATH.\n", stderr);
            return 2;
        }
        return UmicomEducationPrepareSite(repositoryRoot, brandRoot, refresh);
    }
    if (strcmp(command, "verify") == 0) {
        if (requestedPython != NULL || requestedVolume != NULL ||
            requestedDirectory != NULL || once || refresh) return 2;
        return UmicomEducationVerifySite(repositoryRoot);
    }
    if (strcmp(command, "build") == 0) {
        if (refresh || requestedDirectory != NULL || once || requestedVolume != NULL) {
            fputs("This option does not apply to build.\n", stderr);
            return 2;
        }
        return UmicomEducationBuildSiteWithPython(repositoryRoot, requestedPython);
    }
    if (strcmp(command, "publish") == 0) {
        if (requestedPython != NULL || requestedVolume != NULL ||
            requestedBrand != NULL || requestedDirectory != NULL || once) {
            fputs("This option does not apply to publication verification.\n", stderr);
            return 2;
        }
        if (strcmp(bookCommand, "manifest") == 0)
            return UmicomEducationPublicationManifest(repositoryRoot, refresh);
        if (refresh) {
            fputs("`publish verify` is read-only; --refresh is not applicable.\n", stderr);
            return 2;
        }
        return UmicomEducationPublicationVerify(repositoryRoot);
    }
    if (strcmp(command, "book") == 0) {
        if (requestedPython != NULL || requestedDirectory != NULL || once ||
            (strcmp(bookCommand, "list") == 0 && (requestedVolume != NULL || refresh)))
            return 2;
        if (strcmp(bookCommand, "list") == 0)
            return UmicomEducationBookList(repositoryRoot);
        if (requestedVolume == NULL) {
            fputs("Specify a volume folder name with --volume SLUG.\n", stderr);
            return 2;
        }
        if (strcmp(bookCommand, "check") == 0) {
            if (refresh) {
                fputs("`book check` does not create output; --refresh is not applicable.\n", stderr);
                return 2;
            }
            return UmicomEducationBookCheck(repositoryRoot, requestedVolume);
        }
        return UmicomEducationBookAssemble(repositoryRoot, requestedVolume, refresh);
    }
    /* The navigation preflight is a Framework-owned, read-only C23 service.
     * It checks paths and duplicates, while MkDocs retains responsibility for
     * full YAML parsing and page rendering. Do not invent a second YAML parser. */
    if (strcmp(command, "audit") == 0) {
        if (requestedPython != NULL || requestedBrand != NULL ||
            requestedDirectory != NULL || requestedVolume != NULL ||
            refresh || once || buildBeforeServe) {
            fputs("Use audit --repo REPOSITORY without build or output options.\n", stderr);
            return 2;
        }
        UmiEducationNavReport report;
        return UmiEducationAuditNavigation(repositoryRoot, &report, stdout);
    }
    if (strcmp(command, "serve") == 0) {
        if (refresh || requestedVolume != NULL ||
            (requestedPython != NULL && !buildBeforeServe) ||
            (requestedDirectory != NULL && buildBeforeServe)) {
            fputs("serve --build uses the repository's configured site folder; "
                  "--directory is for existing exports only.\n", stderr);
            return 2;
        }
        /* The site's renderer is selected through the same Framework service
         * as `umicom-education build`, including support for spaces in Windows
         * executable paths. No second MkDocs launcher is introduced here.
         * The user explicitly opted into building before starting the server. */
        if (buildBeforeServe) {
            if (UmicomEducationBuildSiteWithPython(repositoryRoot, requestedPython) != 0) {
                fputs("Site build failed; the local server was not started.\n", stderr);
                return 1;
            }
        }
        char site[UMICOM_EDUCATION_PATH_CAPACITY];
        if (requestedDirectory == NULL) {
            if (!EducationPathJoin(site, sizeof(site), repositoryRoot, "site")) return 2;
        } else if (!EducationAbsolutePath(requestedDirectory, site, sizeof(site))) {
            fputs("The selected site directory does not exist.\n", stderr);
            return 2;
        }
        char indexPage[UMICOM_EDUCATION_PATH_CAPACITY];
        if (!EducationIsDirectory(site) ||
            !EducationPathJoin(indexPage, sizeof(indexPage), site, "index.html") ||
            !EducationIsFile(indexPage)) {
            fprintf(stderr, "The website home page is missing: %s\n", indexPage);
            fputs("The C web server is ready, but no generated website exists at this path.\n"
                  "Run `umicom-education build --repo REPOSITORY --python PYTHON` first,\n"
                  "or choose `umicom-education serve --repo REPOSITORY --build --python PYTHON`.\n"
                  "The 'doctor' command checks dependencies; it does not build pages.\n", stderr);
            return 2;
        }
        return UmicomEducationServeSite(site, port, once);
    }
    fprintf(stderr, "Unknown Umicom Education command: %s\n", command);
    EducationPrintHelp();
    return 2;
}
