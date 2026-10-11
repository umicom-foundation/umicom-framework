/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_site_publish.c
 * PURPOSE: Prove native asset publication preserves originals and refused edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/site.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
# include <direct.h>
# define MakeDirectory(path) _mkdir(path)
#else
# include <unistd.h>
# define MakeDirectory(path) mkdir(path, 0755)
#endif

static const char *const chapterNames[] = {
    "introduction_to_islam.md", "history_of_money.md",
    "decline_of_sound_money.md", "modern_banking_and_finance.md",
    "technology_and_banking.md", "islamic_finance_and_future.md",
    "umicom_abs_coin.md", "governance_firewalls.md",
    "hall_of_shame_collaboration_and_betrayal.md", "recap.md"
};
static const char *const pictureNames[] = {
    "islam_bookcover.png", "Diagram_Timeline_of_Money.png", "Diagram_Banks_Create_Money.png",
    "Diagram_2008_Crash.png", "Diagram_Riba_vs_Islamic.png", "Diagram_Tech_in_Banking.png",
    "Diagram_Umicom_ABS_Coin.png", "Diagram_Umicom_Issuance.png", "Diagram_Umicom_Circulation.png",
    "Diagram_Umicom_Redemption.png", "Poster_Quran_FairTrade.png", "Poster_Quran_Riba.png",
    "Poster_Hadith_HonestMerchant.png"
};
static const char *const brandNames[] = {
    "umicom-icon.svg", "umicom-icon-on-dark.svg", "umicom-logo.svg",
    "umicom-logo-on-dark.svg", "umicom-icon.png", "umicom-logo.png", "umicom.ico"
};

static int CreateTree(const char *path)
{
    char temp[UMICOM_EDUCATION_PATH_CAPACITY];
    size_t length = strlen(path);
    if (length >= sizeof(temp)) return 0;
    memcpy(temp, path, length + 1U);
    for (size_t i = 1U; i < length; ++i) {
        if (temp[i] == '/' || temp[i] == '\\') {
            char c = temp[i];
            temp[i] = '\0';
            if (MakeDirectory(temp) != 0 && errno != EEXIST) return 0;
            temp[i] = c;
        }
    }
    return MakeDirectory(temp) == 0 || errno == EEXIST;
}

static void WriteFile(const char *root, const char *name, const char *text)
{
    char path[UMICOM_EDUCATION_PATH_CAPACITY];
    assert(snprintf(path, sizeof(path), "%s/%s", root, name) > 0);
    FILE *file = fopen(path, "wb");
    assert(file != NULL);
    size_t len = strlen(text);
    assert(fwrite(text, 1U, len, file) == len);
    assert(fclose(file) == 0);
}

static void Populate(const char *root, const char *const *names, size_t count, const char *text)
{
    for (size_t i = 0U; i < count; ++i)
        WriteFile(root, names[i], text);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    char repo[UMICOM_EDUCATION_PATH_CAPACITY];
    char bookSrc[UMICOM_EDUCATION_PATH_CAPACITY];
    char imageSrc[UMICOM_EDUCATION_PATH_CAPACITY];
    char brand[UMICOM_EDUCATION_PATH_CAPACITY];
    char content[UMICOM_EDUCATION_PATH_CAPACITY];
    assert(snprintf(repo, sizeof(repo), "%s/repository", argv[1]) > 0);
    assert(snprintf(bookSrc, sizeof(bookSrc), "%s/dropzone/documents/islam_blueprint_for_life", repo) > 0);
    assert(snprintf(imageSrc, sizeof(imageSrc), "%s/dropzone/images", repo) > 0);
    assert(snprintf(brand, sizeof(brand), "%s/brand", argv[1]) > 0);
    assert(snprintf(content, sizeof(content), "%s/content/v001_islam_blueprint_for_life", repo) > 0);
    assert(CreateTree(bookSrc));
    assert(CreateTree(imageSrc));
    assert(CreateTree(brand));
    assert(CreateTree(content));
    Populate(bookSrc, chapterNames, sizeof(chapterNames)/sizeof(chapterNames[0]), "\xef\xbb\xbf# Original volume\n");
    Populate(imageSrc, pictureNames, sizeof(pictureNames)/sizeof(pictureNames[0]), "fake image fixture");
    Populate(brand, brandNames, sizeof(brandNames)/sizeof(brandNames[0]), "fake brand fixture");
    WriteFile(repo, "mkdocs.yml", "site_name: Umicom\n");
    assert(CreateTree(content));
    WriteFile(content, "index.md", "# Islam: Blueprint for Life\n");
    char siteContent[UMICOM_EDUCATION_PATH_CAPACITY];
    assert(snprintf(siteContent, sizeof(siteContent), "%s/content", repo) > 0);
    WriteFile(siteContent, "index.md", "# Bits to Banking\n");
    WriteFile(siteContent, "values.md", "# Values\n");

    /* Reconcile only the disposable test fixture, which may have survived a
     * previous CTest run; never apply this rule to a user's repository. */
    assert(UmicomEducationPrepareSite(repo, brand, 1) == 0);
    assert(UmicomEducationPrepareSite(repo, brand, 0) == 0);
    assert(UmicomEducationVerifySite(repo) == 0);

    /* Edit only the original manuscript. A second prepare without permission
     * must not overwrite the published teaching page. */
    WriteFile(bookSrc, chapterNames[0], "# Manuscript revision\n");
    assert(UmicomEducationPrepareSite(repo, brand, 0) == 3);
    assert(UmicomEducationPrepareSite(repo, brand, 1) == 0);
    assert(UmicomEducationVerifySite(repo) == 0);
    puts("Native publication and explicit refresh checks passed.");
    return 0;
}
