ImageFiltering: main.c
	$(CC) main.c iplib2New.c linkedlist.c utils.c -o images -lm

# Sorber: sorber.c
# 	$(CC) sorber.c linkedlist.c -o sorber -lm