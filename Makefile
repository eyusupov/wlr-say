all: wlr-say

CFLAGS=`pkg-config -cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0` `pkg-config --libs wayland-client`

.PHONY: proto clean
proto: wlr-layer-shell-unstable-v1.c wlr-layer-shell-unstable-v1.h

wlr-layer-shell-unstable-v1.c:
	wayland-scanner private-code ./wlr-layer-shell-unstable-v1.xml ./wlr-layer-shell-unstable-v1.c

wlr-layer-shell-unstable-v1.h:
	wayland-scanner client-header ./wlr-layer-shell-unstable-v1.xml ./wlr-layer-shell-unstable-v1.h

wlr-say: proto
	$(CC) $(CFLAGS) $(LDFLAGS) *.c -o wlr-say

clean:
	rm -f wlr-say wlr-layer-shell-unstable-v1.c wlr-layer-shell-unstable-v1.h
