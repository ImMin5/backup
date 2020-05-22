#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>

#define MAX_FILE_NUM 100
#define BUFSIZE 256
#define CMDNUM 9
#define PERIOD_MIN 5
#define PERIOD_MAX 10
#define OPTION_NUM 4
#define USERID "20142306"
#define BACKUPFILE_NAME "20142306_backup"
#define LOGFILE_NAME "backup_log.log"
struct User
{
	char id[BUFSIZE];
};

struct CMD_LIST
{
	char cmd[BUFSIZE];
	char filename1[BUFSIZE];
	char filename2[BUFSIZE];
	int pri;
	char option[BUFSIZE];
};

struct Backup_info {
	char pathname[BUFSIZE];
	char option[OPTION_NUM][BUFSIZE];
	int stop;
	int period;
	int time;
	int number;
	char flag[4]; //0:m 1:n 2:t 3:d
	char flag_m;
	char flag_n;
	char flag_t;
	char flag_d;
	pthread_t tid;
};

struct Node {
	struct Node *next;
	struct Node *prev;
	struct Backup_info *backup_info;
};

struct List {
	struct Node *tail;
	struct Node *head;
	int index;
};

char cmd[CMDNUM][BUFSIZE];
char backupdir[BUFSIZE];
struct List *list;
int backup_count;
int cmd_count;
int status;

pthread_t main_tid;
FILE *fp_log;

//<--------------------------main.c------------------------->
void token_cmd(char *); //프로그램 실행후 입력한 인자를 공백별로 나눠서 배열에 저장하는 함수
void *ssu_prompt(); 
void make_backupdir(char *,DIR *);
void log_msg(char *, char*);
void makelog_msg(char *,char *, int);
void ls_vim();
void Exit(pthread_t);
void clear_stdin();
void clear_prompt();
void make_backup_file(char *,char *);
int space_in_name(char *);

//<--------------------------ssu_add.c------------------------->
void ssu_add();
void *ssu_add_backup(void *);
void check_add_option(struct Backup_info *);
int check_int_float(char *);
int is_add_option(char *);
int option_location(char *);
void add_option_n(struct Backup_info *);
void add_option_t(struct Backup_info *);
void add_option_d(struct Backup_info *);
int make_option_t_list(const struct dirent *);
int make_option_n_list(const struct dirent *);
int make_option_d_list(const struct dirent *);
//<--------------------------ssu_link.c------------------------->
void link_init();
void link_add(struct Backup_info *);
int link_remove(char*a,int);
void link_remove_all();
struct Node* link_search(char *);
void link_print();
void link_free();
void link_exit();

//<---------------------------ssu_compare.c------------------------>
void ssu_compare(char *, char *);

//<---------------------------ssu_recover.c------------------------->
void ssu_recover(char *);
int make_recover_list(const struct dirent *);

//<---------------------------ssu_reomve.c------------------------->
void ssu_remove(char *);
