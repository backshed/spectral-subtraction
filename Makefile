MAKE=make -j4

all:
	qmake libnoisered/libnoisered.pro -o build/libnoisered/release/Makefile
	$(MAKE) -C build/libnoisered/release/
	qmake denoiseGUI/Interface.pro -o build/denoiseGUI/release/Makefile
	$(MAKE) -C build/denoiseGUI/release/

	(export CC="gcc-4.8" && export CFLAGS="-O3 -march=native" && cd julius-4.2.3 && ./configure)
	$(MAKE) -C julius-4.2.3/
	$(MAKE) -C julius-4.2.3/julius
	$(MAKE) -C julius_ss

clean:
	-rm -rf build
	-rm -rf output/juliusSub
	-rm -rf output/libnoisered.a
	-rm -rf output/Interface
	-$(MAKE) clean -C julius-4.2.3
	-$(MAKE) clean -C julius_ss
