ImageFiltering: main.c
	$(CC) main.c iplib2New.c thpool.c linkedlist.c utils.c -o images -lm -pthread

# Sorber: sorber.c
# 	$(CC) sorber.c thpool.c linkedlist.c -o sorber -lm -pthread

# Old: old.c
# 	$(CC) old.c iplib2New.c -o old -lm