#include "ssu_backup.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int wait;
int make;
void ssu_add()
{
	struct Backup_info *b_info = (struct Backup_info *)malloc(sizeof(struct Backup_info));
	struct stat statinfo;
	char path[BUFSIZE+1] = {};
	int period = 0;
	int loc = 0;
	int cmd_check_count = 1;

	pthread_t backup_tid;
	//입력가능한 단어 수 최대 최소
	if(cmd_count<2 || cmd_count>9) {
		fprintf(stderr,"Usage : add <FILENAME> [PERIOD] [OPTION]\n");
		return ;
	}

	memset(b_info,0x00,sizeof(struct Backup_info)); //Backup_info구조체 초기화
	realpath(cmd[1],path); //파일의 절대경로를 구한다.

	if(strlen(path) > BUFSIZE-1) //절대경로의 길이가 255바이트가 넘어가면 에러처리
		return ;

	check_add_option(b_info);

	if(access(path, F_OK) != 0) {//파일이 존재하는지 검사
		fprintf(stderr,"access error\n");
		return ;
	}
	else
		cmd_check_count++;

	stat(path,&statinfo);

	//d옵션이 아닐 경우 일반 파일인지 검사
	if(b_info -> flag[3] != 'd' && !S_ISREG(statinfo.st_mode)) {
		fprintf(stderr,"%s is not a regularfile\n",cmd[1]);
		return ;
	}
	else if(b_info -> flag[3] =='d' &&  !S_ISDIR(statinfo.st_mode) ) { //d옵션이 주어질경우 디렉토리인지 검사
		fprintf(stderr,"%s is not a directory\n",cmd[1]);
		return ;
	}
	else if(b_info -> flag[3] =='d')
		cmd_check_count++;

	//d옵션이 아닐경우 해당 파일이 백업리스트에 없는지 검사
	if(b_info->flag[3] != 'd' && backup_count != 0) {
		if((link_search(cmd[1]) != NULL)){
			fprintf(stderr,"%s is in backup list\n",cmd[1]);
			return ;
		}
	}

	//PERIOD가 입력되었는지 안되었는지 검사하는 과정

	if(cmd_count >=3 && is_add_option(cmd[2]) ) { //PERIOD 자리에 옵션을 입력했을 경우
		fprintf(stderr,"PERIOD error\n");
		return ;
	}
	else if((period = check_int_float(cmd[2])) != 0) //숫자인지 정수인지 판별하는 함수 
	{
		if(period >=5 && period <=10){
			b_info->period = period;
			cmd_check_count++;
		}
		else{
			fprintf(stderr,"Period range error\n");
			return ;
		}
	}
	else if(cmd_count >=3)//숫자와 옵션이 아닌 다른 값을 입력했을 경우
	{
		fprintf(stderr,"Period error\n");
		return ;
	}
	else if(cmd_count == 2) {
		fprintf(stderr,"PERIOD error\n");
		return ;
	}
	if(b_info->flag[0] == 'm')
		cmd_check_count++;

	if(b_info->flag[1] == 'n'){ //-n옵션 입력시 뒤에 인자가 형식에 맞는지 검사
		loc = option_location("-n");
		if(loc+1 >= cmd_count) {
			fprintf(stderr,"-n Option error\n");
			fprintf(stderr,"cmd_check_count %d cmd_count : %d\n",cmd_check_count, cmd_count);
			return ;
		}
		if(check_int_float(cmd[loc+1]) < 1 || check_int_float(cmd[loc+1]) > 100) {
			fprintf(stderr,"NUMBER error\n");
			return ;
		}
		else {
			b_info->number = check_int_float(cmd[loc+1]);
			cmd_check_count +=2;
		}
	}

	if(b_info->flag[2] == 't'){ //-t옵션 입력시 뒤에 인자가 형식에 맞는지 검사
		loc = option_location("-t");
		if(loc+1 >= cmd_count) {
			fprintf(stderr,"-t Option error\n");
			return ;
		}
		if(check_int_float(cmd[loc+1]) < 60 || check_int_float(cmd[loc+1]) > 1200) {
			fprintf(stderr,"TIME error\n");
			return ;
		}
		else {
			b_info->time = check_int_float(cmd[loc+1]);
			cmd_check_count +=2;
		}
	}

	if(cmd_check_count != cmd_count) {
		fprintf(stderr,"Usage : add <FILENAME> [PERIOD] [OPTION]\n");
		return ;
	}

	memcpy(b_info->pathname, path, sizeof(char) * strlen(path));


	if(b_info->flag[3] != 'd') {
		pthread_create(&backup_tid,NULL ,ssu_add_backup,(void *)b_info);
	}
	else {
		add_option_d(b_info);
	}
}

void *ssu_add_backup(void * back_info)
{	
	char l_msg[BUFSIZE*2] = {};
	char b_filename[BUFSIZE*3] = {};
	char *result = NULL;
	struct stat statinfo; //m 옵션시 사용
	struct Backup_info *b_info = (struct Backup_info *)malloc(sizeof(struct Backup_info));
	time_t make_time;


	memcpy(b_info,back_info,sizeof(struct Backup_info));
	b_info->tid = pthread_self();
	link_add(b_info);

	makelog_msg(l_msg,b_info->pathname,0);
	log_msg(l_msg," added\n");

	if(b_info->flag[0] == 'm') {
		stat(b_info->pathname,&statinfo);
		make_time = statinfo.st_mtime;
	}

	make = 0;
	while(1) {
		memset(b_filename,0x00,sizeof(b_filename));
		memset(l_msg,0x00,sizeof(l_msg));
		memcpy(b_filename, backupdir, sizeof(char) * strlen(backupdir));
		result = strrchr(b_info->pathname,'/');
		strncat(b_filename, result,sizeof(char) * (strlen(result)));

		sleep(b_info->period);
		if(b_info->flag[2] =='t') {
			add_option_t(b_info);
		}

		if(b_info->flag[0] =='m') {
			stat(b_info->pathname, &statinfo);
			if(statinfo.st_mtime != make_time)
				make_time = statinfo.st_mtime;
			else
				continue;
		}

		makelog_msg(l_msg,b_filename,1);
		result = strrchr(l_msg,'_');
		strncat(b_filename,result, sizeof(char) * strlen(result));

		log_msg(l_msg," gernerated\n");
		make_backup_file(b_info->pathname,b_filename);


		if(b_info->flag[1] == 'n') {//-n옵션 실행
			add_option_n(b_info);
		}
	}
}

void add_option_n(struct Backup_info *b_info) {
	char rm_cmd[BUFSIZE+3] = {};
	char filename[BUFSIZE] = {};
	char *result = NULL;
	int num_count = 0;
	int num_start = -1;
	int num_end = 0;
	int del_count = 0;
	struct dirent **numberlist;

	result = strrchr(b_info->pathname,'/');
	memset(filename, 0x00, sizeof(filename));
	memcpy(filename, &result[1], sizeof(char) * strlen(&result[1]));

	if((num_count = scandir(backupdir,&numberlist,make_option_n_list,alphasort)) > b_info->number) {

		for(int i = 0; i < num_count; i++) {
			if((strncmp(filename, numberlist[i]->d_name, sizeof(char) * strlen(filename)) == 0) && (numberlist[i]->d_name[strlen(filename)] == '_')) {
				if(num_start >= 0)
					num_end++;
				else
					num_start = i;
			}
		}
	}


	if((del_count = num_end - b_info->number) > 0) {
		for(int i = num_start; i <= num_start + del_count; i++) {
			memset(rm_cmd,0x00, sizeof(rm_cmd));
			sprintf(rm_cmd,"%s/%s",backupdir,numberlist[i]->d_name);
			if(access(rm_cmd,F_OK) == 0){
			memset(rm_cmd,0x00, sizeof(rm_cmd));
			sprintf(rm_cmd,"rm %s/%s",backupdir,numberlist[i]->d_name);
			pthread_mutex_lock(&mutex);
			system(rm_cmd);
			pthread_mutex_unlock(&mutex);
			}
		}
	}
	sleep(5);
}

void add_option_t(struct Backup_info *b_info) {
	char rm_cmd[BUFSIZE+3] = {};
	char t_cmd[BUFSIZE] = {};
	char filename[BUFSIZE] = {};
	char *result = NULL;
	int num_count = 0;
	int num_start = -1;
	int num_end = 0;
	struct dirent **numberlist;
	struct stat statinfo;
	time_t now;

	result = strrchr(b_info->pathname,'/');
	memset(filename, 0x00, sizeof(filename));
	memcpy(filename, &result[1], sizeof(char) * strlen(&result[1]));

	if((num_count = scandir(backupdir,&numberlist,make_option_t_list,alphasort))) {

		for(int i = 0; i < num_count; i++) {
			if((strncmp(filename, numberlist[i]->d_name, sizeof(char) * strlen(filename)) == 0) && (numberlist[i]->d_name[strlen(filename)] == '_')) {
				if(num_start >= 0)
					num_end++;
				else
					num_start = i;
			}
		}
		if(num_start >= 0) {
			for(int i = num_start; i <= num_start + num_end; i++) {
				sprintf(t_cmd,"%s/%s",backupdir,numberlist[i]->d_name);
				stat(t_cmd,&statinfo);
				time(&now);
				if(now - statinfo.st_mtime > b_info->time) {
					sprintf(rm_cmd,"rm %s/%s",backupdir,numberlist[i]->d_name);
			pthread_mutex_lock(&mutex);
					system(rm_cmd);
			pthread_mutex_unlock(&mutex);
				}

			}
		}//if numstart
	} //if
}

void add_option_d(struct Backup_info *b_info) {
	struct dirent **backuplist;
	struct stat statinfo;
	char subdir[BUFSIZE+1] = {};
	char dirname[BUFSIZE+1] = {};
	int dir_count = 0;
	struct Backup_info info;
	struct Backup_info new_info;
	struct Backup_info new_info2;
	pthread_t new_tid;

	memcpy(dirname, b_info->pathname,sizeof(b_info->pathname));
	memset(&info, 0x00, sizeof(struct Backup_info));
	memcpy(&info, b_info,sizeof(struct Backup_info));

	if((dir_count = scandir(dirname,&backuplist, make_option_d_list,alphasort))){
		for(int i = 0; i < dir_count; i++) {
			sprintf(subdir,"%s/%s",dirname,backuplist[i]->d_name);
			if(strlen(subdir) >255) continue;
			if(stat(subdir,&statinfo) == -1) {
				fprintf(stderr,"stat error\n");
			}

			memset(&new_info,0x00,sizeof(struct Backup_info));
			memcpy(&new_info,&info, sizeof(struct Backup_info));
			memcpy(&new_info2,&info, sizeof(struct Backup_info));

			memcpy(&new_info.pathname, subdir, sizeof(subdir));
			memcpy(&new_info2.pathname, subdir, sizeof(subdir));

			make = 1;
			if(S_ISDIR(statinfo.st_mode)) {
				add_option_d(&new_info);
			}
			else if(S_ISREG(statinfo.st_mode)) {

				if( link_search(new_info2.pathname) == NULL) {
					if(space_in_name(new_info2.pathname)){
						pthread_create(&new_tid,NULL,ssu_add_backup,(void *)(&new_info2));
						while(make);
					}
				}
			}
		}
	}
}
void check_add_option(struct Backup_info *b_info) //입력시 옵션을 판별해주는 함수
{

	for(int i =2; i<cmd_count; i++){
		if(strcmp(cmd[i],"-m") == 0)
			b_info->flag[0] = 'm';
		else if(strcmp(cmd[i],"-n") == 0)
			b_info->flag[1] = 'n';
		else if(strcmp(cmd[i],"-t") == 0)
			b_info->flag[2] = 't';
		else if(strcmp(cmd[i],"-d") == 0)
			b_info->flag[3] = 'd';
	}

}

int is_add_option(char *op) //이 문자가 옵션인지 아닌지 판별해주는 함수
{

	if(strcmp(op,"-m") == 0) {
		return 1;
	}
	else if(strcmp(op,"-n") == 0) {
		return 1;
	}
	else if(strcmp(op,"-t") == 0) {
		return 1;
	}
	else if(strcmp(op,"-d") == 0) {
		return 1;
	}

	return 0;
}

int check_int_float(char *str) //정수인지 실수 인지 판별해주는 함수 
{
	int i= 0;
	float f = 0;
	float rest = 0;

	for(int i =0 ; i <(int)strlen(str); i++) {
		if(!('0'<=str[i] && str[i] <= '9'))
			return 0;
	}

	f = atof(str);
	i = atoi(str);
	rest = f-i;

	if(rest == 0)
		return i; //정수
	else
		return 0;// 실수
}

int option_location(char *op) // 해당 옵션이 cmd에서 어느 위치에 있는지 확인해주는 함수
{
	for(int i = 2; i < cmd_count; i++) {
		if(strcmp(cmd[i],op) == 0)
			return i;
	}

	return 0;
}

int make_option_t_list(const struct dirent *info) {
	char info_realpath[BUFSIZE*2] = {};
	struct stat statinfo;

	if(strncmp(info->d_name, ".", sizeof(info->d_name)) == 0)
		return 0;//포함 안시킴
	if(strncmp(info->d_name, "..", sizeof(info->d_name)) == 0)
		return 0;//포함 안시킴

	sprintf(info_realpath,"%s/%s",backupdir,info->d_name);

	if(stat(info_realpath,&statinfo) == -1){
		fprintf(stderr,"%s \nstat error in make option n list\n",info_realpath);
		return 0;
	}
	if(!S_ISREG(statinfo.st_mode))return 0;

	return 1;
}

int make_option_n_list(const struct dirent *info) {
	char info_realpath[BUFSIZE] = {};
	struct stat statinfo;

	if(strncmp(info->d_name, ".", sizeof(info->d_name)) == 0)
		return 0;//포함 안시킴
	if(strncmp(info->d_name, "..", sizeof(info->d_name)) == 0)
		return 0;//포함 안시킴

	sprintf(info_realpath,"%s/%s",backupdir,info->d_name);

	if(stat(info_realpath,&statinfo) == -1)
		return 0;

	if(!S_ISREG(statinfo.st_mode))return 0;

	return 1;
}

int make_option_d_list(const struct dirent *info) {
	char info_realpath[BUFSIZE] = {};

	if(strncmp(info->d_name, ".", sizeof(info->d_name)) == 0)
		return 0;//포함 안시킴
	if(strncmp(info->d_name, "..", sizeof(info->d_name)) == 0)
		return 0;//포함 안시킴

	sprintf(info_realpath,"%s/%s",backupdir,info->d_name);
	return 1;
}
