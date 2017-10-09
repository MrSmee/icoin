Release Process
====================

* * *

###update (commit) version in sources


	icoin-qt.pro
	contrib/verifysfbinaries/verify.sh
	doc/README*
	share/setup.nsi
	src/clientversion.h (change CLIENT_VERSION_IS_RELEASE to true)

###tag version in git

	git tag -s v1.0.0

###write release notes. git shortlog helps a lot, for example:

	git shortlog --no-merges v0.8.7..v1.0.0

* * *

##perform gitian builds

 From a directory containing the icoin source, gitian-builder and gitian.sigs
  
	export SIGNER=(your gitian key, ie bluematt, sipa, etc)
	export VERSION=1.0.0
	cd ./gitian-builder

 Fetch and build inputs: (first time, or when dependency versions change)

	mkdir -p inputs; cd inputs/
	wget 'http://miniupnp.free.fr/files/download.php?file=miniupnpc-2.0.20170509.tar.gz' -O miniupnpc-2.0.20170509.tar.gz'
	wget 'https://www.openssl.org/source/openssl-1.0.2l.tar.gz'
	wget 'http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz'
	wget 'http://zlib.net/zlib-1.2.11.tar.gz'
	wget 'ftp://ftp.simplesystems.org/pub/libpng/png/src/history/libpng16/libpng-1.6.32.tar.gz'
	wget 'https://fukuchi.org/works/qrencode/qrencode-3.4.4.tar.bz2'
	wget 'http://downloads.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.bz2'
	wget 'http://download.qt.io/official_releases/qt/4.8/4.8.7/qt-everywhere-opensource-src-4.8.7.tar.gz'
	cd ..
	./bin/gbuild ../icoin/contrib/gitian-descriptors/boost-win32.yml
	mv build/out/boost-*.zip inputs/
	./bin/gbuild ../icoin/contrib/gitian-descriptors/deps-win32.yml
	mv build/out/icoin*.zip inputs/
	./bin/gbuild ../icoin/contrib/gitian-descriptors/qt-win32.yml
	mv build/out/qt*.zip inputs/

 Build icoind and icoin-qt on Linux32, Linux64, and Win32:
  
	./bin/gbuild --commit icoin=v${VERSION} ../icoin/contrib/gitian-descriptors/gitian.yml
	./bin/gsign --signer $SIGNER --release ${VERSION} --destination ../gitian.sigs/ ../icoin/contrib/gitian-descriptors/gitian.yml
	pushd build/out
	zip -r icoin-${VERSION}-linux.zip *
	mv icoin-${VERSION}-linux.zip ../../
	popd
	./bin/gbuild --commit icoin=v${VERSION} ../icoin/contrib/gitian-descriptors/gitian-win32.yml
	./bin/gsign --signer $SIGNER --release ${VERSION}-win32 --destination ../gitian.sigs/ ../icoin/contrib/gitian-descriptors/gitian-win32.yml
	pushd build/out
	zip -r icoin-${VERSION}-win32.zip *
	mv icoin-${VERSION}-win32.zip ../../
	popd

  Build output expected:

  1. linux 32-bit and 64-bit binaries + source (icoin-${VERSION}-linux.zip)
  2. windows 32-bit binary, installer + source (icoin-${VERSION}-win32.zip)
  3. Gitian signatures (in gitian.sigs/${VERSION}[-win32]/(your gitian key)/

repackage gitian builds for release as stand-alone zip/tar/installer exe

**Linux .tar.gz:**

	unzip icoin-${VERSION}-linux.zip -d icoin-${VERSION}-linux
	tar czvf icoin-${VERSION}-linux.tar.gz icoin-${VERSION}-linux
	rm -rf icoin-${VERSION}-linux

**Windows .zip and setup.exe:**

	unzip icoin-${VERSION}-win32.zip -d icoin-${VERSION}-win32
	mv icoin-${VERSION}-win32/icoin-*-setup.exe .
	zip -r icoin-${VERSION}-win32.zip icoin-${VERSION}-win32
	rm -rf icoin-${VERSION}-win32

**Perform Mac build:**

  OSX binaries are created on a dedicated 64-bit, OSX 10.12.6 machine.
  iCoin 1.0.x is built with Homebrew.

	qmake RELEASE=1 USE_UPNP=1 USE_QRCODE=1
	make
	export QTDIR=/opt/local/libexec/qt5  # needed to find translations/qt_*.qm files
	T=$(contrib/qt_translations.py $QTDIR/translations src/qt/locale)
	python2.7 share/qt/clean_mac_info_plist.py
	python2.7 contrib/macdeploy/macdeployqtplus iCoin-Qt.app -add-qt-tr $T -dmg -fancy contrib/macdeploy/fancy.plist

 Build output expected: iCoin-Qt.dmg

###Next steps:

* Code-sign Windows -setup.exe (in a Windows virtual machine) and
  OSX iCoin-Qt.app (Note: only Gavin has the code-signing keys currently)

* update icoin.world version
  make sure all OS download links go to the right versions

* update forum version

* update wiki download links

Commit your signature to gitian.sigs:

	pushd gitian.sigs
	git add ${VERSION}/${SIGNER}
	git add ${VERSION}-win32/${SIGNER}
	git commit -a
	git push  # Assuming you can push to the gitian.sigs tree
	popd

