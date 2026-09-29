# Enterprise recovery and owned dataset pages

Sammy Hegab · Umicom Foundation · MIT

## Authority and compatibility

The feature extends `Umicom::enterprise_workspace`. Data Server remains the only database authority. No replacement row store, SQL frontend, new serializer, schema version or background worker is introduced. Existing IDs, recipes, job states, public structures and saved CSV remain intact. The active original Prepare API delegates to the shared private `EwsPrepareJob` path; its earlier implementation remains in an explained disabled block.

## Query contracts

`UmiEnterpriseRowQueryInit` sets ID ascending and the inclusive range 0..INT64_MAX. Capture authorises `enterprise.read` on the dataset and copies its currently loaded records. Literal text is case-sensitive over ID, label and sourceJob. Sorting compares UTF-8 bytes, not locale collation. Primary descending never reverses the ascending ID tie-break. Quantities are numeric, not string-sorted.

The opaque view owns up to 64 rows. Page limits are 1..16; offset equal to total yields an empty final page and larger offsets return NOT_FOUND. Failed page/description calls preserve scalar output. Capture sets the output pointer to null before validation. Destroy accepts null. Copies remain readable after the workspace closes; their disclosure/lifetime is a host responsibility, not a revocable query token.

## Re-preparation contracts

Inspection accepts non-applied jobs, including rejected/cancelled jobs, only with read permission on the old ID and prepare permission on its dataset. It re-evaluates the original frozen CSV through the canonical preview routine. It copies the original and proposed previews, current head/revision and preparer identity/role. This action does not save, approve, cancel or apply anything.

Preparation requires an unused new ID and the same inspected state/actor. Permissions are rechecked. The canonical candidate and transactional save write a new REVIEW job and a single `job.reprepare` audit record: target=new ID, detail=original ID. Approval is never inherited. Any intervening workspace change requires inspection again. The original remains untouched; callers explicitly cancel obsolete work through existing commands.

Pause permits preparation but blocks applying. Capacity errors and storage errors remain failures. A failed save that rolls back leaves no new job; a rollback failure sets storageFault and requires closing the owning instance. Existing applied-job retries remain idempotent; re-preparing applied jobs is refused.

## Storage integrity

Both EwsCurrent (including no-ops) and EwsSave compare all stored chunks and namespace inventory with the loaded canonical encoding within an active Data Server transaction. A same-header changed chunk is BUSY; malformed/missing inventory is PARSE_ERROR. The previous state remains readable and unexpected stored bytes are retained. This is consistency checking, not authentication, encryption, an external-writer lock, or self-healing.

## Ownership and concurrency

One owner thread serialises workspace access. Borrowed Data Server and authorisation objects must outlive it. Capture must not race mutation or destruction. An owned view/review may outlive the originating workspace but must not be destroyed concurrently with its readers. There are no callback registries, worker queues or timers here.

The GTK Slave Controller owns its two snapshots, disposes them on replacement/destruction, disconnects widget callbacks before controller destruction and sends shared commands. The original five page positions remain unchanged. Reload invalidates a recovery plan, while a query stays clearly labelled frozen until recaptured. Six optional GTK tests supplement the two original ones; no missing-display skip should be recorded as a pass.

## Limits and qualification

Existing limits: 8 datasets,64 rows/dataset,32 rows/import,16 jobs,128 audit records,16,384 CSV bytes; new page bound16. No general SQL execution, remote connector, schedule, authenticated login, encryption or signed package is added. Public tutorial: docs/learning/ENTERPRISE_RECOVERY.html. Focused build: examples/enterprise_recovery; OS composition: tools/enterprise-recovery.
