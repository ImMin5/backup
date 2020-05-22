#include "ssu_backup.h"

int flag_no;
extern pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
	DIR *dp;
	struct stat file_info;

	if(argc > 2) {
		fprintf(stderr,"usage : %s <backupdir>\n",argv[0]);
		exit(1);
	}

	if(argc == 2) {

		if(( dp = opendir(argv[1])) == NULL) {
			fprintf(stderr,"Usage : %s <BACKUPDIR>\n",argv[0]);
			exit(1);
		}
		else{
			stat(argv[1], &file_info);
			if (!S_ISDIR(file_info.st_mode)) {
				fprintf(stderr,"Usage : %s <BACKUPDIR>\n",argv[0]);
				exit(1);
			}
			else if (access(argv[1], F_OK) < 0){
				fprintf(stderr,"Usage : %s <BACKUPDIR>\n",argv[0]);

				exit(1);
			}
			else if (access(argv[1], X_OK) < 0) {
				fprintf(stderr,"usage : %s <backupdir>2\n",argv[0]);
				exit(1);
			}

		}

		realpath(argv[1],backupdir);
		strcat(backupdir,"/");
		strncat(backupdir,BACKUPFILE_NAME,sizeof(BACKUPFILE_NAME)-1);
		make_backupdir(backupdir,dp);
	}

	if(argc == 1){
		flag_no = 1;//인자가 없을 경우
		realpath("./",backupdir);
		strcat(backupdir,"/");
		strncat(backupdir,BACKUPFILE_NAME,sizeof(BACKUPFILE_NAME)-1);
		make_backupdir(backupdir,dp);
	}


	link_init();//백업 리스트 초기화
	system("clear");
	pthread_create(&main_tid, NULL,ssu_prompt,NULL);

	pthread_join(main_tid, (void *)status);

	if(fp_log != NULL) fclose(fp_log);
	link_free(); //링크 해제
	exit(0);
}

void *ssu_prompt()
{
	char buf[BUFSIZE] ={};

	while(1)
	{
		memset(buf,0x00,BUFSIZE);
		fprintf(stdout,"%s>",USERID);
		fgets(buf,BUFSIZE,stdin);

		token_cmd(buf);

		if( strcmp(cmd[0], "add") == 0) {
			ssu_add();
			
		}
		else if( strcmp(cmd[0], "remove") == 0) {
			ssu_remove(cmd[1]);
		}
		else if( strcmp(cmd[0], "compare") == 0) {
			ssu_compare(cmd[1],cmd[2]);
		}
		else if( strcmp(cmd[0], "recover") == 0) {
			ssu_recover(cmd[1]);
		}
		else if( strcmp(cmd[0], "vi") == 0) {
			ls_vim();
		}
		else if( strcmp(cmd[0], "vim") == 0) {
			ls_vim();
		}
		else if( strcmp(cmd[0], "list") == 0) {
			link_print();
		}
		else if( strcmp(cmd[0], "ls") == 0) {
			ls_vim();
		}
		else if( strcmp(cmd[0], "exit") == 0){
			Exit(pthread_self());
		}
		else if(cmd[0][0] == '\0'){
		}
		else
		{
			fprintf(stdout,"'%s'cmd doesn't exist\n",cmd[0]);
		}
	}
	return NULL;

}

void make_backupdir(char *dirname, DIR *dp)
{
	if((dp = opendir(dirname)) == NULL) {
		mkdir(dirname,0766);
	}
}
void token_cmd(char *buf)
{
	char *result;
	int i = 0;

	memset(cmd,0x00, sizeof(cmd));
	buf[strlen(buf)-1] = '\0';
	result = strtok(buf," ");
	while (result != NULL && i < CMDNUM){

		if(cmd[1] == NULL)
			memcpy(cmd[i],result,sizeof(char) * (strlen(result)-1));
		else
			memcpy(cmd[i],result,sizeof(char) * strlen(result));

		result = strtok(NULL," ");
		i++;
	}

	cmd_count = i;


}
void make_backup_file(char *start, char *des){
	char cp_cmd[BUFSIZE *2] = {};
	sprintf(cp_cmd,"cp -f %s %s",start,des);
	system(cp_cmd);
}

void log_msg(char *msg,char *add) {
	if(fp_log== NULL){
		fp_log = fopen(LOGFILE_NAME, "a+");
	}
	strncat(msg, add, sizeof(char) * strlen(add));
	fwrite(msg,sizeof(char) * strlen(msg),1,fp_log);
}

void makelog_msg(char *l_msg,char *real_path,int op) {
	time_t t= time(NULL);
	struct tm tm = *localtime(&t);
	char time_msg[BUFSIZE] = {};

	sprintf(l_msg,"[%02d%02d%02d %02d%02d%02d] ",tm.tm_year%100, tm.tm_mon+1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	strncat(l_msg,real_path,sizeof(char) * strlen(real_path));
	if(op == 1){
		sprintf(time_msg,"_%02d%02d%02d%02d%02d%02d",tm.tm_year%100, tm.tm_mon+1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		strncat(l_msg,time_msg, sizeof(char) * strlen(time_msg));
	}
}

void ls_vim()
{
	char buf[BUFSIZE] = {};

	memcpy(buf,cmd[0], sizeof(cmd[0]));
	for(int i = 1; i< cmd_count;i++){
		strcat(buf," ");
		strcat(buf,cmd[i]);
	}
	system(buf);
}
void clear_stdin() {
	int c;
	while(( c = getchar()) != EOF && c != '\n'){};
}
void clear_prompt() {
	int c;
	while(( c = getchar()) != EOF ){};
}

int space_in_name(char *filename) {
	for(int i = 0; i < (int)strlen(filename); i++)
		if(isspace(filename[i]) != 0) {
			return 0; //공백 
		}
	return 1;
}
void Exit(pthread_t main_tid)
{
	link_exit();
	pthread_exit(&main_tid);
}
