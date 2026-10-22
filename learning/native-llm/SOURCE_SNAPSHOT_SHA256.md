# Cryptographic identity for native training source snapshots

Umicom Framework's native C23 dataset loader copies approved local source files into immutable `UmiAiCorpus` instances. Training windows are subsequently drawn from those copies, not by repeatedly opening source paths.

The existing `UmiAiTrainingDatasetDocument` catalogue exposes a non-cryptographic FNV-1a64 fingerprint for each document. That remains useful for identifying accidental duplicates, but hashing an FNV fingerprint with SHA-256 does **not** give the original source bytes a collision-resistant identity. The new opt-in source identity hashes the exact imported bytes.

## The two digest interfaces

`UmiAiCorpusSha256` returns 32 bytes of SHA-256 over the corpus's owned source bytes. `UmiAiTrainingDatasetSourceSha256` returns 32 bytes over a domain-separated sequence that includes ordered document metadata and the individual corpus digests.

The dataset digest covers the following canonical sequence, independent of host byte order or C structure padding:

```text
ASCII "UMICOM-AI-DATASET-SOURCE-SNAPSHOT-1\n"
LE64 document_count
for every document in manifest order:
    LE32 partition (1=train, 2=eval)
    LE64 relative_path_byte_count
    raw relative_path bytes (without NUL)
    LE64 original_corpus_byte_count
    SHA256(original immutable source bytes) [32 raw bytes]
```

Every input is already held by the Framework dataset. No source path is reopened, no second corpus is created, and neither method exposes an internal byte pointer. Inputs remain borrowed. The 32-byte caller output is not altered after any validation or hashing failure.

## Optional checkpoint companion

A clipped AdamW run can opt into a new `.source-lock` record, in addition to the existing `.clip-policy` and `.training-lock` records:

```text
UMICOM-AI-SOURCE-LOCK-1
checkpoint_sha256=<64 lowercase hexadecimal characters>
dataset_source_sha256=<64 lowercase hexadecimal characters>
```

`UmiAiTrainingSourceLockFormat` and `UmiAiTrainingSourceLockVerify` operate on supplied canonical combined-checkpoint bytes and an already-loaded immutable dataset. `SaveNewFile` refuses to replace an existing path; `VerifyFile` reads a bounded record and refuses extra bytes or a changed representation.

The `.training-lock` continues to cover the AdamW optimiser, sampling seeds, schedule and clipping configuration. The `.source-lock` specifically replaces the weak source-fingerprint assumption with collision-resistant hashing of imported bytes. **Both records must be verified together**, along with `.clip-policy`, to claim a strict-source-lock resume.

The educational C23 CLI accepts `--strict-source-lock`, which **implies** the earlier `--strict-run-lock`. Its original invocations and file formats are preserved. Old runs do not magically acquire a cryptographic source lock; begin a new reviewed run to enable this policy.

## Verify the learning implementation

The focused CTest cases are:

- `framework.ai.training.corpus_source_sha256`: known SHA-256 vector and ownership.
- `framework.ai.training.dataset_source_sha256`: ordered-dataset identity and sampler stability.
- `framework.ai.training.source_lock`: exact formatting, checkpoint changes, no-clobber publication and refusal of corrupt records.
- `framework.ai.training.run_lock_disk_resume`: exercises all three independent records alongside the real combined-checkpoint round trip and byte-identical continuation.

The teaching dataset and limited CPU decoder are not a production LLM. SHA-256 is a **digest**, not a digital signature, certificate, security attestation, proof of training correctness, or source-file authorisation. A malicious source submitted before review can be hashed just as readily as legitimate source. File publication of a checkpoint and three companions is also not a single atomic filesystem transaction; incomplete bundles must be rejected on resume.

**Security boundary:** do not load untrusted training manifests, source files or checkpoints, and do not bypass anti-malware warnings to run the examples. Cryptographic signing, host attestation and source-review governance remain separate tasks.

**Author and organisation:** Sammy Hegab / Umicom Foundation · MIT.
