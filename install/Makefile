#
# Create installation files for RichesseGPS
# Copyright (c) 2008, David Andrs
#

PRODUCT=RichesseGPS
VERSION=2.1

# On linux
CABWIZ=wine z:\\opt\\wince-arm\\wce400\\tools\\cabwiz.exe
ZIP=zip

# On windows
#CABWIZ="c:/program files/windows ce tools/wce420/pocket pc 2003/tools/cabwiz.exe"
#ZIP="c:/program files/unxutils/usr/local/wbin/zip.exe"

all: cab

cab: $(PRODUCT)-$(VERSION).ARM.CAB

$(PRODUCT)-$(VERSION).ARM.CAB: RichesseGPS.inf
	$(CABWIZ) RichesseGPS.inf /CPU ARM
	$(RM) $(PRODUCT).ARM.DAT
	mv $(PRODUCT).ARM.CAB $(PRODUCT)-$(VERSION).ARM.CAB

zip: $(PRODUCT)-$(VERSION).ARM.CAB
	$(ZIP) $(PRODUCT)-$(VERSION).zip $(PRODUCT)-$(VERSION).ARM.CAB

clean:
	rm -f *.DAT
	rm -f *.CAB
	rm -f *.zip
