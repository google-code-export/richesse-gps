#
# Create installation files for RichesseGPS
# Copyright (c) 2008, David Andrs
#

PRODUCT=RichesseGPS
VERSION=2.0

CABWIZ="wine c:\\program files\\windows ce tools\\wce420\\pocket pc 2003\\tools\\cabwiz.exe"
ZIP="c:/program files/unxutils/usr/local/wbin/zip.exe"

all: cab

cab: $(PRODUCT).ARM.CAB

$(PRODUCT).ARM.CAB: RichesseGPS.inf
	$(CABWIZ) RichesseGPS.inf /cpu ARM

zip: $(PRODUCT).ARM.CAB
	cp $(PRODUCT).ARM.CAB $(PRODUCT)-$(VERSION).ARM.cab
#	$(ZIP) -m $(PRODUCT)-$(VERSION).zip $(PRODUCT)-$(VERSION).ARM.cab

dist:

clean:
	rm -f *.DAT
	rm -f *.CAB
	rm -f *.zip
