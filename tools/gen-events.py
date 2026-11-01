#!/usr/bin/python3

"""
Generate `events.log` as JSON-lines
in current working dir
with no args.
"""

import json
import os
import random


if __name__ == "__main__":
    cwd = os.getcwd()
    print(f"Generate `events.log` in `{cwd}` (pseudo-random).")

    proc_range = ["powershell.exe", "smss.exe",
                  "csrss.exe", "wininit.exe",
                  "svchost.exe", "services.exe",
                  "explorer.exe", "taskhostw.exe",
                  "dllhost.exe", "cmd.exe",
                  "powershell.exe", "lsass.exe",
                  "winlogon.exe", "mstsc.exe",
                  "rundll32.exe", "wscript.exe",
                  "cscript.exe", "schtasks.exe",
                  "taskmgr.exe", "msedge.exe",
                  "wmplayer.exe", "wuauclt.exe",
                  "regedit.exe", "msiexec.exe",
                  "perfmon.exe", "mshta.exe",
                  "reg.exe", "certutil.exe",
                  "RuntimeBroker.exe"
                  ]

    pid_range = range(9999)
    ppid_range = range(9999)

    ts_start = 1725350010
    ts_range = range(ts_start, 1725450000)

    type_range = ["ProcessStarted", "FileWritten"]

    path_range = ["/tmp/payload.bin",
                  "/opt/openoffice/writer",
                  "/usr/bin/quake3",
                  "/usr/bin/ar",
                  "/usr/bin/meson",
                  "/usr/bin/zstd",
                  "/run/media/usb-stick/eicar",
                  "/root/q1/music/track02.ogg",
                  "/root/q1/music/track03.ogg",
                  "/root/q1/music/track04.ogg",
                  "/root/q1/music/track05.ogg",
                  "/root/q1/music/track06.ogg",
                  "/root/q1/music/track07.ogg",
                  "/root/q1/music/track08.ogg",
                  "/root/q1/music/track09.ogg",
                  "/root/q1/music/track10.ogg",
                  "/root/q1/music/track11.ogg",
                  "/root/q1/tracklist.cfg",
                  "/home/user/id/q3ta/pak0.pk3",
                  "/home/user/id/q3ta/pak1.pk3",
                  "/home/user/id/q3ta/baseq3/pak1.pk3",
                  "/home/user/id/q3ta/baseq3/pak2.pk3",
                  "/home/user/id/q3ta/baseq3/pak3.pk3",
                  "/home/user/id/q3ta/baseq3/pak4.pk3",
                  "/home/user/id/q3ta/baseq3/pak5.pk3",
                  "/home/user/id/q3ta/baseq3/pak6.pk3",
                  "/home/user/id/q3ta/baseq3/pak7.pk3",
                  "/home/user/id/q3ta/baseq3/pak8.pk3",
                  "/home/user/id/q3ta/missionpack/pak1.pk3",
                  "/home/user/id/q3ta/missionpack/pak2.pk3",
                  "/home/user/id/q3ta/missionpack/pak3.pk3",
                  "/usr/share/icons/skype.png"
                  ]

    # Sample data
    data = [
        {"type": "ProcessStarted", "pid": 1044, "ppid": 812,
            "name": "powershell.exe", "ts": 1725350000},
        {"type": "ProcessStarted", "pid": 1045, "ppid": 812,
            "name": "powershell.exe", "ts": 1725350002},
        {"type": "FileWritten", "path": "/tmp/payload.bin", "ts": 1725350005}
    ]

    with open('events.log', 'w') as f:
        for record in data:
            f.write(json.dumps(record) + '\n')

        for i in range(10001):
            event = {}
            event_type = random.choice(type_range)
            ts = ts_start + i

            if event_type == type_range[0]:
                pid = random.choice(pid_range)
                ppid = random.choice(ppid_range)
                name = random.choice(proc_range)
                event = {"type": event_type, "pid": pid,
                         "ppid": ppid, "name": name, "ts": ts}

            else:  # "FileWritten"
                path = random.choice(path_range)
                event = {"type": event_type, "path": path, "ts": ts}

            f.write(json.dumps(event) + '\n')
