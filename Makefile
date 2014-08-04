gcc-4.2.1.tar.gz:
	./download-gcc

download-gcc: tmp/gcc-4.2.1.tar.gz

build-bliss: build/gcc/bliss/bli1

build/gcc/bliss/bli1:
	./make-bliss

