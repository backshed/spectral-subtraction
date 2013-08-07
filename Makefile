MAKE=make

all: lred julius jss


lred:
	qmake libnoisered/libnoisered.pro -o build/libnoisered/release/Makefile
	$(MAKE) -C build/libnoisered/release/

julius:
	(export CC="gcc-4.8" && export CFLAGS="-O3 -march=native" && cd julius-4.2.3 && ./configure)
	$(MAKE) -C julius-4.2.3/
	$(MAKE) -C julius-4.2.3/julius -f MakefileJLS

jss:
	qmake julius_ss/JuliusSub/JuliusSub.pro -o build/juliusss/release/Makefile
	$(MAKE) -C build/juliusss/release

gui:
	qmake denoiseGUI/Interface.pro -o build/denoiseGUI/release/Makefile
	$(MAKE) -C build/denoiseGUI/release/

clean:
	-rm -rf build
	-rm -rf output/juliusSub
	-rm -rf output/libnoisered.a
	-rm -rf output/Interface
	-rm -rf output/libjls.a

	-$(MAKE) clean -C julius-4.2.3
	-$(MAKE) clean -C julius_ss
