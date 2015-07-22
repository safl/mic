all: clean miclord voodoo runner

miclord: libmiclord.c
	icc -openmp -fPIC -shared -std=c99 libmiclord.c -o lib/libmiclord.so

voodoo: libvoodoo.c
	icc -openmp -fPIC -shared -std=c99 libvoodoo.c -o lib/libvoodoo.so

runner: runner.c
	icc -I./ -lmiclord -openmp -std=c99 runner.c -o bin/runner

clean:
	rm -f bin/runner
	rm -f lib/*.so
