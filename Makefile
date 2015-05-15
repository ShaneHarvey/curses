LIBS=-lncurses
BINS=clife
CFLAGS=-Wall -Werror

%: %.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

all: $(BINS)

clean:
	@rm -f $(BINS)
