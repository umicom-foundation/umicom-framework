# Independently signed native LLM release approval

## Purpose

A valid Ed25519 publisher signature confirms that the publisher key signed the
exact checkpoint publication record. It does **not** confirm that another
person or organisation reviewed and authorised the model for distribution.
An existing native release-approval file pins the SHA-256 identity and order
of one to eight already verified checkpoint publications, but its contents are
plain text and therefore require protection outside the release package.

The optional Framework **signed release approval** layer allows an approved
release authority to sign the exact original plaintext approval bytes with a
separately chosen Ed25519 key. The release verifier uses that independently
trusted approval public key *and* the existing publisher trust policy. The
signer does not modify any model, optimiser state or source file.

## Native C23 components

- `include/umicom/ai/training/training_approval_attestation.h`: stable opt-in
  C ABI for signing, verification and failure-atomic JSON release checks.
- `src/ai/training/training_approval_attestation.c`: bounded approval input,
  Ed25519 with OpenSSL 3, strict detached-record grammar, no-clobber file
  publication and composition of the existing release-approval verifier.
- `learning/native-llm/verify_signed_release_approval.c`: thin CLI exposing
  the Framework APIs without implementing independent cryptographic logic.

None of these files is used when
`UMICOM_AI_TRAINING_ENABLE_SIGNATURES=OFF`. Existing unsigned approval APIs,
checkpoint envelopes, source locks, gradients and optimiser interfaces remain
unchanged.

## Approval format and signature record

The reviewer creates an approval for an **already verified** ordered batch:

```text
UMICOM-AI-RELEASE-APPROVAL-1
publication_count=2
evaluated_at_unix_seconds=150
batch_sha256=<64 lowercase hexadecimal characters>
```

The value of `batch_sha256` comes from a successful native trust-batch
verification. The example time is illustrative; operators must establish their
own verification and authorisation policy.

The `sign` operation creates a *new* sibling file
`<approval-path>.ed25519` with strict ASCII fields:

```text
UMICOM-AI-RELEASE-APPROVAL-SIGNATURE-1
public_key_sha256=<SHA-256 of raw Ed25519 approval public key>
signature=<128 lowercase hexadecimal characters>
```

The signature covers the byte string:

```text
UMICOM-AI-RELEASE-APPROVAL-ED25519-1\n
<exact original approval file bytes>
```

The actual signed message has no delimiter or newline between the domain
label's final LF and the first byte of the existing approval file. Therefore,
changing line endings, whitespace, comments or any of the approval keys
invalidates the signature. The signature record does **not** supply its own
trusted key: its fingerprint is only checked against the independently
selected approval public key.

## Build and use

From the Umicom Applications repository root in a configured Windows UCRT64
PowerShell session:

```powershell
cmake -S .\framework\learning\native-llm `
  -B .\build\windows-ucrt64-native-llm-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
  -DUMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON
cmake --build .\build\windows-ucrt64-native-llm-debug `
  --target umicom-native-llm-signed-release --parallel 2
```

The key material must have been provisioned, reviewed and stored separately
from source control. To sign a reviewed approval:

```powershell
& '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-signed-release.exe' `
  sign 'C:\umicom\reviewed\model-release.approval' `
  'C:\umicom\protected-keys\release-approver-private.pem'
```

To verify one approved publication (append more checkpoints in approved order):

```powershell
& '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-signed-release.exe' `
  verify 'C:\umicom\reviewed\model-release.approval' `
  'C:\umicom\trusted-keys\release-approver-public.pem' `
  'C:\umicom\trusted-keys\publishers.trust' `
  'C:\umicom\release\model.umi'
```

The same first-party native verifier checks all referenced checkpoint evidence
again under the publisher trust policy. On complete success it emits one JSON
record with `release_approval.approval_record_signed: true` and a separate
`approval_signature.verification_key_sha256` identifier. On error it emits no
success JSON and returns a nonzero process exit status.

## Tests and limitations

With signing support enabled, focused native CTest cases are
`framework.ai.training.signed_approval_roundtrip` and
`framework.ai.training.signed_approval_rejections`.

The tests generate disposable Ed25519 keys and publication evidence locally;
no private key files or generated checkpoints are committed.

An independently supplied public key provides a cryptographic trust anchor,
not proof that the key belongs to a specific legal reviewer. The caller must
protect both the publisher trust policy and the approval-key distribution
channel, and must establish organisational separation of duties. Verification
uses sequential file reads rather than an atomic filesystem snapshot. Ed25519
does not provide a trusted signing timestamp, a hardware-backed key, revocation
infrastructure or assurance of training-data rights and model quality. Use a
controlled host and immutable evidence, and reverify immediately before any
release operation.
