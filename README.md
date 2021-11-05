# Page Languages

- **US English**
- [日本語](README_ja.md)
- [简体中文](README_zh-CN.md) - [繁體中文（香港）](README_zh-HK.md) - [繁體中文（臺灣）](README_zh-TW.md)

---

# ThousandPlayer
A Qt6-based open-source audio player developed from scratch. It is a tribute to [TTPlayer（千千静听）](https://zh.wikipedia.org/wiki/千千音乐播放器),
one of the most popular local music players for Microsoft Windows in China during 2000s.

The software is still in early development stage,
but already has the basic functions that a local audio player should have. Feel free to paticipate in the development!

## Features
- Cross-platform.
  - Linux and Microsoft Windows 10 are supported at present. Need other developers' help to check the availability in macOS.
- Most UI elements have text with proper font sizes instead of icon-only for the sake of localizability. Current supported UI languages are:
  - English
  - 日本語
  - 简体中文
  - 繁體中文（香港）
  - 繁體中文（臺灣）
- Currently supports following audio formats:
  - WAV (`.wav`)
  - FLAC (`.flac`)
  - MP3 (`.mp3`)
  - AAC (`.aac`, `.m4a`)
  - Vorbis (`.ogg`)
- A horizontally expandable main window. No need to worry about long music informations.
- A cover viewer for viewing album covers.
- Rich finding and sorting functions.
- Standard ReplayGain support with pre-amplification and default ReplayGain.
  - Reading of ReplayGain is depending on **TagLib** and thus cannot support all tag-format combinations. Check the [ReplayGain support list](ReplayGain.md) for details.
- Customizable fonts of audio infomation label and playlist.
- Multi-threaded file loading.

## Planned features
- Playlist persistence and multi-playlist support.
- Viewer and editor for `.lrc` lyrics files.
- Audio format converting function.
- Music tag editor.
- ReplayGain scanning function.
- `.cue` sheet support.

# Compilation Tutorial
