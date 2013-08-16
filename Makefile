MAKE=make
JULIUS_URL="http://sourceforge.jp/frs/redir.php?m=jaist&f=%2Fjulius%2F59049%2Fjulius-4.2.3.tar.gz"
JULIUS_NAME=julius-4.2.3
all: lred julius jss


lred:
	qmake libnoisered/libnoisered.pro -o build/libnoisered/release/Makefile
	$(MAKE) -C build/libnoisered/release/

juliusinit:
	wget $(JULIUS_URL)
	tar -zxvf $(JULIUS_NAME).tar.gz
	(export CC="gcc-4.8" && export CFLAGS="-O3 -march=native" && cd $(JULIUS_NAME) && ./configure)


julius:
	cp julius_patch/* $(JULIUS_NAME)/julius/
	$(MAKE) -C $(JULIUS_NAME)/
	$(MAKE) -C $(JULIUS_NAME)/julius/ -f MakefileJLS

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
