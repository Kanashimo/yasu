all:
	meson compile -C build
	cd build && ./yasu
