all: wlr-say

CFLAGS=`pkg-config -cflags gtk+-3.0 wayland-client`
LDFLAGS=`pkg-config --libs gtk+-3.0 wayland-client`

.PHONY: proto clean compile_commands
proto: wlr-layer-shell-unstable-v1.c wlr-layer-shell-unstable-v1.h

wlr-layer-shell-unstable-v1.c:
	wayland-scanner private-code ./wlr-layer-shell-unstable-v1.xml ./wlr-layer-shell-unstable-v1.c

wlr-layer-shell-unstable-v1.h:
	wayland-scanner client-header ./wlr-layer-shell-unstable-v1.xml ./wlr-layer-shell-unstable-v1.h

wlr-say: proto
	$(CC) $(CFLAGS) $(LDFLAGS) -O3 *.c -o wlr-say

clean:
	rm -f wlr-say wlr-layer-shell-unstable-v1.c wlr-layer-shell-unstable-v1.h

compile_commands:
	echo '[' > compile_commands.json
	for file in *.c; do \
		echo '{ "directory": "'$(PWD)'", "command": "'$(CC)' '$(CFLAGS)' '$(LDFLAGS)' '$$file'", "file": "'$$file'" },' >> compile_commands.json; \
	done
	truncate -s -2 compile_commands.json
	echo ']' >> compile_commands.json
