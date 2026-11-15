/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/architecture.c
 *
 * PURPOSE:
 *   Enforce Framework/Studio boundaries and flag excessive source-level
 *   coupling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/architecture.h"
#include "umicom/codeguard/source.h"
#include "source_reader.h"
#include <stdio.h>
#include <string.h>
/* Provide the finding operation used by this module and its client applications. */
/* Return finding storage failures to callers rather than silently omitting architecture evidence. The previous implementation is retained for engineering review. */
#if 0
static void finding(UmiCodeGuardResult *r,const char *id,UmiCodeGuardSeverity sev,unsigned conf,const char *path,const char *msg,const char *fix){UmiCodeGuardFinding f={0};(void)snprintf(f.rule_id,sizeof(f.rule_id),"%s",id);f.severity=sev;f.category=UMI_CODEGUARD_CATEGORY_ARCHITECTURE;f.confidence=conf;(void)snprintf(f.path,sizeof(f.path),"%s",path);f.line=1U;f.column=1U;(void)snprintf(f.message,sizeof(f.message),"%s",msg);(void)snprintf(f.remediation,sizeof(f.remediation),"%s",fix);(void)umi_codeguard_result_add(r,&f);}
#endif
static UmiStatus finding(UmiCodeGuardResult *result, const char *id,
    UmiCodeGuardSeverity severity, unsigned confidence, const char *path,
    const char *message, const char *remediation)
{
    UmiCodeGuardFinding item = {0};
    (void)snprintf(item.rule_id, sizeof item.rule_id, "%s", id);
    item.severity = severity;
    item.category = UMI_CODEGUARD_CATEGORY_ARCHITECTURE;
    item.confidence = confidence;
    (void)snprintf(item.path, sizeof item.path, "%s", path);
    item.line = 1U;
    item.column = 1U;
    (void)snprintf(item.message, sizeof item.message, "%s", message);
    (void)snprintf(item.remediation, sizeof item.remediation, "%s", remediation);
    return umi_codeguard_result_add(result, &item);
}
/*
 * Provide the codeguard architecture scan file operation used by this module and its
 * client applications.
 */
/* Propagate measurement, finding, read and close failures so a retained service cannot publish incomplete architecture evidence as a successful scan. The previous implementation remains for review. */
#if 0
UmiStatus umi_codeguard_architecture_scan_file(const char *root,const char *path,const UmiCodeGuardProfile *p,UmiCodeGuardResult *r){UmiCodeGuardSourceInfo info;FILE *f;char line[8192];size_t ln=0U;(void)root;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(path==NULL||p==NULL||r==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_codeguard_source_measure(path,&info)!=UMI_STATUS_OK)return UMI_STATUS_IO_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(info.include_count>p->max_include_count)finding(r,"CODEGUARD-ARCH-COUPLING-002",UMI_CODEGUARD_MEDIUM,85U,path,"Source file has unusually high include fan-out.","Split responsibilities or depend on narrower public contracts.");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(info.lines>p->max_source_lines)finding(r,"CODEGUARD-ARCH-GOD-001",UMI_CODEGUARD_MEDIUM,82U,path,"Source file is large enough to merit a responsibility review.","Split cohesive responsibilities into independently testable modules without fragmenting trivial code.");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(path,"framework/include/")!=NULL){f=fopen(path,"rb");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL)return UMI_STATUS_IO_ERROR;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(fgets(line,sizeof(line),f)!=NULL){++ln;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(line,"gtk/gtk.h")!=NULL||strstr(line,"GtkWidget")!=NULL){UmiCodeGuardFinding x={0};(void)snprintf(x.rule_id,sizeof(x.rule_id),"CODEGUARD-ARCH-UI-002");x.severity=UMI_CODEGUARD_HIGH;x.category=UMI_CODEGUARD_CATEGORY_ARCHITECTURE;x.confidence=99U;(void)snprintf(x.path,sizeof(x.path),"%s",path);x.line=ln;(void)snprintf(x.message,sizeof(x.message),"Toolkit-specific GTK type/header leaked into a public Framework header.");(void)snprintf(x.remediation,sizeof(x.remediation),"Move GTK details behind the Framework GTK adapter and expose toolkit-neutral data/contracts.");(void)umi_codeguard_result_add(r,&x);}}fclose(f);}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(path,"applications/studio/")!=NULL){f=fopen(path,"rb");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL)return UMI_STATUS_IO_ERROR;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(fgets(line,sizeof(line),f)!=NULL){++ln;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(line,"sqlite3.h")!=NULL||strstr(line,"sqlite3_")!=NULL)finding(r,"CODEGUARD-ARCH-DATA-002",UMI_CODEGUARD_HIGH,98U,path,"Studio appears to access SQLite directly instead of the Data Server.","Route persistence through Umicom Data Server repositories and transactions.");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(line,"framework/src/")!=NULL)finding(r,"CODEGUARD-ARCH-PRIVATE-002",UMI_CODEGUARD_HIGH,99U,path,"Studio references Framework private implementation source.","Use only installed/public include/umicom APIs and exported CMake targets.");}fclose(f);}return UMI_STATUS_OK;}
#endif
/* Apply the configured byte limit to measurement and every architecture pass through shared native input. Preserve the prior stdio policy for engineering review. The previous implementation remains for review. */
#if 0
UmiStatus umi_codeguard_architecture_scan_file(const char *root, const char *path,
    const UmiCodeGuardProfile *profile, UmiCodeGuardResult *result)
{
    (void)root;
    if (path == NULL || profile == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardSourceInfo info;
    UmiStatus status = umi_codeguard_source_measure(path, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.include_count > profile->max_include_count) {
        status = finding(result, "CODEGUARD-ARCH-COUPLING-002", UMI_CODEGUARD_MEDIUM,
            85U, path, "Source file has unusually high include fan-out.",
            "Split responsibilities or depend on narrower public contracts.");
        if (status != UMI_STATUS_OK) return status;
    }
    if (info.lines > profile->max_source_lines) {
        status = finding(result, "CODEGUARD-ARCH-GOD-001", UMI_CODEGUARD_MEDIUM,
            82U, path, "Source file is large enough to merit a responsibility review.",
            "Split cohesive responsibilities into independently testable modules without fragmenting trivial code.");
        if (status != UMI_STATUS_OK) return status;
    }
    size_t line_number = 0U;
    /* Retain the existing two textual policy phases and path classification.
     * Both now stop on a failed finding and close their stream before return. */
    for (unsigned phase = 0U; phase < 2U; ++phase) {
        if (phase == 0U && strstr(path, "framework/include/") == NULL) continue;
        if (phase == 1U && strstr(path, "applications/studio/") == NULL) continue;
        FILE *file = fopen(path, "rb");
        if (file == NULL) return UMI_STATUS_IO_ERROR;
        char line[8192];
        while (status == UMI_STATUS_OK && fgets(line, sizeof line, file) != NULL) {
            ++line_number;
            if (phase == 0U &&
                (strstr(line, "gtk/gtk.h") != NULL || strstr(line, "GtkWidget") != NULL)) {
                UmiCodeGuardFinding item = {0};
                (void)snprintf(item.rule_id, sizeof item.rule_id, "CODEGUARD-ARCH-UI-002");
                item.severity = UMI_CODEGUARD_HIGH;
                item.category = UMI_CODEGUARD_CATEGORY_ARCHITECTURE;
                item.confidence = 99U;
                (void)snprintf(item.path, sizeof item.path, "%s", path);
                item.line = line_number;
                (void)snprintf(item.message, sizeof item.message,
                    "Toolkit-specific GTK type/header leaked into a public Framework header.");
                (void)snprintf(item.remediation, sizeof item.remediation,
                    "Move GTK details behind the Framework GTK adapter and expose toolkit-neutral data/contracts.");
                status = umi_codeguard_result_add(result, &item);
            }
            if (phase == 1U && (strstr(line, "sqlite3.h") != NULL || strstr(line, "sqlite3_") != NULL))
                status = finding(result, "CODEGUARD-ARCH-DATA-002", UMI_CODEGUARD_HIGH,
                    98U, path, "Studio appears to access SQLite directly instead of the Data Server.",
                    "Route persistence through Umicom Data Server repositories and transactions.");
            if (status == UMI_STATUS_OK && phase == 1U && strstr(line, "framework/src/") != NULL)
                status = finding(result, "CODEGUARD-ARCH-PRIVATE-002", UMI_CODEGUARD_HIGH,
                    99U, path, "Studio references Framework private implementation source.",
                    "Use only installed/public include/umicom APIs and exported CMake targets.");
        }
        if (ferror(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
        if (fclose(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
#endif
typedef struct UmiCodeGuardArchitectureVisit {
    const char *path;
    UmiCodeGuardResult *result;
    unsigned phase;
    size_t line_number;
} UmiCodeGuardArchitectureVisit;

/* Keep the existing textual policy and diagnostic wording. The shared reader
 * owns segmentation, budgets, native path handling and cancellation. */
static UmiStatus umi_codeguard_architecture_line(const char *line, size_t size,
    size_t number, void *context)
{
    (void)size;
    (void)number;
    UmiCodeGuardArchitectureVisit *visit = context;
    if (visit->line_number == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++visit->line_number;
    UmiStatus status = UMI_STATUS_OK;
    if (visit->phase == 0U &&
        (strstr(line, "gtk/gtk.h") != NULL || strstr(line, "GtkWidget") != NULL)) {
        UmiCodeGuardFinding item = {0};
        (void)snprintf(item.rule_id, sizeof item.rule_id, "CODEGUARD-ARCH-UI-002");
        item.severity = UMI_CODEGUARD_HIGH;
        item.category = UMI_CODEGUARD_CATEGORY_ARCHITECTURE;
        item.confidence = 99U;
        (void)snprintf(item.path, sizeof item.path, "%s", visit->path);
        item.line = visit->line_number;
        (void)snprintf(item.message, sizeof item.message,
            "Toolkit-specific GTK type/header leaked into a public Framework header.");
        (void)snprintf(item.remediation, sizeof item.remediation,
            "Move GTK details behind the Framework GTK adapter and expose toolkit-neutral data/contracts.");
        status = umi_codeguard_result_add(visit->result, &item);
    }
    if (visit->phase == 1U &&
        (strstr(line, "sqlite3.h") != NULL || strstr(line, "sqlite3_") != NULL))
        status = finding(visit->result, "CODEGUARD-ARCH-DATA-002", UMI_CODEGUARD_HIGH,
            98U, visit->path, "Studio appears to access SQLite directly instead of the Data Server.",
            "Route persistence through Umicom Data Server repositories and transactions.");
    if (status == UMI_STATUS_OK && visit->phase == 1U && strstr(line, "framework/src/") != NULL)
        status = finding(visit->result, "CODEGUARD-ARCH-PRIVATE-002", UMI_CODEGUARD_HIGH,
            99U, visit->path, "Studio references Framework private implementation source.",
            "Use only installed/public include/umicom APIs and exported CMake targets.");
    return status;
}

UmiStatus umi_codeguard_architecture_scan_file_cancellable(const char *root,
    const char *path, const UmiCodeGuardProfile *profile, UmiCodeGuardResult *result,
    const UmiCancellationToken *cancellation)
{
    (void)root;
    if (path == NULL || profile == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardSourceInfo info;
    UmiStatus status = umi_codeguard_source_measure_bounded(path, profile->max_file_bytes,
        &info, cancellation);
    if (status != UMI_STATUS_OK) return status;
    if (info.include_count > profile->max_include_count) {
        status = finding(result, "CODEGUARD-ARCH-COUPLING-002", UMI_CODEGUARD_MEDIUM,
            85U, path, "Source file has unusually high include fan-out.",
            "Split responsibilities or depend on narrower public contracts.");
        if (status != UMI_STATUS_OK) return status;
    }
    if (umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    if (info.lines > profile->max_source_lines) {
        status = finding(result, "CODEGUARD-ARCH-GOD-001", UMI_CODEGUARD_MEDIUM,
            82U, path, "Source file is large enough to merit a responsibility review.",
            "Split cohesive responsibilities into independently testable modules without fragmenting trivial code.");
        if (status != UMI_STATUS_OK) return status;
    }
    UmiCodeGuardArchitectureVisit visit = {.path = path, .result = result};
    /* Each independent pass rechecks its own open handle and byte budget.
     * There is no claim that all passes observe one immutable file version. */
    for (unsigned phase = 0U; phase < 2U; ++phase) {
        if (phase == 0U && strstr(path, "framework/include/") == NULL) continue;
        if (phase == 1U && strstr(path, "applications/studio/") == NULL) continue;
        visit.phase = phase;
        status = umi_codeguard_source_visit_lines(path, profile->max_file_bytes,
            umi_codeguard_architecture_line, &visit, cancellation);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_cancellation_token_is_requested(cancellation)
        ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}

UmiStatus umi_codeguard_architecture_scan_file(const char *root, const char *path,
    const UmiCodeGuardProfile *profile, UmiCodeGuardResult *result)
{
    return umi_codeguard_architecture_scan_file_cancellable(root, path, profile, result, NULL);
}
