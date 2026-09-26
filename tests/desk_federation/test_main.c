/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_main.c
 *
 * PURPOSE:
 *   Run one named, registered regression case with a useful failure location.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
typedef int (*TestFn)(const char *, const char *);
typedef struct TestCase { const char *name; TestFn run; const char *argument; } TestCase;
static const TestCase CASES[] = {
    {"groups.inert_create_reload", TestGroups, "inert_create_reload"},
    {"groups.save_roundtrip", TestGroups, "save_roundtrip"},
    {"groups.replace_and_shrink", TestGroups, "replace_and_shrink"},
    {"groups.remove_middle", TestGroups, "remove_middle"},
    {"groups.remove_last", TestGroups, "remove_last"},
    {"groups.duplicate_members", TestGroups, "duplicate_members"},
    {"groups.empty_members", TestGroups, "empty_members"},
    {"groups.member_capacity", TestGroups, "member_capacity"},
    {"groups.workspace_capacity", TestGroups, "workspace_capacity"},
    {"groups.utf8_title", TestGroups, "utf8_title"},
    {"groups.malformed_title", TestGroups, "malformed_title"},
    {"groups.unterminated_fields", TestGroups, "unterminated_fields"},
    {"groups.self_rejected", TestGroups, "self_rejected"},
    {"groups.group_session_namespaces", TestGroups, "group_session_namespaces"},
    {"groups.capture_running", TestGroups, "capture_running"},
    {"groups.capture_empty", TestGroups, "capture_empty"},
    {"groups.capture_transient", TestGroups, "capture_transient"},
    {"groups.copied_ownership", TestGroups, "copied_ownership"},
    {"groups.invalid_kind_id", TestGroups, "invalid_kind_id"},
    {"review.preview_inert", TestReview, "preview_inert"},
    {"review.single_use", TestReview, "single_use"},
    {"review.cancel_inert", TestReview, "cancel_inert"},
    {"review.new_preview_supersedes", TestReview, "new_preview_supersedes"},
    {"review.caller_plan_cannot_tamper", TestReview, "caller_plan_cannot_tamper"},
    {"review.running_activates", TestReview, "running_activates"},
    {"review.skip_reasons", TestReview, "skip_reasons"},
    {"review.missing_application", TestReview, "missing_application"},
    {"review.self_skipped", TestReview, "self_skipped"},
    {"review.stale_generation", TestReview, "stale_generation"},
    {"review.stale_token", TestReview, "stale_token"},
    {"review.stale_presence", TestReview, "stale_presence"},
    {"review.stale_running", TestReview, "stale_running"},
    {"review.stale_new_registration", TestReview, "stale_new_registration"},
    {"review.workspace_edit_cancels", TestReview, "workspace_edit_cancels"},
    {"review.partial_failure_continues", TestReview, "partial_failure_continues"},
    {"review.mixed_skip_and_request", TestReview, "mixed_skip_and_request"},
    {"review.per_item_recheck", TestReview, "per_item_recheck"},
    {"review.reentry_rejected", TestReview, "reentry_rejected"},
    {"review.acceptance_not_readiness", TestReview, "acceptance_not_readiness"},
    {"review.read_failure_consumes", TestReview, "read_failure_consumes"},
    {"review.bad_catalogue", TestReview, "bad_catalogue"},
    {"review.unrelated_change_allowed", TestReview, "unrelated_change_allowed"},
    {"links.roundtrip", TestLinks, "roundtrip"},
    {"links.reject_external", TestLinks, "reject_external"},
    {"links.reject_encoding", TestLinks, "reject_encoding"},
    {"links.empty_and_unknown", TestLinks, "empty_and_unknown"},
    {"links.length_boundaries", TestLinks, "length_boundaries"},
    {"links.output_unchanged", TestLinks, "output_unchanged"},
    {"links.review_link_inert", TestLinks, "review_link_inert"},
    {"storage.borrowed_transaction", TestStorage, "borrowed_transaction"},
    {"storage.other_namespace_preserved", TestStorage, "other_namespace_preserved"},
    {"storage.corrupt_header", TestStorage, "corrupt_header"},
    {"storage.missing_member", TestStorage, "missing_member"},
    {"storage.missing_meta", TestStorage, "missing_meta"},
    {"storage.extra_namespace_record", TestStorage, "extra_namespace_record"},
    {"storage.numeric_meta_rejected", TestStorage, "numeric_meta_rejected"},
    {"storage.duplicate_stored_members", TestStorage, "duplicate_stored_members"},
    {"storage.bad_hex_title", TestStorage, "bad_hex_title"},
    {"storage.revision_exhaustion", TestStorage, "revision_exhaustion"},
    {"storage.reload_cancels_review", TestStorage, "reload_cancels_review"},
    {"storage.sqlite_restart", TestStorage, "sqlite_restart"},
    {"storage.sqlite_stale_writer", TestStorage, "sqlite_stale_writer"},
    {"storage.sqlite_stale_review", TestStorage, "sqlite_stale_review"},
    {"storage.sqlite_write_rollback", TestStorage, "sqlite_write_rollback"},
    {"storage.sqlite_delete_rollback", TestStorage, "sqlite_delete_rollback"},
    {"activity.initial_baseline", TestActivity, "initial_baseline"},
    {"activity.unchanged_deduplicated", TestActivity, "unchanged_deduplicated"},
    {"activity.state_change", TestActivity, "state_change"},
    {"activity.acknowledge", TestActivity, "acknowledge"},
    {"activity.bounded_ring", TestActivity, "bounded_ring"},
    {"activity.read_failure_preserves_baseline", TestActivity, "read_failure_preserves_baseline"},
    {"activity.removed_registration", TestActivity, "removed_registration"},
    {"activity.non_durable", TestActivity, "non_durable"},
    {"storage.same_revision_damage_blocks_review", TestStorage, "same_revision_damage_blocks_review"},
    {"storage.same_revision_damage_blocks_execute", TestStorage, "same_revision_damage_blocks_execute"},
    {"storage.same_revision_damage_blocks_write", TestStorage, "same_revision_damage_blocks_write"},
    {"storage.data_server_copy_boundaries", TestStorage, "data_server_copy_boundaries"},
};
int main(int argc, char **argv)
{
    if (argc < 2) { fprintf(stderr, "Supply a named test case.\n"); return 2; }
    for (size_t index = 0U; index < sizeof CASES / sizeof CASES[0]; ++index) {
        if (strcmp(argv[1], CASES[index].name) == 0) {
            const int result = CASES[index].run(CASES[index].argument, argc > 2 ? argv[2] : NULL);
            if (result == 0) printf("PASS %s\n", CASES[index].name);
            else if (result == 77) printf("SKIP %s: SQLite is not built.\n", CASES[index].name);
            return result;
        }
    }
    fprintf(stderr, "Unknown test: %s\n", argv[1]); return 2;
}
