# Screenshots and demo video

## Current assets

- `hero.jpg` — the still referenced by the top-level `README.md`: the closing
  frame of the recording, at 00:38.6, showing the finished build with the dug
  trench in the foreground. GitHub uses an image, never the video, for link
  previews, and it is what readers see if the video fails to load.
- `../demo.mp4` — 30.3 s, 1920×1080 @ 60 fps, 8.67 MiB. **Deliberately
  gitignored**: GitHub only plays videos uploaded as issue or pull request
  attachments, so a committed file would render as a dead link and bloat the
  repository.

## Embedding the video

Drag `Docs/demo.mp4` into any issue or pull request comment box on this repo and
let GitHub upload it. Copy the resulting
`https://github.com/user-attachments/assets/…` URL and paste it into the main
`README.md` on a line of its own, at the marked slot. No markdown syntax around
it. Videos are capped at **10 MB** on a free plan.

## Rebuilding the video from a new recording

Find where the recording actually ends — a screen recorder usually leaves a
second or two of frozen frame after the game exits. Measure motion rather than
brightness:

```sh
ffmpeg -ss 37 -i raw.mp4 -vf "fps=10,scale=320:-2" /tmp/m/%03d.png
# then diff consecutive frames; motion stops where the difference flatlines
```

Trim, then hit a size target with a two-pass encode:

```sh
ffmpeg -ss 9 -to 39.3 -i raw.mp4 -an \
  -c:v libx264 -preset veryfast -crf 16 -pix_fmt yuv420p cut.mp4

# bitrate = target_bytes * 8 / seconds; 8.5 MiB over 30.3 s is about 2350k
ffmpeg -i cut.mp4 -c:v libx264 -preset slow -b:v 2350k -pass 1 -an -f null /dev/null
ffmpeg -i cut.mp4 -c:v libx264 -preset slow -b:v 2350k -pass 2 \
  -pix_fmt yuv420p -movflags +faststart -an demo.mp4
```

To join two non-adjacent segments, cut each and dissolve between them so the
jump reads as deliberate — `offset` is the first clip's length minus the
transition:

```sh
ffmpeg -i a.mp4 -i b.mp4 \
  -filter_complex "[0:v][1:v]xfade=transition=fade:duration=0.5:offset=15.5,format=yuv420p[v]" \
  -map "[v]" -c:v libx264 -preset slow -crf 18 -an cut.mp4
```

Notes that cost time to work out:

- **Two-pass at a target beats guessing at `-crf`.** Constant quality on fast
  camera motion overshot the cap by 70 %.
- **Do not downscale to 720p to save space.** Encoded to the same file size,
  1080p came out sharper at both 3150k and 2350k — the flat shading compresses
  cheaply, so scaling down discards detail x264 would have kept for free.
  Halving to 30 fps bought no visible sharpness either, and costs smoothness.
- `-pix_fmt yuv420p` is required or browsers will not decode it, `-an` drops the
  empty audio track, and `-movflags +faststart` lets playback begin before the
  whole file has downloaded.

Pull the still straight out of the recording, then convert it — a 1080p frame
of this render is about 58 000 unique colours after the downscale, so PNG lands
around 1.2 MB while a 4:4:4 JPEG is 500 KB and indistinguishable at 400 % zoom:

```sh
ffmpeg -ss 00:00:38.6 -i raw.mp4 -frames:v 1 -q:v 2 hero.png
magick hero.png -quality 95 -sampling-factor 1x1 -strip hero.jpg
```

`-sampling-factor 1x1` disables chroma subsampling, which is what keeps the HUD
text and the crosshair crisp. Do **not** try to shrink the PNG by quantizing
with dithering — the dither noise defeats PNG compression and more than doubles
the file.

## If you record again

- **The MangoHud bar is deliberate** — it is there to show the frame rate. Note
  it does overlap the engine's own F3 debug menu; nudge one of them if you want
  both readable.
- **Press `X`** to dismiss the controls overlay once you have shown it.
- **Turn vsync on** — `Window::SetVSync(true)` in `main.cpp`. Uncapped, the
  engine renders a few thousand frames a second that the compositor never
  samples; vsync gives evenly paced frames and a much quieter GPU.
- **Record at native resolution and downscale afterwards.** The engine has no
  antialiasing, so downscaling supersamples the voxel edges.
