/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/release_baseline/main.c
 * PURPOSE:
 *   Read an explicit release contract and report missing or inconsistent evidence.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: tools/release_baseline/main.c
 * Purpose: Read an explicit release contract and report missing or inconsistent evidence.
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/evidence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
static wchar_t *Wide(const char *text)
{
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, NULL, 0);
    if (n <= 0) return NULL;
    wchar_t *out = calloc((size_t)n, sizeof(*out));
    if (out != NULL && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, out, n) != n) { free(out); return NULL; }
    return out;
}
#endif
/* Read only the two files explicitly selected by the caller. No traversal,
 * writes, shell expansion, Git calls, log execution or automatic network access.
 * The caller must use ordinary trusted local files. Filesystem parents and
 * concurrently changing inputs are not a security boundary or locked snapshot. */
static UmiStatus Read(const char *path, char **outText, size_t *outSize)
{
    FILE *file;
#ifdef _WIN32
    wchar_t *wide = Wide(path);
    if (wide == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    file = _wfopen(wide, L"rb"); free(wide);
#else
    file = fopen(path,"rb");
#endif
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    char *buffer = malloc(UMI_RELEASE_TEXT_LIMIT + 1U);
    if (buffer == NULL) { (void)fclose(file); return UMI_STATUS_OUT_OF_MEMORY; }
    size_t n = fread(buffer,1U,UMI_RELEASE_TEXT_LIMIT + 1U,file);
    bool bad = ferror(file) != 0;
    if (fclose(file) != 0) bad = true;
    if (bad || n > UMI_RELEASE_TEXT_LIMIT) { free(buffer); return bad ? UMI_STATUS_IO_ERROR : UMI_STATUS_CAPACITY_EXCEEDED; }
    *outText = buffer; *outSize = n;
    return UMI_STATUS_OK;
}
static void Reason(unsigned reason)
{
    if (reason == 0U) { (void)printf("reported-complete"); return; }
    if ((reason & UMI_RELEASE_EVIDENCE_MISSING) != 0U) (void)printf("missing ");
    if ((reason & UMI_RELEASE_EVIDENCE_CONTEXT) != 0U) (void)printf("wrong-context ");
    if ((reason & UMI_RELEASE_EVIDENCE_OUTCOME) != 0U) (void)printf("not-passed ");
    if ((reason & UMI_RELEASE_EVIDENCE_COUNTS) != 0U) (void)printf("incomplete-counts ");
    if ((reason & UMI_RELEASE_EVIDENCE_REFERENCE) != 0U) (void)printf("missing-reference ");
}
static int Run(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1],"--help") == 0) {
        (void)puts("Umicom release baseline\n  show CONTRACT.tsv\n  check CONTRACT.tsv EVIDENCE.tsv\n"
            "Exit 0: command complete or metadata complete; 1: release evidence blocked; 2: input/I/O error.\n"
            "This tool checks assertions, not their authenticity. It does not read referenced logs, verify artefact hashes,\n"
            "run tests, validate Git state, approve a product or publish a release. Use trusted ordinary local files.");
        return ferror(stdout) != 0 ? 2 : 0;
    }
    bool show = argc == 3 && strcmp(argv[1],"show") == 0;
    if (!show && !(argc == 4 && strcmp(argv[1],"check") == 0)) {
        (void)fputs("Use --help for the exact arguments.\n",stderr); return 2;
    }
    char *contractText = NULL, *evidenceText = NULL;
    size_t contractLength = 0U, evidenceLength = 0U;
    UmiReleaseContract *contract = NULL; UmiReleaseEvidence *evidence = NULL;
    UmiReleaseEvidenceAssessment assessment;
    UmiStatus status = Read(argv[2],&contractText,&contractLength);
    if (status == UMI_STATUS_OK) status = UmiReleaseContractParse(contractText,contractLength,&contract);
    const char empty[] = "UMICOM-RELEASE-EVIDENCE\t1\n";
    if (status == UMI_STATUS_OK && !show) status = Read(argv[3],&evidenceText,&evidenceLength);
    if (status == UMI_STATUS_OK) status = UmiReleaseEvidenceParse(show ? empty : evidenceText,show ? sizeof(empty)-1U : evidenceLength,&evidence);
    if (status == UMI_STATUS_OK) status = UmiReleaseEvidenceAssess(contract,evidence,&assessment);
    int result = 2;
    if (status != UMI_STATUS_OK) (void)fprintf(stderr,"Input refused: %s. No files were changed.\n",umi_status_text(status));
    else {
        (void)printf("Candidate label: %s\nExpected source revision: %s\n",UmiReleaseContractCandidate(contract),UmiReleaseContractSource(contract));
        (void)puts("ASSERTIONS ONLY: referenced logs, artefacts, signatures and Git state have not been verified by this tool.");
        for (size_t i = 0U; i < assessment.required; ++i) {
            const UmiReleaseRequirement *r = UmiReleaseContractAt(contract,i);
            (void)printf("%s [%s/%s, %s, %s]: ",r->id,r->profile,r->configuration,r->kind,r->ownerBatch);
            Reason(assessment.reasons[i]); (void)printf(" -- %s\n",r->title);
        }
        (void)printf("Required: %zu; reported complete: %zu; missing: %zu; rejected: %zu.\n",assessment.required,assessment.reportedComplete,assessment.missing,assessment.rejected);
        (void)puts(assessment.readyForOwnerReview ? "METADATA COMPLETE. Independent evidence inspection and product-owner acceptance are still required." : "RELEASE BLOCKED. No product is certified stable by this report.");
        result = show || assessment.readyForOwnerReview ? 0 : 1;
        if (fflush(stdout) != 0 || ferror(stdout) != 0) result = 2;
    }
    UmiReleaseEvidenceDestroy(evidence); UmiReleaseContractDestroy(contract);
    free(evidenceText); free(contractText);
    return result;
}
#ifdef _WIN32
int wmain(int argc, wchar_t **argv)
{
    char **args = calloc((size_t)argc + 1U,sizeof(*args));
    if (args == NULL) return 2;
    int result = 2; bool valid = true;
    for (int i = 0; i < argc; ++i) {
        int n = WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,argv[i],-1,NULL,0,NULL,NULL);
        if (n <= 0 || (args[i] = malloc((size_t)n)) == NULL || WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,argv[i],-1,args[i],n,NULL,NULL) != n) { valid = false; break; }
    }
    if (valid) result = Run(argc,args);
    for (int i = 0; i < argc; ++i) free(args[i]);
    free(args); return result;
}
#else
int main(int argc, char **argv) { return Run(argc,argv); }
#endif
