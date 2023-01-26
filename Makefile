#OBJS : ApplicationManager.o

app : ApplicationManager.o
	gcc -o app ApplicationManager.o

ApplicationManager.o: ApplicationManager.c
	gcc -c ApplicationManager.c

clean :
	rm app $(OBJS)