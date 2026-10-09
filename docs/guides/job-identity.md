# Record the inputs a job refers to

A saved outcome and current readiness answer different questions. “Succeeded” says
that a recorded operation finished successfully. It does not say that a later edit,
a different compiler or another project has been checked.

1. Keep job metadata in a private Data Server scope. Never put credentials, command
   lines, account details or raw provider responses in its captions.
2. Use `UmiJobHistoryBeginIdentified` to reserve an outcome with a copied
   `UmiJobIdentity`. Reservation and identity persistence share one transaction.
3. Record progress only after the corresponding work has happened. Stop before
   starting another external action if its checkpoint cannot be saved.
4. Compare recorded and current identities using `UmiJobIdentityCompare`.
   Show missing evidence as unknown. An empty input digest is not an empty source tree.
5. Keep external operations separate from history reads. An unfinished record after
   restart does not establish whether a provider request, process or payment finished.
   Reconcile with the responsible system before considering another action.

The identity contains three SHA-256 digests. The subject identifies a resource or
project. Configuration identifies reviewed settings. Inputs identifies a host-defined
set of immutable inputs. Use length-delimited fields and a documented ordering when
constructing a compound digest, so different field sequences cannot have the same
encoding. Include dependency content and settings that affect the operation. The
history store checks syntax, not completeness or authenticity.

For compiler workflows, `UmiBuildProfileJobIdentity` hashes the resolved project root
and the supplied profile values. It intentionally leaves Inputs empty because it
does not capture a source snapshot. Paths, compiler names and preset names can remain
the same while their contents change. The job caption identifies the requested phase;
matching settings across phases is not proof that those phases all ran.

`examples/data/job_identity.c` demonstrates a local line-count operation over an
explicit buffer, then compares the record against changed input bytes. It uses an
in-memory server and does not connect to any service. The optional learning target is
`umicom-job-identity-example`.

`Umicom::base` owns portable SHA-256. `Umicom::data` owns identity validation and
durable history. Existing native-launcher digest APIs remain compatible. These
digests are neither encryption nor signatures; keep the database private and never
treat a digest match as permission to run code, submit an order or transfer money.

If a history cannot be decoded, preserve it for inspection rather than replacing
it with an empty database. Legacy records without identity remain supported.

## Fingerprint an explicit input set

1. Create a `UmiJobInputs` builder with the maximum number of inputs your operation
   accepts. Capacity is explicit; a full set refuses further inputs instead of
   silently omitting dependencies.
2. Add each owned buffer with `UmiJobInputsAddBytes`, using a stable logical name.
   For streamed content, calculate its SHA-256 digest and use
   `UmiJobInputsAddDigest`. The builder copies names and digests, not source bytes.
3. Seal the builder with `UmiJobInputsSeal` and copy the returned digest into the
   job identity's Inputs field. Names are sorted, so insertion order does not change
   the digest. A duplicate logical name is refused even if its content is identical.
4. Execute against the same owned input buffers. Re-reading mutable files after
   sealing does not establish that the recorded content was actually processed.
5. Destroy the builder when finished. Sealing is repeatable, but a sealed builder
   refuses additional inputs.

An explicitly empty set has a digest; missing input evidence remains an empty
string. Logical names are case-sensitive byte strings and are never opened as paths.
Unicode normalisation, dependency discovery, file ownership and provider permissions
remain the host's responsibility. The builder performs no filesystem or network I/O.
