/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/guide_files.c
 *
 * PURPOSE:
 *   Compare discovered standalone guides with one explicit approved asset directory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "internal.h"
#include <stdlib.h>
#include <string.h>

static int GuidePathOrder(const void *a, const void *b)
{ return strcmp(*(const char *const *)a, *(const char *const *)b); }

/* Both layouts use these same reference bytes for their asset comparisons. */
static const char *const GuideAssetNames[] = {
    "umicom.ico", "umicom-logo.svg", "umicom-logo-on-dark.svg", "umicom-icon.svg"
};

static int GuideLoadCanonical(const char *directory, UmiSourceBrandAsset loaded[4],
    UmiSourceContractReport *report)
{
    int result = 0;
    for (size_t i = 0U; i < 4U; ++i) {
        char *path = ScJoin(directory, GuideAssetNames[i]);
        if (path != NULL) loaded[i].bytes = ScReadBytes(path, &loaded[i].length);
        if (loaded[i].bytes == NULL || loaded[i].length == 0U || loaded[i].length > 2U * 1024U * 1024U) {
            ScFinding(report, path != NULL ? path : directory, 1U,
                "guide.asset.source", "Missing, unreadable or oversized canonical asset", true);
            result = 2;
        }
        free(path);
    }
    return result;
}

int UmiSourceContractAuditGuides(const char *const *roots, size_t count,
    const char *asset_directory, UmiSourceContractReport *report)
{
    if (report == NULL || roots == NULL || count == 0U || count > SC_MAX_FILES ||
        asset_directory == NULL || asset_directory[0] == '\0') return 2;
    UmiSourceBrandAsset loaded[4] = {{0}};
    ScFiles files = {0};
    size_t before = report->findings, errors_before = report->input_errors;
    int result = GuideLoadCanonical(asset_directory, loaded, report);
    /* The earlier inline asset loader fed this audit alone. The shared loader
     * now applies the same names and extent limits to bundles, too. */
#if 0
    const char *names[] = {"umicom.ico", "umicom-logo.svg", "umicom-logo-on-dark.svg", "umicom-icon.svg"};
    int result = 0;
    for (size_t i = 0U; i < 4U; ++i) {
        char *path = ScJoin(asset_directory, names[i]);
        if (path != NULL) loaded[i].bytes = ScReadBytes(path, &loaded[i].length);
        if (loaded[i].bytes == NULL || loaded[i].length == 0U || loaded[i].length > 2U * 1024U * 1024U) {
            ScFinding(report, path != NULL ? path : asset_directory, 1U,
                "guide.asset.source", "Missing, unreadable or oversized canonical asset", true);
            result = 2;
        }
        free(path);
    }
#endif
    if (result != 0) goto finish;
    for (size_t i = 0U; i < count; ++i) {
        if (roots[i] == NULL || ScCollectGuides(roots[i], &files) != 0) {
            ScFinding(report, roots[i] != NULL ? roots[i] : "", 1U,
                "guide.input.directory", "Missing, unreadable or unsupported guide directory", true);
            result = 2;
        }
    }
    if (files.count == 0U) {
        ScFinding(report, roots[0] != NULL ? roots[0] : "", 1U,
            "guide.input.empty", "No standalone HTML guides were found", true);
        result = 2; goto finish;
    }
    qsort(files.items, files.count, sizeof(*files.items), GuidePathOrder);
    UmiSourceBrandAssets assets = {loaded[0], loaded[1], loaded[2], loaded[3]};
    for (size_t i = 0U; i < files.count; ++i) {
        if (i != 0U && strcmp(files.items[i - 1U], files.items[i]) == 0) continue;
        char *html = ScRead(files.items[i]);
        if (html == NULL) {
            ScFinding(report, files.items[i], 1U, "guide.input.read", "Cannot read a bounded text guide", true);
            result = 2; continue;
        }
        int current = UmiSourceContractGuideBranding(html, strlen(html), files.items[i], &assets, report);
        if (current > result) result = current;
        free(html);
    }
finish:
    for (size_t i = 0U; i < 4U; ++i) free((void *)loaded[i].bytes);
    ScFilesFree(&files);
    if (report->input_errors != errors_before) return 2;
    if (report->findings != before && result == 0) return 1;
    return result;
}

/* Read only managed package paths, never an href or src from an input page. */
static bool GuideBundledAsset(const char *directory, const char *name, UmiSourceBrandAsset reference)
{
    char *path = ScJoin(directory, name);
    size_t length = 0U;
    unsigned char *bytes = NULL;
    if (path != NULL && ScGuideOrdinaryPath(path, false)) bytes = ScReadBytes(path, &length);
    bool same = bytes != NULL && length == reference.length && memcmp(bytes, reference.bytes, length) == 0;
    free(bytes);
    free(path);
    return same;
}

static int GuideBundledTree(const char *root, const UmiSourceBrandAsset loaded[4],
    UmiSourceContractReport *report)
{
    char *resolved = NULL;
    ScFiles files = {0};
    if (ScCollectBundledGuides(root, &files, &resolved) != 0 || files.count == 0U) {
        ScFinding(report, root != NULL ? root : "", 1U, "guide.bundle.directory",
            "Select a readable package directory containing HTML guides", true);
        free(resolved);
        ScFilesFree(&files);
        return 2;
    }

    /* A tree may also contain self-contained guides. Record missing package
     * assets as unavailable and let only pages that use them report findings. */
    UmiSourceBrandAssets assets = {loaded[0], loaded[1], loaded[2], loaded[3]};
    ScGuideBundle bundle = {0};
    char *directory = ScJoin(resolved, "assets");
    char *css = NULL;
    if (directory != NULL && ScGuideOrdinaryPath(directory, true)) {
        bundle.ico = GuideBundledAsset(directory, GuideAssetNames[0], loaded[0]);
        bundle.logo_light = GuideBundledAsset(directory, GuideAssetNames[1], loaded[1]);
        bundle.logo_dark = GuideBundledAsset(directory, GuideAssetNames[2], loaded[2]);
        bundle.symbol = GuideBundledAsset(directory, GuideAssetNames[3], loaded[3]);
        char *stylesheet = ScJoin(directory, "umicom-documentation.css");
        if (stylesheet != NULL && ScGuideOrdinaryPath(stylesheet, false)) css = ScRead(stylesheet);
        free(stylesheet);
    }
    bundle.css = css;
    bundle.css_length = css != NULL ? strlen(css) : 0U;
    free(directory);

    qsort(files.items, files.count, sizeof(*files.items), GuidePathOrder);
    int result = 0;
    size_t prefix = strlen(resolved);
    for (size_t i = 0U; i < files.count; ++i) {
        const char *path = files.items[i];
        char *html = ScRead(path);
        if (html == NULL) {
            ScFinding(report, path, 1U, "guide.input.read", "Cannot read a bounded text guide", true);
            result = 2;
            continue;
        }
        /* The collector appends child names with '/' on both native hosts. */
        size_t depth = 0U;
        for (const char *relative = path + prefix + 1U; *relative != '\0'; ++relative)
            if (*relative == '/') ++depth;
        int current = ScGuideBundledAudit(html, strlen(html), path, &assets, &bundle, depth, report);
        if (current > result) result = current;
        free(html);
    }
    free(css);
    free(resolved);
    ScFilesFree(&files);
    return result;
}

int UmiSourceContractAuditBundledGuides(const char *const *roots, size_t count,
    const char *asset_directory, UmiSourceContractReport *report)
{
    if (report == NULL || roots == NULL || count == 0U || count > SC_MAX_FILES ||
        asset_directory == NULL || asset_directory[0] == '\0') return 2;
    UmiSourceBrandAsset loaded[4] = {{0}};
    int result = GuideLoadCanonical(asset_directory, loaded, report);
    if (result == 0) {
        for (size_t i = 0U; i < count; ++i) {
            int current = GuideBundledTree(roots[i], loaded, report);
            if (current > result) result = current;
        }
    }
    for (size_t i = 0U; i < 4U; ++i) free((void *)loaded[i].bytes);
    return result;
}
