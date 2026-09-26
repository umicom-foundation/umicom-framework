"""Umicom Framework | Sammy Hegab, Umicom Foundation | MIT.
Independent arithmetic, JSON and read-only Linux capture checks. Fixture input
is explicitly marked; no synthetic result is counted as a live guest boot.
"""
from __future__ import annotations
import json
import os
from pathlib import Path
import random
import subprocess
import sys
import tempfile

checks = 0
def check(value: bool, message: str) -> None:
    global checks
    checks += 1
    if not value:
        raise AssertionError(message)
def execute(args: list[str], **kwargs: object) -> subprocess.CompletedProcess[str]:
    return subprocess.run(args, text=True, capture_output=True, timeout=20, check=True, **kwargs)
def stat_text(pid: int, name: str = "Umicom Notes", rss: int = 3) -> str:
    fields = [str(n) for n in range(4, 25)]
    fields[0] = "1"
    fields[22 - 4] = "777"
    fields[24 - 4] = str(rss)
    return f"{pid} ({name}) S " + " ".join(fields) + "\n"
def main() -> None:
    test, report = map(str, map(Path, sys.argv[1:3]))
    sample = json.loads(execute([test, "json-output"]).stdout)
    check(sample["source"] == 'Source "quoted"\n\x1b /é', "Unicode and control round trip")
    check(sample["processes"]["items"][0]["pid"] == str(2**64 - 1), "64-bit precision")
    check(sample["processes"]["items"][0]["residentBytes"] is None, "unknown RSS remains null")
    generator = random.Random(200926)
    ratios = [(0, 0), (0, 1), (1, 1), (2**64-2, 2**64-1)]
    for _ in range(2048):
        total = generator.randrange(1, 2**64)
        ratios.append((generator.randrange(total + 1), total))
    result = execute([test, "ratio-stream"], input="".join(f"{a} {b}\n" for a, b in ratios)).stdout.splitlines()
    check(len(result) == len(ratios), "all ratios returned")
    for (part, total), row in zip(ratios, result):
        status, percent = map(int, row.split())
        check((status, percent) == ((0, part * 10000 // total) if total else (10, 777)), "independent integer ratio")
    live = json.loads(execute([report]).stdout)
    check(not live["fixture"], "native capture is not fixture")
    if sys.platform != "linux":
        print(f"{checks} checks; Linux fixtures not applicable on {sys.platform}")
        return
    check(live["memory"]["status"] == 0 and int(live["memory"]["totalBytes"]) > 0, "live memory")
    check(live["cpu"]["status"] == 0 and len(live["cpu"]["ticks"]) == 8, "live CPU")
    check(live["storage"]["status"] == 0, "live filesystem")
    total = next(int(line.split()[1]) * 1024 for line in Path("/proc/meminfo").read_text().splitlines() if line.startswith("MemTotal:"))
    check(int(live["memory"]["totalBytes"]) == total, "native memory total agrees")
    check(live["processes"]["listed"] > 0, "native process enumeration")
    with tempfile.TemporaryDirectory(prefix="umicom-monitor-") as directory:
        root = Path(directory); proc = root / "proc-fixture"; proc.mkdir(); (proc / "net").mkdir()
        (proc / "meminfo").write_text("MemTotal: 1000 kB\nMemAvailable: 400 kB\n")
        (proc / "stat").write_text("cpu 10 20 30 40 50 60 70 80 900 901\n")
        header = "Inter-| Receive | Transmit\n face |bytes packets errs drop fifo frame compressed multicast|bytes packets errs drop fifo colls carrier compressed\n"
        (proc / "net/dev").write_text(header + " eth0: 1000 2 3 4 5 6 7 8 9000 10 11 12 13 14 15 16\n")
        for pid in (100, 40, 75):
            (proc / str(pid)).mkdir(); (proc / str(pid) / "stat").write_text(stat_text(pid))
        boot = root / "boot.report"
        boot.write_text("UMICOM_BOOT_REPORT 1\nmode=normal\nstate=ready\nplanned=2\ncompleted=2\nreason=none\nsource=" + "a"*64 + "\n")
        def capture() -> dict:
            return json.loads(execute([report, "--fixture-proc", str(proc), "--storage", str(root), "--boot-report", str(boot)]).stdout)
        snap = capture()
        check(snap["fixture"] and snap["source"] == str(proc), "fixture source identified")
        check(snap["memory"]["totalBytes"] == "1024000" and snap["memory"]["availableBytes"] == "409600", "memory values")
        check([p["pid"] for p in snap["processes"]["items"]] == ["40", "75", "100"], "sorted PID list")
        check(all(p["startTicks"] == "777" for p in snap["processes"]["items"]), "start identities")
        check(snap["network"]["items"][0]["receivedBytes"] == "1000" and snap["network"]["items"][0]["transmittedBytes"] == "9000", "network columns")
        check(snap["boot"]["status"] == 0 and snap["boot"]["state"] == "ready", "canonical boot reader used")
        (proc / "meminfo").write_text("MemTotal: 1000 kB\n")
        snap = capture(); check(snap["memory"]["status"] != 0 and snap["memory"]["totalBytes"] == "0", "failed source does not retain stale value")
        (proc / "meminfo").unlink(); (proc / "meminfo").symlink_to("/proc/meminfo")
        check(capture()["memory"]["status"] != 0, "symlink read refused")
        (proc / "meminfo").unlink(); os.mkfifo(proc / "meminfo")
        check(capture()["memory"]["status"] != 0, "FIFO read refused without blocking")
        (proc / "75/stat").write_text(stat_text(999))
        snap = capture(); check(snap["processes"]["unreadable"] == 1 and snap["processes"]["listed"] == 2, "PID mismatch cannot impersonate another record")
        for pid in range(1000, 1200):
            (proc / str(pid)).mkdir(); (proc / str(pid) / "stat").write_text(stat_text(pid))
        snap = capture(); check(snap["processes"]["listed"] == 128 and snap["processes"]["status"] == 3, "bounded process list")
        check(snap["processes"]["seen"] == 203 and snap["processes"]["unreadable"] == 1, "observed and unreadable counts")
        check([int(p["pid"]) for p in snap["processes"]["items"]] == [40,100]+list(range(1000,1126)), "lowest PIDs retained")
        (proc / "net/dev").write_text(header + "".join(f" e{i}: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16\n" for i in range(33)))
        snap = capture(); check(snap["network"]["listed"] == 32 and snap["network"]["seen"] == 33 and snap["network"]["status"] == 3, "bounded interface list")
        (proc / "net/dev").write_text(header + "invalid\n")
        snap = capture(); check(snap["network"]["status"] != 0 and snap["network"]["listed"] == 0, "bad interface input fails visibly")
        boot.write_text("UMICOM_BOOT_REPORT 1\nstate=ready\n")
        snap = capture(); check(snap["boot"]["status"] != 0 and snap["boot"]["state"] is None, "invalid boot report is not ready")
        boot.write_bytes(b"x" * 513)
        check(capture()["boot"]["status"] == 3, "boot byte cap")
    print(f"{checks} independent checks passed, including {len(ratios)} arithmetic comparisons; native Linux capture and labelled proc fixtures.")
if __name__ == "__main__":
    main()
