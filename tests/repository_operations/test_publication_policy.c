/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_policy.c
 *
 * PURPOSE:
 *   Verify private-path exclusions, remote identities and caller boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    (void)fprintf(stderr, "Check failed at line %d: %s\n", __LINE__, #condition); \
    return EXIT_FAILURE; } } while (0)

int main(void)
{
    const char *blocked[] = {
        "UPDATE_private.md", "docs/update_notes.txt", "Batch_91.md",
        "content/BATCH_chat.txt", "_local/audit.json", ".codex/config.toml",
        "notes/private/message.md", ".env", ".env.production", "AGENTS.md",
        "ROADMAP_MAJOR_UPDATES.md", "UMICOM_EDUCATION_NATIVE_BATCH.md",
        "SITE_MIGRATION.md", "build/bin/app.exe", "Debug/kernel.elf",
        "cmake-build-debug/cache", "docs/adr/0001-framework-first-healthcare.md",
        "../outside.c", "src/\nprivate.c", "/outside.c"
    };
    const char *allowed[] = {
        "src/repository/publication.c", "src/debug/session.c",
        "docs/guides/repository-publication.html", "tests/build/test_plan.c",
        "src/batch_queue.c", "docs/update-guide.md", ".env.example",
        ".env.sample", "assets/icon.png", "docs/architecture.md"
    };
    char https[1024], ssh[1024];
    UmiRepositoryPublicationRequest request;
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof(*report));
    CHECK(report != NULL);
    for (size_t i = 0U; i < sizeof(blocked) / sizeof(blocked[0]); ++i)
        CHECK(umi_repository_publication_private_path(blocked[i]));
    for (size_t i = 0U; i < sizeof(allowed) / sizeof(allowed[0]); ++i)
        CHECK(!umi_repository_publication_private_path(allowed[i]));
    CHECK(umi_publication_origin_identity("https://GitHub.com/team/app.git",
        https, sizeof(https)) == UMI_STATUS_OK);
    CHECK(umi_publication_origin_identity("git@github.com:team/app.git",
        ssh, sizeof(ssh)) == UMI_STATUS_OK);
    CHECK(strcmp(https, ssh) == 0);
    CHECK(umi_publication_origin_identity("https://secret@github.com/team/app",
        https, sizeof(https)) != UMI_STATUS_OK);
    CHECK(umi_publication_origin_identity("https://github.com/team/app?token=secret",
        https, sizeof(https)) != UMI_STATUS_OK);
    CHECK(umi_publication_origin_identity("https://host/team/app\nhttps://host/other",
        https, sizeof(https)) != UMI_STATUS_OK);
    CHECK(umi_publication_origin_identity("ext::untrusted", https,
        sizeof(https)) != UMI_STATUS_OK);
    CHECK(umi_publication_origin_identity("https://host/app", https, 2U)
        == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_repository_publication_request_init(&request, ".");
    CHECK(!request.apply && request.refresh && request.timeout_ms == 300000U);
    report->count = 41U;
    CHECK(umi_repository_publication_run_sized(NULL, NULL, &request, report,
        sizeof(*report) - 1U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(report->count == 41U); /* A mismatched caller buffer is not written. */
    request.apply = 1;
    request.refresh = 0;
    CHECK(umi_publication_run_backend(NULL, NULL, &request, report, NULL)
        == UMI_STATUS_INVALID_ARGUMENT);
    free(report);
    return EXIT_SUCCESS;
}
