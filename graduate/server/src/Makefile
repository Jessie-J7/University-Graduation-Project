SOURCE:=$(wildcard *.c)
fileserver:${SOURCE}
	gcc $^ -g  -o ../bin/$@ -Lusr/lib/mysql -lmysqlclient  -lpthread -lcrypto
clean:
	rm thread_pool
