/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_links.c
 *
 * PURPOSE:
 *   Reject shell strings, URLs, ambiguous paths and encoded link payloads.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
int TestLinks(const char *name, const char *path)
{
    UmiDeskFederationLink link = {0}, original;
    char output[UMI_DESK_FEDERATION_LINK_CAPACITY];
    (void)path;
    if (strcmp(name, "roundtrip") == 0) {
        const char *const texts[] = {"umicom://app/org.umicom.bank", "umicom://group/finance", "umicom://session/Morning_01"};
        for (size_t index = 0U; index < 3U; ++index) {
            OK(UmiDeskFederationLinkParse(texts[index], &link)); OK(UmiDeskFederationLinkFormat(&link, output, sizeof output)); CHECK(strcmp(output, texts[index]) == 0);
        }
    } else if (strcmp(name, "reject_external") == 0) {
        const char *const texts[] = {"https://example.com", "file:///C:/tmp/app.exe", "cmd /c calc", "powershell.exe", "umicom://app/C:\\evil.exe", "umicom://app/../evil", "umicom://app/-option", "umicom://app/a/b", "umicom://app/a;quit"};
        for (size_t index = 0U; index < sizeof texts / sizeof texts[0]; ++index) CHECK(UmiDeskFederationLinkParse(texts[index], &link) != UMI_STATUS_OK);
    } else if (strcmp(name, "reject_encoding") == 0) {
        const char *const texts[] = {"umicom://app/org%2Eumicom", "umicom://group/finance?run=1", "umicom://app/a#b", "umicom://app/a\n", "umicom://app/a ", " UMICOM://app/a", "umicom://app/a@b", "umicom://app/a:b"};
        for (size_t index = 0U; index < sizeof texts / sizeof texts[0]; ++index) CHECK(UmiDeskFederationLinkParse(texts[index], &link) != UMI_STATUS_OK);
    } else if (strcmp(name, "empty_and_unknown") == 0) {
        CHECK(UmiDeskFederationLinkParse("", &link) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDeskFederationLinkParse("umicom://app/", &link) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDeskFederationLinkParse("umicom://payment/transfer", &link) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDeskFederationLinkParse(NULL, &link) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDeskFederationLinkParse("umicom://app/a", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "length_boundaries") == 0) {
        char text[240];
        strcpy(text, "umicom://app/"); memset(text + 13U, 'a', 159U); text[172] = '\0'; OK(UmiDeskFederationLinkParse(text, &link));
        text[172] = 'a'; text[173] = '\0'; CHECK(UmiDeskFederationLinkParse(text, &link) == UMI_STATUS_INVALID_ARGUMENT);
        strcpy(text, "umicom://group/"); memset(text + 15U, 'a', 63U); text[78] = '\0'; OK(UmiDeskFederationLinkParse(text, &link));
        text[78] = 'a'; text[79] = '\0'; CHECK(UmiDeskFederationLinkParse(text, &link) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "output_unchanged") == 0) {
        memset(&link, 0x5A, sizeof link); original = link;
        CHECK(UmiDeskFederationLinkParse("umicom://app/../bad", &link) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&link, &original, sizeof link) == 0);
        OK(UmiDeskFederationLinkParse("umicom://app/org.umicom.bank", &link)); strcpy(output, "untouched");
        CHECK(UmiDeskFederationLinkFormat(&link, output, 4U) == UMI_STATUS_CAPACITY_EXCEEDED && strcmp(output, "untouched") == 0);
    } else if (strcmp(name, "review_link_inert") == 0) {
        TestFixture *fixture = calloc(1U, sizeof *fixture); UmiDeskFederationPlan plan; UmiDeskWorkspace workspace = TestWorkspace("finance", 2U);
        CHECK(fixture != NULL); OK(TestOpen(fixture, NULL)); OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        OK(UmiDeskFederationPrepareLink(fixture->service, "umicom://group/finance", &plan)); CHECK(plan.count == 2U && fixture->host.requestCount == 0U);
        OK(UmiDeskFederationPrepareLink(fixture->service, "umicom://app/org.umicom.bank", &plan)); CHECK(plan.count == 1U && fixture->host.requestCount == 0U);
        CHECK(UmiDeskFederationPrepareLink(fixture->service, "umicom://session/missing", &plan) == UMI_STATUS_NOT_FOUND);
        TestClose(fixture); free(fixture);
    } else CHECK(false);
    return 0;
}
