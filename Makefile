#
# RichesseGPS
#

all:
	$(MAKE) -C richesseGPS
	$(MAKE) -C setup

clean:
	$(MAKE) -C richesseGPS clean
	$(MAKE) -C setup clean
	$(MAKE) -C install clean

#
# Make distribution package
#
dist:
	$(MAKE) -C install

