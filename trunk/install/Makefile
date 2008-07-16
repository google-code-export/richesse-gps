#
# Create installation files for RichesseGPS
# Copyright (c) 2008, David Andrs
#

PRODUCT=RichesseGPS
VERSION=2.0

CABWIZ="c:/program files/windows ce tools/wce420/pocket pc 2003/tools/cabwiz.exe"
NSIS="c:/program files/nsis/makensis.exe"
ZIP="c:/program files/unxutils/usr/local/wbin/zip.exe"

all: cab

cab: $(PRODUCT).ARM.CAB

exe: $(PRODUCT)-$(VERSION).exe

$(PRODUCT).ARM.CAB: RichesseGPS.inf
	$(CABWIZ) RichesseGPS.inf /cpu ARM

zip: $(PRODUCT).ARM.CAB
	cp $(PRODUCT).ARM.CAB $(PRODUCT)-$(VERSION).ARM.cab
	$(ZIP) -m $(PRODUCT)-$(VERSION).zip $(PRODUCT)-$(VERSION).ARM.cab

$(PRODUCT)-$(VERSION).exe: setup.nsi $(PRODUCT).ARM.CAB
	$(NSIS) /DLANGUAGE=English /DVERSION=$(VERSION) setup.nsi 

dist:

clean:
	rm -f *.DAT
	rm -f *.CAB
	rm -f *.exe
	rm -f *.zip
