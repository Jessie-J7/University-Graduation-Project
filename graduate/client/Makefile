SOURCE:=$(wildcard *.c)
client:${SOURCE}
	gcc $^ -g  -o $@ -lcrypt -lcrypto
clean:
	rm client
