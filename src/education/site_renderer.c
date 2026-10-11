/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/site_renderer.c
 *
 * PURPOSE:
 *   Select a Python installation that actually contains MkDocs and Material,
 *   then run the existing website renderer as an explicitly supervised child.
 *   The C23 native services remain usable without Python; only the MkDocs
 *   website renderer requires this external dependency.
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

/* The original single-interpreter implementation is retained for comparison:
 * it selected the first `python` on PATH. On MSYS2 UCRT64 this normally selects
 * the toolchain Python, even when MkDocs is installed into a separate Windows
 * Python or a repository virtual environment. A failed build consequently
 * reported `No module named mkdocs` and left the website unavailable. The new
 * implementation probes the complete renderer before choosing an interpreter.
 */
#if 0
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
#endif

typedef struct EducationPython {
    char executable[UMICOM_EDUCATION_PATH_CAPACITY];
    int launcherArgument; /* `py -3` selects the Windows Python launcher. */
} EducationPython;

static int EducationJoin(char *buffer, size_t capacity, const char *root, const char *name)
{
    int used = snprintf(buffer, capacity, "%s/%s", root, name);
    return used >= 0 && (size_t)used < capacity;
}

static int EducationFileExists(const char *path)
{
    struct stat st;
    #if defined(_WIN32)
    return path != NULL && stat(path, &st) == 0 && (st.st_mode & _S_IFMT) == _S_IFREG;
#else
    return path != NULL && stat(path, &st) == 0 && S_ISREG(st.st_mode);
#endif
}

/* Return the process exit code, or -1 when a child could not be started/waited.
 * Never interpret a user-supplied interpreter path as shell syntax. */
static int EducationSpawn(const char *executable, const char *const arguments[])
{
#if defined(_WIN32)
    intptr_t outcome = _spawnvp(_P_WAIT, executable, arguments);
    return outcome < 0 ? -1 : (int)outcome;
#else
    pid_t child = 0;
    int spawnResult = posix_spawnp(&child, executable, NULL, NULL,
                                  (char *const *)arguments, environ);
    if (spawnResult != 0) return -1;
    int status = 0;
    while (waitpid(child, &status, 0) < 0) {
        if (errno == EINTR) continue;
        return -1;
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif
}

static int EducationRun(const EducationPython *python, int probe,
                        const char *configFile, const char *siteDirectory)
{
    const char *arguments[15];
    size_t used = 0U;
    arguments[used++] = python->executable;
    if (python->launcherArgument) arguments[used++] = "-3";
    if (probe) {
        /* This check is deliberately quiet. Asking only `python --version`
         * would falsely accept Python installations without the renderer. */
        arguments[used++] = "-c";
        arguments[used++] =
            "import importlib.util,sys;sys.exit(0 if "
            "importlib.util.find_spec('mkdocs') is not None and "
            "importlib.util.find_spec('material') is not None else 1)";
    } else {
        arguments[used++] = "-m";
        arguments[used++] = "mkdocs";
        arguments[used++] = "build";
        arguments[used++] = "--strict";
        arguments[used++] = "--config-file";
        arguments[used++] = configFile;
        arguments[used++] = "--site-dir";
        arguments[used++] = siteDirectory;
    }
    arguments[used] = NULL;
    return EducationSpawn(python->executable, arguments);
}

static int EducationCandidate(EducationPython *candidate, const char *path, int launcher)
{
    if (path == NULL || *path == '\0') return 0;
    size_t length = strlen(path);
    if (length >= sizeof(candidate->executable)) return 0;
    memcpy(candidate->executable, path, length + 1U);
    candidate->launcherArgument = launcher;
    return EducationRun(candidate, 1, NULL, NULL) == 0;
}

/* --python is authoritative. Otherwise try a checkout-owned environment first,
 * then an explicit environment selection, then operating-system discovery.
 * On Windows, py -3 is intentionally examined before PATH's `python`: MSYS2
 * prepends C:\\msys64\\ucrt64\\bin to PATH for compiling C programs. */
static int EducationFindPython(const char *repositoryRoot, const char *requested,
                               EducationPython *selected)
{
    if (requested != NULL)
        return EducationCandidate(selected, requested, 0);
    char venv[UMICOM_EDUCATION_PATH_CAPACITY];
#if defined(_WIN32)
    const char *localInterpreter = ".venv/Scripts/python.exe";
#else
    const char *localInterpreter = ".venv/bin/python";
#endif
    if (EducationJoin(venv, sizeof(venv), repositoryRoot, localInterpreter) &&
        EducationFileExists(venv) && EducationCandidate(selected, venv, 0)) return 1;
    const char *preferred = getenv("UMICOM_EDUCATION_PYTHON");
    if (preferred != NULL && *preferred != '\0')
        return EducationCandidate(selected, preferred, 0);
#if defined(_WIN32)
    if (EducationCandidate(selected, "py", 1)) return 1;
#endif
    if (EducationCandidate(selected, "python3", 0)) return 1;
    if (EducationCandidate(selected, "python", 0)) return 1;
    return 0;
}

static void EducationExplainRendererFailure(const char *requested)
{
    if (requested != NULL)
        fprintf(stderr, "The selected interpreter is unavailable or lacks MkDocs and MkDocs Material: %s\n",
                requested);
    else
        fputs("No usable Python environment with MkDocs and MkDocs Material was found.\n", stderr);
    fputs("On Windows, prepare a repository-local environment using PowerShell:\n"
          "  Set-Location \"C:\\umicom\\Umicom-Bits2Banking\"\n"
          "  py -3 -m venv .venv\n"
          "  & \".\\.venv\\Scripts\\python.exe\" -m pip install -r .\\requirements-docs.txt\n"
          "  & \".\\.venv\\Scripts\\python.exe\" -m mkdocs --version\n"
          "Then repeat the build. If the Python launcher is unavailable,\n"
          "use the full path of a working Python interpreter with --python PATH.\n"
          "On Linux: python3 -m venv .venv && .venv/bin/python -m pip install -r requirements-docs.txt\n"
          "No existing Python installation is modified automatically.\n", stderr);
}

/* A renderer check can be requested before any site is built, making failure
 * visible in `doctor` rather than only when `serve` is attempted. */
int UmicomEducationCheckRenderer(const char *repositoryRoot, const char *requestedPython)
{
    EducationPython selected;
    if (repositoryRoot == NULL || *repositoryRoot == '\0') return 2;
    if (!EducationFindPython(repositoryRoot, requestedPython, &selected)) {
        fputs("[MISSING] MkDocs renderer (Python, mkdocs, material).\n", stderr);
        EducationExplainRendererFailure(requestedPython);
        return 1;
    }
    printf("[OK] MkDocs renderer available via %s%s\n", selected.executable,
           selected.launcherArgument ? " -3" : "");
    return 0;
}

int UmicomEducationBuildSiteWithPython(const char *repositoryRoot, const char *requestedPython)
{
    if (repositoryRoot == NULL || *repositoryRoot == '\0') return 2;
    char configuration[UMICOM_EDUCATION_PATH_CAPACITY];
    char destination[UMICOM_EDUCATION_PATH_CAPACITY];
    char indexPage[UMICOM_EDUCATION_PATH_CAPACITY];
    if (!EducationJoin(configuration, sizeof(configuration), repositoryRoot, "mkdocs.yml") ||
        !EducationJoin(destination, sizeof(destination), repositoryRoot, "site") ||
        !EducationJoin(indexPage, sizeof(indexPage), destination, "index.html")) return 2;
    if (!EducationFileExists(configuration)) {
        fprintf(stderr, "Missing MkDocs configuration: %s\n", configuration);
        return 2;
    }
    EducationPython selected;
    if (!EducationFindPython(repositoryRoot, requestedPython, &selected)) {
        EducationExplainRendererFailure(requestedPython);
        return 1;
    }
    printf("Rendering with %s%s; MkDocs output: %s\n", selected.executable,
           selected.launcherArgument ? " -3" : "", destination);
    fflush(stdout);
    int outcome = EducationRun(&selected, 0, configuration, destination);
    if (outcome != 0) {
        fprintf(stderr, "MkDocs failed (child exit code %d). No website success has been recorded.\n",
                outcome);
        return 1;
    }
    if (!EducationFileExists(indexPage)) {
        fprintf(stderr, "MkDocs reported success, but the expected home page is missing: %s\n", indexPage);
        return 1;
    }
    printf("[OK] Website built: %s\n", indexPage);
    return 0;
}

int UmicomEducationBuildSite(const char *repositoryRoot)
{
    return UmicomEducationBuildSiteWithPython(repositoryRoot, NULL);
}
