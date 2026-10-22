/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_execution/test_link_bounds.c
 * PURPOSE: Check coordinate overflow and public navigation buffer boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/navigation.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%d: %s\n", __LINE__, #value); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiTerminalLinkResult links;
    UmiTerminalLink link = {0};
    UmiUiNavigationEntrySnapshot navigation, previous;
    if (strcmp(argv[1], "coordinates") == 0) {
        const struct { const char *text, *path; uint32_t line, column; } samples[] = {
            {"a:7:3", "a", 7U, 3U},
            {"a:7", "a", 7U, 0U},
            {"src/main.c:42:7:", "src/main.c", 42U, 7U},
            {"C:/src/main.c:42:7", "C:/src/main.c", 42U, 7U},
            {"C:/src/main.c:42", "C:/src/main.c", 42U, 0U},
            {"src/main.c:4294967295:4294967295", "src/main.c", UINT32_MAX, UINT32_MAX},
            {"src/main.c:00042:00007", "src/main.c", 42U, 7U}
        };
        for (size_t index = 0U; index < sizeof samples / sizeof samples[0]; ++index) {
            CHECK(umi_terminal_links_detect(samples[index].text, &links) == UMI_STATUS_OK);
            CHECK(links.count == 1U && links.links[0].kind == UMI_TERMINAL_LINK_COMPILER_LOCATION);
            CHECK(strcmp(links.links[0].target, samples[index].path) == 0);
            CHECK(links.links[0].line == samples[index].line && links.links[0].column == samples[index].column);
        }
    } else if (strcmp(argv[1], "overflow") == 0 || strcmp(argv[1], "zero") == 0) {
        const char *values[] = {"src/main.c:4294967296", "src/main.c:4294967296:2",
            "src/main.c:2:4294967296", "src/main.c:18446744073709551615",
            "src/main.c:999999999999999999999999999999999999999999",
            "src/main.c:0", "src/main.c:0:2", "src/main.c:2:0"};
        size_t begin = strcmp(argv[1], "zero") == 0 ? 5U : 0U;
        size_t end = strcmp(argv[1], "zero") == 0 ? 8U : 5U;
        for (size_t index = begin; index < end; ++index) {
            CHECK(umi_terminal_links_detect(values[index], &links) == UMI_STATUS_OK);
            for (size_t item = 0U; item < links.count; ++item)
                CHECK(links.links[item].kind != UMI_TERMINAL_LINK_COMPILER_LOCATION);
        }
    } else if (strcmp(argv[1], "unterminated") == 0 || strcmp(argv[1], "uri-capacity") == 0) {
        memset(&navigation, 0xa5, sizeof navigation);
        memcpy(&previous, &navigation, sizeof previous);
        memset(link.target, 'x', sizeof link.target);
        if (strcmp(argv[1], "uri-capacity") == 0) link.target[sizeof navigation.uri] = '\0';
        CHECK(umi_terminal_ui_link_navigation(&link, &navigation) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&navigation, &previous, sizeof navigation) == 0);
    } else if (strcmp(argv[1], "utf8-label") == 0) {
        size_t edge = sizeof navigation.label - 2U;
        memset(link.target, 'x', edge);
        memcpy(link.target + edge, "\xc3\xa9.c", 5U);
        link.kind = UMI_TERMINAL_LINK_FILE;
        CHECK(umi_terminal_ui_link_navigation(&link, &navigation) == UMI_STATUS_OK);
        CHECK(strlen(navigation.label) == edge);
        CHECK(strcmp(navigation.uri, link.target) == 0);
    } else if (strcmp(argv[1], "arguments") == 0) {
        CHECK(umi_terminal_ui_link_navigation(NULL, &navigation) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_terminal_ui_link_navigation(&link, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_terminal_ui_link_navigation(&link, &navigation) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
