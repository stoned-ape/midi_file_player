all: midi_player.exe

midi_player.exe: main.cpp makefile
	cl -Wall /EHsc /O2 /fp:fast /std:c++20 main.cpp /link /out:midi_player.exe
# 	nvcc -O3 -std=c++17 main.cpp -o midi_player.exe
# 	clang-cl -Wall /O2 /fp:fast /std:c++20 main.cpp /link /out:midi_player.exe

run: midi_player.exe  
	midi_player.exe spring.mid
# 	midi_player.exe minuet_in_g.mid
# 	midi_player.exe gnossienne.mid
# 	midi_player.exe fur_elise.mid
# 	midi_player.exe cannon_in_c.mid
