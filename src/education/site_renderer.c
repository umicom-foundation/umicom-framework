/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/site_renderer.c
 *
 * PURPOSE:
 *   Build the existing MkDocs educational website using a supervised child
 *   invocation, without changing the Markdown parser or publication semantics.
 *   This native process adapter is transitional; Python and MkDocs remain the
 *   documented publishing dependencies until output parity is proven.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/site.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#if defined(_WIN32)
# include <process.h>
#else
# include <spawn.h>
# include <sys/wait.h>
# include <unistd.h>
extern char **environ;
#endif

int UmicomEducationBuildSite(const char *repositoryRoot)
{
    if (repositoryRoot == NULL || repositoryRoot[0] == '\0') return 2;
    char configuration[UMICOM_EDUCATION_PATH_CAPACITY];
    char destination[UMICOM_EDUCATION_PATH_CAPACITY];
    int n = snprintf(configuration, sizeof(configuration), "%s/mkdocs.yml", repositoryRoot);
    if (n < 0 || (size_t)n >= sizeof(configuration)) return 2;
    n = snprintf(destination, sizeof(destination), "%s/site", repositoryRoot);
    if (n < 0 || (size_t)n >= sizeof(destination)) return 2;
    struct stat metadata;
    if (stat(configuration, &metadata) != 0) {
        fprintf(stderr, "Missing MkDocs configuration: %s\n", configuration);
        return 2;
    }
    /* The argv vector never passes through a shell, and the configured site
     * directory belongs to this repository. Do not execute manuscript text. */
    const char *const arguments[] = {
        "python", "-m", "mkdocs", "build", "--strict", "--config-file",
        configuration, "--site-dir", destination, NULL
    };
    puts("Building the educational website using MkDocs. Python and MkDocs are still required.");
    fflush(stdout);
#if defined(_WIN32)
    intptr_t result = _spawnvp(_P_WAIT, "python", arguments);
    if (result == -1) {
        perror("Cannot launch MkDocs using Python");
        return 1;
    }
    return result == 0 ? 0 : 1;
#else
    pid_t child = 0;
    int status = posix_spawnp(&child, "python", NULL, NULL,
                              (char *const *)arguments, environ);
    if (status != 0) {
        fprintf(stderr, "Cannot launch Python: %s\n", strerror(status));
        return 1;
    }
    int outcome = 0;
    if (waitpid(child, &outcome, 0) < 0) {
        perror("Cannot wait for MkDocs");
        return 1;
    }
    return WIFEXITED(outcome) && WEXITSTATUS(outcome) == 0 ? 0 : 1;
#endif
}
