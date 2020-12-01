ImageFiltering: main.c
	$(CC) main.c iplib2New.c thpool.c utils.c queue.c -o images -lm -pthread