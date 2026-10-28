# Canonical educational audit-record contract

**Umicom Foundation · Sammy Hegab · MIT**

All integer fields are big-endian and independent of host structure padding.

| Byte offset | Width | Field |
|---:|---:|---|
| 0 | 8 | Research round identifier (positive) |
| 8 | 8 | Worker-scoped sequence (positive) |
| 16 | 8 | Declared distinct sample count |
| 24 | 4 | Outcome: accepted=1, rejected=2, quarantined=3 |
| 28 | 4 | Reserved, must equal zero |
| 32 | 32 | Contributor identifier digest |
| 64 | 32 | Base checkpoint SHA-256 |
| 96 | 32 | Dataset commitment SHA-256 |
| 128 | 32 | Policy SHA-256 |
| 160 | 32 | Update commitment SHA-256 |
| 192 | 32 | Evaluation/evidence commitment SHA-256 |

**Total payload:** 224 bytes. The record-chain component separately commits payload length, record kind, sequence, history identifier and predecessor hash. The record kind is the four-byte tag `AIFD`.

All six digests must be nonzero. These checks validate canonical structure, not the truth of any assertion. Accepted decisions require a positive sample count, but no claim is made that those samples were legally sourced or trained.

The new adapter delegates hashing, chaining and batch verification to `umicom/record_chain/chain.h`; there is no independent cryptographic implementation, ledger consensus or network transaction replay policy. A privately held final checkpoint (or separately authenticated trusted publication) is essential for detecting replacement/truncation of an entire history.
