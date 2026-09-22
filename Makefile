.PHONY: all debug setup

all:
	meson compile -C build
	cd build && ./yasu

debug:
	meson compile -C build
	cd build && gdb ./yasu

setup:
	meson setup build
