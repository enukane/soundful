all: test-wav test-wav2

test-wav: test-wav.c
	$(CC) $(CFLAGS) -o $@ $<
test-wav2: test-wav2.c
	$(CC) $(CFLAGS) -o $@ $<


clean: test-wav
	rm $<
