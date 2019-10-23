CC = gcc
PACKAGES = x11
CFLAGS = -g -Wall -Wextra -pedantic -Wno-deprecated-declarations $(shell pkg-config $(PACKAGES) --cflags --libs)
ifdef SANATIZE_ADDRESS
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif
BINARY = meow

all: $(BINARY)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINARY): $(wildcard *.c)
	$(CC) $(CFLAGS) -o $@ $^

install: all
	install -Dm 755 meow /usr/local/bin/meow
	install -Dm 644 meow.desktop /usr/share/xsessions/meow.desktop

.PHONY: uninstall
uninstall:
	$(RM) /usr/local/bin/meow
	$(RM) /usr/share/xsessions/meow.desktop

.PHONY: clean

clean:
	$(RM) */*.o *.o $(BINARY)
