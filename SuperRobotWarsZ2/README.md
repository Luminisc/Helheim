#### Installation
1. Visual Studio C++ Build Tools v142 (VS 2019)
2. Install `libpng-v142` from Nuget, to `png2bin` and `bin2png` projects

---

# Helheim - Super Robot Wars Z2
Used for handling game-related files for Z2: Saisei-hen (Second Super Robot Wars Z).

## [Note]
### [Packages]
Except for `EVENTBIN.BIN`, which has its own separate index file, the indices for all other package files are located in `EBOOT.BIN`.

No easy way around it—gather them all.

### [Font]
Located in `EBOOT.BIN`, at offset `0x1E6A74-0x32B274`.
- Tile: 32×8
- Length: 32×16
- Height: 8×(63+1)
- Format: 4bpp
- Encoding: 889F-9872

### [Text]
- `EVENTBIN.BIN`: Story text and victory condition text.
- `OP.BIN`: Character names and series title images in the opening.
- `SRVC.BIN`: Battle dialogue.
- `DATA000.BIN`: Opening text.
- `DATA014.BIN`: Story summaries.
- `DATA015.BIN`: MAP-DATA information.
- `DATA020.BIN`: Character text.
- `DATA021.BIN`: Mecha (unit) text.
- `EBOOT.BIN`: System text, save data text (even some images). No encoding file provided, so save text is stored directly in UTF-8.

### [Images]
Most images requiring editing are in:
- `Z2DATA.BIN`
- `VEFF2DX.BIN`
- `DATA013.BIN`

Most images are in GIM format, while some use a custom format—simple image data + palette data.

### [Compression Algorithm]
LZ encoding series. Interesting, but the original uses DP (Dynamic Programming). I didn't implement DP in my version—might refine it later.

### [Mugenkyoku (Infinite Prison) Mod]
Requires unpacking the mod's `EVENTBIN.BIN`, then using:
- The mod's `EBOOT.BIN` (renamed to `WBOOT.BIN`).
- A base, unmodified `EBOOT.BIN` (renamed to `BOOT.BIN`, included).

## [New]
**Ver 1.0**
- Fully completed.
- Added support for Mugenkyoku mod and BGM modification tools.

**Ver 0.7**
- Capable of handling most Z2: Saisei-hen localization tasks.

---
# (notes) Super Robot Wars Z2 Translation Tools

this appears to be a collection of tools for extracting, modifying, and reimporting game data for the PSP game "Super Robot Wars Z2" (超级机器人大战Z2). Here's a summary of each tool:

## Text Extraction/Insertion Tools

- **EBOOT_import.py**: Imports translated text and index information into the game's main executable (EBOOT.BIN)
- **GIFTTBL_dump.py/GIFTTBL_import.py**: Extracts and reimports text from GIFTTBL files containing gift/reward text
- **MTFL_dump.py/MTFL_import.py**: Handles extraction and insertion of text from files with "MTFLz2_2" headers
- **MTVZKN_dump.py/MTVZKN_import.py**: Works with encyclopedia/gallery text files for characters, robots, and keywords
- **OPERATE_dump.py/OPERATE_import.py**: Processes text from OPERATE files containing operation/mission information
- **SRVC_dump.py/SRVC_import.py**: Manages battle dialogue text in SRVC files
- **synopsis_dump.py/synopsis_import.py**: Extracts and inserts plot synopsis text
- **plus_import.py**: Special importer for handling extra-long text strings

## Binary File Handling Tools

- **bin_unpack/bin_pack**: Tools for unpacking and repacking general binary files
- **bin_unpack_idx/bin_pack_idx**: Similar tools that work with indexed binary files
- **EVENTBIN_unpack/EVENTBIN_pack**: Specifically for handling EVENTBIN.BIN containing story events and text

## Image Conversion Tools

- **bin2png/bin2png.c**: Converts the game's binary image data to PNG format for editing
- **png2bin/png2bin.c**: Converts edited PNG images back to the game's binary format
- **png.ini**: Configuration file defining parameters for various image files

## Support Tools

- **index_insert**: Tool for inserting index information into files
- **bgm_pack**: Tool for packing background music files

## Documentation

- **README.md**: Contains information about file formats, encoding, compression methods, and other technical details about the game's data

These tools appear to be comprehensive for translating and modifying most aspects of the game, including text, images, and possibly audio.