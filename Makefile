.PHONY: all debug

all:
	meson compile -C build
	cd build && ./yasu

debug:
	meson compile -C build
	cd build && gdb ./yasu
