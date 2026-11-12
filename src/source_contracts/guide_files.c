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
int UmiSourceContractAuditGuides(const char *const *roots, size_t count,
    const char *asset_directory, UmiSourceContractReport *report)
{
    if (report == NULL || roots == NULL || count == 0U || count > SC_MAX_FILES ||
        asset_directory == NULL || asset_directory[0] == '\0') return 2;
    const char *names[] = {"umicom.ico", "umicom-logo.svg", "umicom-logo-on-dark.svg", "umicom-icon.svg"};
    UmiSourceBrandAsset loaded[4] = {{0}};
    ScFiles files = {0};
    size_t before = report->findings, errors_before = report->input_errors;
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
