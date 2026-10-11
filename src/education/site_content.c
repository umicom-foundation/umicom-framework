/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/site_content.c
 *
 * PURPOSE:
 *   Publish existing educational chapters and official Umicom brand assets
 *   without changing their original sources or silently replacing edits.
 *   Provides a native alternative to tools/prepare_learning_site.py, retained
 *   in the Bits2Banking repository during feature-parity verification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/education/site.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
# include <direct.h>
# include <windows.h>
# include <process.h>
# define EducationMakeDirectory(path) _mkdir(path)
# define EducationProcessId _getpid
#else
# include <unistd.h>
# define EducationMakeDirectory(path) mkdir(path, 0755)
# define EducationProcessId getpid
#endif

static const char *const educationChapters[] = {
    "introduction_to_islam.md", "history_of_money.md",
    "decline_of_sound_money.md", "modern_banking_and_finance.md",
    "technology_and_banking.md", "islamic_finance_and_future.md",
    "umicom_abs_coin.md", "governance_firewalls.md",
    "hall_of_shame_collaboration_and_betrayal.md", "recap.md"
};

static const char *const educationImages[] = {
    "islam_bookcover.png", "Diagram_Timeline_of_Money.png",
    "Diagram_Banks_Create_Money.png", "Diagram_2008_Crash.png",
    "Diagram_Riba_vs_Islamic.png", "Diagram_Tech_in_Banking.png",
    "Diagram_Umicom_ABS_Coin.png", "Diagram_Umicom_Issuance.png",
    "Diagram_Umicom_Circulation.png", "Diagram_Umicom_Redemption.png",
    "Poster_Quran_FairTrade.png", "Poster_Quran_Riba.png",
    "Poster_Hadith_HonestMerchant.png"
};

static const char *const educationBrand[] = {
    "umicom-icon.svg", "umicom-icon-on-dark.svg", "umicom-logo.svg",
    "umicom-logo-on-dark.svg", "umicom-icon.png", "umicom-logo.png", "umicom.ico"
};

static int EducationJoin(char *out, size_t capacity, const char *left, const char *right)
{
    if (out == NULL || left == NULL || right == NULL) return 0;
    int result = snprintf(out, capacity, "%s/%s", left, right);
    return result >= 0 && (size_t)result < capacity;
}

static int EducationFileExists(const char *path)
{
    struct stat info;
    #if defined(_WIN32)
    return stat(path, &info) == 0 && (info.st_mode & _S_IFMT) == _S_IFREG;
#else
    return stat(path, &info) == 0 && S_ISREG(info.st_mode);
#endif
}

static int EducationDirectoryExists(const char *path)
{
    struct stat info;
    #if defined(_WIN32)
    return stat(path, &info) == 0 && (info.st_mode & _S_IFMT) == _S_IFDIR;
#else
    return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
#endif
}

static int EducationCreateParentDirectories(const char *filename)
{
    char path[UMICOM_EDUCATION_PATH_CAPACITY];
    size_t count = strlen(filename);
    if (count >= sizeof(path)) return 0;
    memcpy(path, filename, count + 1U);
    /* Only construct parents from a known target path. Windows drive prefixes
     * are not themselves directories to create. */
    for (size_t i = 1U; i < count; ++i) {
        if (path[i] == '/' || path[i] == '\\') {
            char saved = path[i];
            if (i >= 2U && path[i - 1U] != ':') {
                path[i] = '\0';
                if (!EducationDirectoryExists(path) &&
                    EducationMakeDirectory(path) != 0 && errno != EEXIST)
                    return 0;
                path[i] = saved;
            }
        }
    }
    return 1;
}

static int EducationSameContent(const char *sourcePath, const char *destinationPath,
                                int stripBom)
{
    FILE *source = fopen(sourcePath, "rb");
    FILE *destination = fopen(destinationPath, "rb");
    if (source == NULL || destination == NULL) {
        if (source != NULL) fclose(source);
        if (destination != NULL) fclose(destination);
        return 0;
    }
    if (stripBom) {
        unsigned char signature[3];
        size_t used = fread(signature, 1U, 3U, source);
        if (used != 3U || signature[0] != 0xefU ||
            signature[1] != 0xbbU || signature[2] != 0xbfU)
            (void)fseek(source, 0L, SEEK_SET);
    }
    unsigned char input[8192];
    unsigned char stored[8192];
    int same = 1;
    for (;;) {
        size_t first = fread(input, 1U, sizeof(input), source);
        size_t second = fread(stored, 1U, sizeof(stored), destination);
        if (first != second || memcmp(input, stored, first) != 0) {
            same = 0;
            break;
        }
        if (first != sizeof(input)) {
            if (ferror(source) || ferror(destination)) same = 0;
            break;
        }
    }
    fclose(source);
    fclose(destination);
    return same;
}

/* Return 0 on success, 2 if source absent, 3 if an edited target is preserved,
 * or 1 for an I/O error. Target publication is staged and renamed atomically
 * on the same filesystem, so a short write cannot masquerade as a whole file. */
static int EducationPublishOne(const char *src, const char *dst, int stripBom, int refresh)
{
    if (!EducationFileExists(src)) {
        fprintf(stderr, "MISSING SOURCE: %s\n", src);
        return 2;
    }
    int existing = EducationFileExists(dst);
    if (existing && EducationSameContent(src, dst, stripBom)) {
        printf("unchanged: %s\n", dst);
        return 0;
    }
    if (existing && !refresh) {
        fprintf(stderr, "DIFFERENT (preserved): %s\n", dst);
        return 3;
    }
    if (!EducationCreateParentDirectories(dst)) {
        fprintf(stderr, "Cannot create the destination directory: %s\n", dst);
        return 1;
    }
    char temp[UMICOM_EDUCATION_PATH_CAPACITY];
    int printed = snprintf(temp, sizeof(temp), "%s.umicom-education-%lu.tmp",
                           dst, (unsigned long)EducationProcessId());
    if (printed < 0 || (size_t)printed >= sizeof(temp)) return 1;
    FILE *input = fopen(src, "rb");
    FILE *output = fopen(temp, "wb");
    if (input == NULL || output == NULL) {
        if (input != NULL) fclose(input);
        if (output != NULL) fclose(output);
        (void)remove(temp);
        return 1;
    }
    if (stripBom) {
        unsigned char signature[3];
        size_t size = fread(signature, 1U, sizeof(signature), input);
        if (size != 3U || signature[0] != 0xefU ||
            signature[1] != 0xbbU || signature[2] != 0xbfU)
            (void)fseek(input, 0L, SEEK_SET);
    }
    unsigned char buffer[8192];
    int ioOk = 1;
    for (;;) {
        size_t count = fread(buffer, 1U, sizeof(buffer), input);
        if (count > 0U && fwrite(buffer, 1U, count, output) != count) {
            ioOk = 0;
            break;
        }
        if (count < sizeof(buffer)) {
            if (ferror(input)) ioOk = 0;
            break;
        }
    }
    if (fclose(input) != 0) ioOk = 0;
    if (fclose(output) != 0) ioOk = 0;
    if (!ioOk) {
        (void)remove(temp);
        fprintf(stderr, "Could not copy complete source: %s\n", src);
        return 1;
    }
#if defined(_WIN32)
    if (!MoveFileExA(temp, dst, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        (void)remove(temp);
        fprintf(stderr, "Could not publish destination: %s\n", dst);
        return 1;
    }
#else
    if (rename(temp, dst) != 0) {
        (void)remove(temp);
        fprintf(stderr, "Could not publish destination: %s\n", dst);
        return 1;
    }
#endif
    printf("%s: %s\n", existing ? "updated" : "added", dst);
    return 0;
}

static int EducationProcessGroup(const char *sourceRoot, const char *targetRoot,
                                  const char *const *names, size_t length,
                                  int stripBom, int refresh)
{
    int result = 0;
    for (size_t i = 0U; i < length; ++i) {
        char from[UMICOM_EDUCATION_PATH_CAPACITY];
        char to[UMICOM_EDUCATION_PATH_CAPACITY];
        if (!EducationJoin(from, sizeof(from), sourceRoot, names[i]) ||
            !EducationJoin(to, sizeof(to), targetRoot, names[i])) return 1;
        int code = EducationPublishOne(from, to, stripBom, refresh);
        if (code != 0 && result == 0) result = code;
    }
    return result;
}

int UmicomEducationPrepareSite(const char *repositoryRoot, const char *brandRoot, int refresh)
{
    if (repositoryRoot == NULL || brandRoot == NULL) return 2;
    char source[UMICOM_EDUCATION_PATH_CAPACITY];
    char destination[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!EducationJoin(source, sizeof(source), repositoryRoot, "dropzone/documents/islam_blueprint_for_life") ||
        !EducationJoin(destination, sizeof(destination), repositoryRoot, "content/v001_islam_blueprint_for_life")) return 2;
    int chapters = EducationProcessGroup(source, destination, educationChapters,
        sizeof(educationChapters) / sizeof(educationChapters[0]), 1, refresh);
    if (!EducationJoin(source, sizeof(source), repositoryRoot, "dropzone/images") ||
        !EducationJoin(destination, sizeof(destination), repositoryRoot, "content/assets/books/islam")) return 2;
    int artwork = EducationProcessGroup(source, destination, educationImages,
        sizeof(educationImages) / sizeof(educationImages[0]), 0, refresh);
    if (!EducationJoin(destination, sizeof(destination), repositoryRoot, "content/assets/branding")) return 2;
    int brand = EducationProcessGroup(brandRoot, destination, educationBrand,
        sizeof(educationBrand) / sizeof(educationBrand[0]), 0, refresh);
    if (chapters != 0 || artwork != 0 || brand != 0) {
        fputs("Publication incomplete. Original sources and differing published pages were preserved.\n", stderr);
        return chapters != 0 ? chapters : (artwork != 0 ? artwork : brand);
    }
    puts("Site inputs are ready. Run `build` to generate the HTML website.");
    return 0;
}

static int EducationCheckGroup(const char *root, const char *const *names, size_t length)
{
    int failed = 0;
    for (size_t i = 0U; i < length; ++i) {
        char path[UMICOM_EDUCATION_PATH_CAPACITY];
        if (!EducationJoin(path, sizeof(path), root, names[i])) return 1;
        if (!EducationFileExists(path)) {
            printf("MISSING: %s\n", path);
            failed = 1;
        }
    }
    return failed;
}

int UmicomEducationVerifySite(const char *repositoryRoot)
{
    if (repositoryRoot == NULL) return 2;
    static const char *const essentials[] = {
        "mkdocs.yml", "content/index.md", "content/values.md",
        "content/v001_islam_blueprint_for_life/index.md",
        "content/assets/branding/umicom.ico"
    };
    int failed = EducationCheckGroup(repositoryRoot, essentials,
        sizeof(essentials) / sizeof(essentials[0]));
    char path[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!EducationJoin(path, sizeof(path), repositoryRoot, "content/v001_islam_blueprint_for_life")) return 2;
    failed |= EducationCheckGroup(path, educationChapters,
        sizeof(educationChapters) / sizeof(educationChapters[0]));
    if (!EducationJoin(path, sizeof(path), repositoryRoot, "content/assets/books/islam")) return 2;
    failed |= EducationCheckGroup(path, educationImages,
        sizeof(educationImages) / sizeof(educationImages[0]));
    if (!EducationJoin(path, sizeof(path), repositoryRoot, "content/assets/branding")) return 2;
    failed |= EducationCheckGroup(path, educationBrand,
        sizeof(educationBrand) / sizeof(educationBrand[0]));
    if (!EducationJoin(path, sizeof(path), repositoryRoot, "dropzone/documents/islam_blueprint_for_life")) return 2;
    failed |= EducationCheckGroup(path, educationChapters,
        sizeof(educationChapters) / sizeof(educationChapters[0]));
    puts(failed ? "Website inputs are incomplete." : "Website inputs present: original and published material retained.");
    return failed ? 1 : 0;
}
