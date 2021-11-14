## Audio metadata (tag) support table
- **RG** means this program supports reading ReplayGain values from this type of metadata.
- **CV** means this program supports reading pictures (which are typically cover arts) from this type of metadata.
- Old metadata formats such as ID3v1 and RIFF are not listed in the table since they are too primitive to store ReplayGain and cover art.


|                        | Vorbis comment | ID3v2 tag | iTunes MP4 metadata | APEv2 tag |
|------------------------|:--------------:|:---------:|:-------------------:|:---------:|
| FLAC<sup>1</sup>       |     RG / CV    |  RG / CV  |                     |           |
| MPEG-4 (alac, aac/m4a) |                |           |       RG / CV       |           |
| MP3                    |                |  RG / CV  |                     |     RG    |
| Vorbis (ogg)           |     RG / CV    |           |                     |           |
| WAV                    |                |           |                     |           |

<sup>1</sup> FLAC files may store images by itself, which can also be read by this software.
&nbsp;
## Supported ReplayGain formats
- **Vorbis comment** & **APEv2 tag**: Values stored with the key
```
REPLAYGAIN_TRACK_GAIN
replaygain_track_gain
REPLAYGAIN_ALBUM_GAIN
replaygain_album_gain
```
are supported.

- **iTunes MP4 metadata**: Values stored with the key 
```
----:com.apple.iTunes:REPLAYGAIN_TRACK_GAIN
----:com.apple.iTunes:replaygain_track_gain
----:com.apple.iTunes:REPLAYGAIN_ALBUM_GAIN
----:com.apple.iTunes:replaygain_album_gain
```
are supported. 

- **ID3v2 tag**: There are too many ways to store ReplayGain values inside ID3v2. This program only supports ReplayGain values stored in `RVA2` frame as specified by [ID3v2 2.4.0](https://id3.org/id3v2.4.0-frames), the latest version of ID3v2.

## See also
- [ReplayGain - hydrogenaudio](https://wiki.hydrogenaud.io/index.php/ReplayGain)
- [ReplayGain 1.0 specification - hydrogenaudio](https://wiki.hydrogenaud.io/index.php?title=ReplayGain_1.0_specification)
- [ReplayGain 2.0 specification - hydrogenaudio](https://wiki.hydrogenaud.io/index.php?title=ReplayGain_2.0_specification)
- [ReplayGain legacy metadata formats - hydrogenaudio](https://wiki.hydrogenaud.io/index.php?title=ReplayGain_legacy_metadata_formats)
- [README of loudgain](https://github.com/Moonbase59/loudgain)