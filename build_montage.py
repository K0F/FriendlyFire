#!/usr/bin/env python3
import subprocess
import os
import tempfile

font = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
segments = [
    ("/mnt/data/Vasulkas/00000.mp4", 1.0, 11.0, "SANTA FE 2016 // SKLAD RODEO ROAD // HLEDÁNÍ SOUČÁSTEK"),
    ("/mnt/data/Vasulkas/00005.mp4", 4.0, 18.0, "DÍLNA RUFFINA STREET // MONTÁŽ HLINÍKOVÉHO RÁMU A PÍSTŮ"),
    ("/mnt/data/Vasulkas/00008.mp4", 5.0, 19.0, "OPTICKÁ STOLICE // KODAK CAROUSEL 35MM A ZRCADLA"),
    ("/mnt/data/Vasulkas/00013.mp4", 5.0, 19.0, "SYNCHRONIZACE CLON // PNEUMATICKÁ CHOREOGRAFIE"),
    ("/mnt/data/Vasulkas/00020.mp4", 2.0, 16.0, "OŽIVENÝ FRIENDLY FIRE // PŘED EXPEDICÍ DO BRNA")
]

out_dir = "/home/kof/src/friendlyfire/assets"
os.makedirs(out_dir, exist_ok=True)
tmp_files = []

for i, (src, start, end, label) in enumerate(segments):
    tmp_out = f"/tmp/ff_seg_{i}.mp4"
    tmp_files.append(tmp_out)
    dur = end - start
    # Draw lower-third box with label
    vf = (
        f"scale=1280:720,fps=25,"
        f"drawbox=y=ih-70:color=black@0.75:width=iw:height=70:t=fill,"
        f"drawtext=fontfile={font}:text='{label}':fontcolor=white:fontsize=22:x=30:y=h-48"
    )
    cmd = [
        "ffmpeg", "-y", "-ss", str(start), "-i", src, "-t", str(dur),
        "-vf", vf, "-c:v", "libx264", "-preset", "fast", "-crf", "22",
        "-c:a", "aac", "-b:a", "128k", "-ar", "44100", "-ac", "2",
        tmp_out
    ]
    print(f"Rendering segment {i}...")
    subprocess.run(cmd, check=True)

# Concat
concat_list = "/tmp/ff_concat.txt"
with open(concat_list, "w") as f:
    for t in tmp_files:
        f.write(f"file '{t}'\n")

final_out = os.path.join(out_dir, "friendly_fire_montage.mp4")
concat_cmd = [
    "ffmpeg", "-y", "-f", "concat", "-safe", "0", "-i", concat_list,
    "-c", "copy", final_out
]
print("Concatenating into final montage...")
subprocess.run(concat_cmd, check=True)

for t in tmp_files:
    if os.path.exists(t):
        os.remove(t)
if os.path.exists(concat_list):
    os.remove(concat_list)

print(f"Done! Created: {final_out}")
