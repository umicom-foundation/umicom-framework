/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_recovery/main.c
 * PURPOSE: Dispatch each registered boundary without inventing a successful unknown test.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    static const char *const names[] = {
        "query_defaults",
        "query_empty",
        "query_sort_id_desc",
        "query_sort_label",
        "query_sort_label_desc",
        "query_sort_quantity",
        "query_sort_quantity_desc",
        "query_filter_unicode",
        "query_filter_origin",
        "query_filter_multiline",
        "query_filter_case",
        "query_filter_none",
        "query_range",
        "query_exact_quantity",
        "query_invalid_utf8",
        "query_unterminated",
        "query_invalid_sort",
        "query_reverse_range",
        "query_range_overflow",
        "query_missing",
        "query_denied",
        "query_page_edges",
        "query_64_rows",
        "query_frozen",
        "query_lifetime",
        "query_copy_isolation",
        "recovery_preview",
        "recovery_workflow",
        "recovery_rejected",
        "recovery_cancelled",
        "recovery_disabled",
        "recovery_paused",
        "recovery_applied",
        "recovery_missing",
        "recovery_denied",
        "recovery_report",
        "recovery_lifetime",
        "recovery_stale",
        "recovery_actor_changed",
        "recovery_existing_id",
        "recovery_invalid_id",
        "recovery_revoked",
        "sqlite_write_failure",
        "sqlite_restart",
        "storage_changed_chunk",
        "storage_noop",
        "storage_unknown",
        "storage_missing",
        "storage_namespace",
        "storage_reprepare",
        "sqlite_changed_chunk",
        "sqlite_stale_writer",
    };
    bool known=false;
    if(argc<2)return 2;
    for(size_t i=0U;i<sizeof(names)/sizeof(names[0]);++i) if(strcmp(names[i],argv[1])==0) known=true;
    if(!known){fprintf(stderr,"Unknown test %s\n",argv[1]);return 2;}
    if(strncmp(argv[1],"query_",6U)==0)return TestQueries(argv[1]);
    if(strncmp(argv[1],"storage_",8U)==0||strcmp(argv[1],"sqlite_changed_chunk")==0||strcmp(argv[1],"sqlite_stale_writer")==0)return TestStorage(argv[1],argc>2?argv[2]:NULL);
    if(strncmp(argv[1],"recovery_",9U)==0||strncmp(argv[1],"sqlite_",7U)==0)return TestRecovery(argv[1],argc>2?argv[2]:NULL);
    fprintf(stderr,"Unknown test %s\n",argv[1]);return 2;
}
