# MIDI File Player

This program plays midi files using the Direct Sound Windows API.

## Build Instructions

run `nmake` or `cl -Wall /EHsc /O2 /fp:fast /std:c++20 main.cpp /link /out:midi_player.exe`
in a visual studio developer command prompt

## Usage

play one of the included midi files

```bat
midi_player.exe spring.mid
midi_player.exe minuet_in_g.mid
midi_player.exe gnossienne.mid
midi_player.exe fur_elise.mid
midi_player.exe cannon_in_c.mid
```