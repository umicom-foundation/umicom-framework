# Federated authenticated frame contract

**Umicom Framework · Sammy Hegab · Umicom Foundation · MIT**

## Wire layout, schema 1

| Offset | Size | Meaning |
|---:|---:|---|
| 0 | 4 | ASCII `UMFA` magic |
| 4 | 2 | big-endian version `1` |
| 6 | 2 | zero reserved flags |
| 8 | 8 | nonzero key epoch, big-endian |
| 16 | 4 | length of the existing `UMFW` frame, big-endian |
| 20 | N | exact canonical `UMFW` bytes, including their own SHA-256 |
| 20+N | 32 | HMAC-SHA-256 of offsets `[0,20+N)` |

Total is `20 + N + 32` bytes. The permitted inner frame is `184 + 4*P + 32` bytes, where `P=0` for assignments and `1≤P≤4096` for results. The outer frame therefore ranges from 268 to 16,652 bytes.

The `UMFA` magic/version and a separately provisioned *per-worker and per-epoch key* separate this MAC from unrelated protocol messages. The key epoch is not a credential. The decoder rejects mismatched epochs and tags before publishing any decoded content.

## Correct publication order

Before a result is admitted, independently check: authenticated transport and worker registry; which key belongs to which worker and epoch; data-policy permissions; `UMFA` MAC; existing `UMFW` checksum/schema/finite numbers; exact round, sequence, dataset, checkpoint and contributor identity; existing process-local `FederatedJob` ticket; and the existing `FederatedRound` norms and aggregation rules. Maintain durable deduplication through the Framework's inbox/outbox when retry/restart semantics are needed. Never transmit a local ticket.

For queued messages, `AuthBindEnvelope` only returns a **borrowed** pointer. Use the existing `umi_message_copy` when the payload must survive its producer and verify it again at its final trust boundary.

## Demonstration versus assurance

HMAC protects integrity and possession of an authorised shared secret, not model quality or confidentiality. SHA-256 by itself is unkeyed and does not authenticate workers. The tests use a deterministic toy key for reproduction, not a secure provisioning scheme. This repository does not claim a LAN, WAN, TLS, signed-contribution chain, proof-of-training or ledger consensus implementation.
