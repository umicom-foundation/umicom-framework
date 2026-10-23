# Reviewing and approving a native LLM release

A signed training checkpoint and a verified publisher trust policy are important
requirements for release confidence. They are not, by themselves, an approval
of a particular *selection* of model checkpoints. A trusted signer might have
signed several different experiments, including ones never intended to ship.

Umicom Framework offers an **optional, explicit release approval gate**.
It reuses the existing Ed25519 publisher checks, signed publication verifier
and ordered JSON trust-batch service. The new C23 layer checks one separately
approved expected digest before issuing an approved-release receipt.

## Why there are two reviews

1. **Publisher trust:** Does each checkpoint have a valid Ed25519 signature
   from a publisher allowed by the independently maintained trust policy at
   the selected evaluation instant?
2. **Release selection:** Are these the *exact checkpoints in the approved
   order* that the release reviewer selected?

A valid signature answers only the first question. A matching batch digest
answers only the second, provided the expected digest comes from a separately
protected and independently reviewed source.

## Prepare the approved batch

After reviewing the signed publications and benchmark evidence, verify a
candidate release using the existing native command:

```text
umicom-native-llm-trust-batch <trust-policy> <evaluation-unix-seconds> \
    <baseline.umi> <candidate.umi>
```

Check the output's `batch_sha256` field together with the signer fingerprints,
source-lock identities, publication order and trust-policy validity. Choose
an explicit evaluation time that is appropriate to your release process.
**This is not a trusted signing timestamp.** Do not select an earlier instant
to bypass key revocation or other current security policy.

Store the reviewed expected digest in an **independently controlled** local
file, outside the proposed release directory:

```ini
UMICOM-AI-RELEASE-APPROVAL-1
publication_count=2
evaluated_at_unix_seconds=150
batch_sha256=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
```

The hash above and time `150` are **illustrative placeholders**, not a usable
approval for real model files. There are exactly three keys, all mandatory.
`batch_sha256` is a 64-character lower-case SHA-256 hex digest. No checkpoint
path is embedded in the approval record. Its order is taken from the
independent, reviewed list passed to the verifier.

## Verify an approved release (Windows PowerShell)

These are *interactive build and verification commands*, not project
functionality implemented in a scripting language:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S .\framework\learning\native-llm `
  -B .\build\windows-ucrt64-native-llm-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON `
  -DUMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON
cmake --build .\build\windows-ucrt64-native-llm-debug --parallel 2

$verify = '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-release-approve.exe'
& $verify `
  'C:\umicom\approved-releases\candidate.approval' `
  'C:\umicom\trusted-publishers\publishers.trust' `
  'C:\umicom\trusted-models\baseline.umi' `
  'C:\umicom\trusted-models\candidate.umi'
if ($LASTEXITCODE -ne 0) { throw 'Release approval verification failed' }
```

Every checkpoint and its companion publication and signature records must
already exist. The approved file and policy must be obtained **independently**
of the release package being verified. Do not manufacture an expected digest
from the unreviewed files during the same release-gate step: that would make
the approval tautological.

## C23 integration and regression tests

The new public entry points are:

- `UmiAiTrainingReleaseApprovalParse` — strict, bounded, failure-atomic parse.
- `UmiAiTrainingReleaseApprovalLoadFile` — bounded local input and parse.
- `UmiAiTrainingReleaseApprovalVerifyJson` — all checks, exact digest pin,
  then a failure-atomic JSON result.

The existing trust-batch, publication, signing and checkpoint APIs and on-disk
formats remain unchanged. The new sources compile only when
`UMICOM_AI_TRAINING_ENABLE_SIGNATURES=ON`.

```powershell
ctest --test-dir .\build\windows-ucrt64-native-llm-debug `
  -R '^framework\.ai\.training\.release_approval_' `
  --output-on-failure --no-tests=error
```

**Scope:** These checks establish exact file identity, publisher trust and a
reviewer's pinned selection at a caller-selected time. They do not establish
training-data permissions, model competence, host integrity, trusted time,
secure approval-file distribution, or an atomic multi-file filesystem snapshot.
Any hostile concurrent file replacement, quarantined executable, or insecure
signing key requires an independent security response.
