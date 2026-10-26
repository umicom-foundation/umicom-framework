/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_coverage_manifest.c
 *
 * PURPOSE:
 *   Reject ambiguous coverage plans while preserving exact host and test names.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "umicom/testing/ctest_coverage_manifest.h"
#include "umicom/base/csv_document.h"
#define HEADER "kind,host,configuration,source_identity,test,capability,report\n"
#define HOST "host,windows,Debug,source-A,,,results.xml\n"
#define REQUIRE "require,windows,Debug,,z.pass,GTK display,\n"

static int Large(bool hosts)
{
    size_t count = hosts ? UMI_CTEST_COVERAGE_MAX_HOSTS + 1U : 4096U;
    size_t capacity = count * 96U + sizeof HEADER;
    char *text = malloc(capacity);
    CHECK(text);
    size_t used = (size_t)snprintf(text, capacity, "%s", HEADER);
    for (size_t i = 0; i < count; ++i) {
        int length = hosts
            ? snprintf(text + used, capacity - used, "host,host-%zu,Debug,source-A,,,results.xml\n", i)
            : snprintf(text + used, capacity - used, "require,windows,Debug,,row-%zu,,\n", i);
        CHECK(length > 0 && (size_t)length < capacity - used);
        used += (size_t)length;
    }
    UmiCtestCoverageManifest *manifest = NULL;
    UmiStatus status = UmiCtestCoverageManifestImport(text, used, &manifest);
    if (hosts) CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && !manifest);
    else {
        OK(status);
        CHECK(UmiCtestCoverageManifestRequirementCount(manifest) == count);
        CHECK(!strcmp(UmiCtestCoverageManifestRequirementAt(manifest, count - 1)->name, "row-4095"));
    }
    UmiCtestCoverageManifestDestroy(manifest);
    free(text);
    return 0;
}
int main(int argc, char **argv)
{
    const char *names[] = {"valid", "quoted", "bom", "crlf", "ownership", "empty", "header-only",
        "columns", "unknown", "duplicate-host", "duplicate-requirement", "unused-field", "blank",
        "absolute", "drive", "parent", "backslash-parent", "dot", "double-separator", "trailing-separator",
        "backslash", "control", "invalid-utf8", "unterminated", "capacity", "null", "large", "host-limit"};
    CHECK(argc == 2 && Known(argv[1], names, sizeof names / sizeof names[0]));
    const char *name = argv[1];
    if (!strcmp(name, "large") || !strcmp(name, "host-limit")) return Large(!strcmp(name, "host-limit"));
    const char *text = HEADER HOST REQUIRE;
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(name, "quoted")) text = HEADER
        "host,\"Linux, desktop\",Debug,source-A,,,results.xml\n"
        "require,\"Linux, desktop\",Debug,,\"caf\xc3\xa9, test\",\"GTK, display\",\n";
    if (!strcmp(name, "bom")) text = "\xef\xbb\xbf" HEADER HOST REQUIRE;
    if (!strcmp(name, "crlf")) text =
        "kind,host,configuration,source_identity,test,capability,report\r\n"
        "host,windows,Debug,source-A,,,results.xml\r\nrequire,windows,Debug,,z.pass,,\r\n";
    if (!strcmp(name, "empty")) { text = ""; expected = UMI_STATUS_PARSE_ERROR; }
    if (!strcmp(name, "header-only")) text = HEADER;
    if (!strcmp(name, "columns")) { text = HEADER "host,windows,Debug,source-A,,,results.xml,extra\n"; expected = UMI_STATUS_PARSE_ERROR; }
    if (!strcmp(name, "unknown")) { text = HEADER "execute,windows,Debug,source-A,,,results.xml\n"; expected = UMI_STATUS_PARSE_ERROR; }
    if (!strcmp(name, "duplicate-host")) { text = HEADER HOST HOST REQUIRE; expected = UMI_STATUS_ALREADY_EXISTS; }
    if (!strcmp(name, "duplicate-requirement")) { text = HEADER HOST REQUIRE REQUIRE; expected = UMI_STATUS_ALREADY_EXISTS; }
    if (!strcmp(name, "unused-field")) { text = HEADER "require,windows,Debug,source-A,z.pass,,\n"; expected = UMI_STATUS_PARSE_ERROR; }
    if (!strcmp(name, "blank")) { text = HEADER "\n"; expected = UMI_STATUS_PARSE_ERROR; }
    if (!strcmp(name, "absolute")) { text = HEADER "host,windows,Debug,source-A,,,/report.xml\n"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "drive")) { text = HEADER "host,windows,Debug,source-A,,,C:report.xml\n"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "parent")) { text = HEADER "host,windows,Debug,source-A,,,sub/../report.xml\n"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "backslash-parent")) { text = HEADER "host,windows,Debug,source-A,,,sub\\..\\report.xml\n"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "dot")) { text = HEADER "host,windows,Debug,source-A,,,./report.xml\n"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "double-separator")) { text = HEADER "host,windows,Debug,source-A,,,sub//report.xml\n"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "trailing-separator")) { text = HEADER "host,windows,Debug,source-A,,,report.xml/\n"; expected = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "backslash")) text = HEADER "host,windows,Debug,source-A,,,sub\\report.xml\n" REQUIRE;
    if (!strcmp(name, "control")) { text = HEADER "require,windows,Debug,,z.pass,\"two\nlines\",\n"; expected = UMI_STATUS_PARSE_ERROR; }
    if (!strcmp(name, "invalid-utf8")) { text = HEADER "require,windows,Debug,,\x80,,\n"; expected = UMI_STATUS_PARSE_ERROR; }
    if (!strcmp(name, "unterminated")) { text = HEADER "require,\"unterminated"; expected = UMI_STATUS_PARSE_ERROR; }
    UmiCtestCoverageManifest *manifest = NULL;
    if (!strcmp(name, "null")) {
        CHECK(UmiCtestCoverageManifestImport(text, strlen(text), NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageManifestImport(NULL, 1, &manifest) == UMI_STATUS_INVALID_ARGUMENT && !manifest);
        CHECK(UmiCtestCoverageManifestHostCount(NULL) == 0 && UmiCtestCoverageManifestRequirementCount(NULL) == 0);
        CHECK(UmiCtestCoverageManifestHostAt(NULL, 0) == NULL && UmiCtestCoverageManifestRequirementAt(NULL, 0) == NULL);
        UmiCtestCoverageManifestDestroy(NULL);
        return 0;
    }
    if (!strcmp(name, "capacity")) {
        CHECK(UmiCtestCoverageManifestImport(text, UMI_CSV_MAX_BYTES + 1U, &manifest) ==
            UMI_STATUS_CAPACITY_EXCEEDED && !manifest);
        return 0;
    }
    size_t length = strlen(text);
    char *copy = malloc(length + 1U);
    CHECK(copy); memcpy(copy, text, length + 1U);
    UmiStatus status = UmiCtestCoverageManifestImport(copy, length, &manifest);
    memset(copy, 'x', length); free(copy);
    CHECK(status == expected);
    if (status != UMI_STATUS_OK) { CHECK(!manifest); return 0; }
    size_t count = !strcmp(name, "header-only") ? 0 : 1;
    CHECK(UmiCtestCoverageManifestHostCount(manifest) == count);
    CHECK(UmiCtestCoverageManifestRequirementCount(manifest) == count);
    CHECK(UmiCtestCoverageManifestHostAt(manifest, count) == NULL);
    CHECK(UmiCtestCoverageManifestRequirementAt(manifest, count) == NULL);
    if (count) {
        const UmiCtestCoverageHostFile *host = UmiCtestCoverageManifestHostAt(manifest, 0);
        const UmiCtestCoverageRequirement *requirement = UmiCtestCoverageManifestRequirementAt(manifest, 0);
        CHECK(!strcmp(host->source_identity, "source-A"));
        CHECK(!strcmp(requirement->name, !strcmp(name, "quoted") ? "caf\xc3\xa9, test" : "z.pass"));
        CHECK(!strcmp(host->host, requirement->host));
    }
    UmiCtestCoverageManifestDestroy(manifest);
    return 0;
}
