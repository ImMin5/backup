ssu_backup : main.o ssu_link.o ssu_add.o ssu_compare.o ssu_recover.o ssu_remove.o
	gcc -o ssu_backup main.o ssu_link.o ssu_add.o ssu_compare.o ssu_recover.o ssu_remove.o -lpthread
main.o : main.c
	gcc -c -Wall -W -o main.o main.c 
ssu_add.o : ssu_add.c
	gcc -c -Wall -W -o ssu_add.o ssu_add.c
ssu_link.o : ssu_link.c
	gcc -c -Wall -W -o ssu_link.o ssu_link.c
ssu_compare.o : ssu_compare.c
	gcc -c -Wall -W -o ssu_compare.o ssu_compare.c
ssu_recover.o : ssu_recover.c
	gcc -c -Wall -W -o ssu_recover.o ssu_recover.c
ssu_remove.o : ssu_remove.c
	gcc -c -Wall -W -o ssu_remove.o ssu_remove.c
gdb : main.c ssu_link.c ssu_add.c ssu_compare.c ssu_recover.c ssu_remove.c
	gcc -Wall -W -g main.c ssu_link.c ssu_add.c ssu_compare.c ssu_recover.c ssu_remove.c -o test -lpthread
run : 
	./ssu_backup
clean :
	rm backup_log.log
