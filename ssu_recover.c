#include "ssu_backup.h"
extern pthread_mutex_t mutex;
char filename[BUFSIZE];

void ssu_recover(char *recover_name)
{
	struct Node *nodeInfo =NULL;
	struct dirent **dirlist;
	int dir_count = 0;//, dir_save=0;
	int recover_count = 0;
	int idx = 0;
	int flag_n = 0;
	char real_path[BUFSIZE *2] = {};
	char real_path2[BUFSIZE *2] = {};
	char real_path3[BUFSIZE *2] = {};
	char cp_cmd[BUFSIZE *2] = {};
	char l_msg[BUFSIZE *2] = {};
	char *result = NULL;

	struct stat statinfo;

	if(cmd_count <2) {
		fprintf(stderr,"Usage : recover <FILENAME> [OPTION]\n");
		return ;
	}

	realpath(recover_name, real_path);
	result = strrchr(real_path,'/');

	memset(filename, 0x00,sizeof(filename));
	memcpy(filename, &result[1], sizeof(char) *(strlen(result)-1));
	//파일이 존재하는 지 확인한다.
	if(access(real_path,F_OK) != 0) {
		fprintf(stderr,"%s file doesn't exist\n",filename);
		return ;
	}
	//현재 백업 리스트에 있는지 검사한다.
	if( (nodeInfo = link_search(real_path)) == NULL) {
		fprintf(stderr,"%s doesn't exist in backuplist\n",filename);
		return ;
	}


	if((dir_count = scandir(backupdir,&dirlist, make_recover_list, alphasort)) == -1) {
		fprintf(stderr,"Dir error\n");
		return ;
	}

	//변경할 파일에 대한 백업 파일이 존재 하지 않을 경우
	if(dir_count == 0) {
		fprintf(stderr,"%s's backupfile doesn't exist\n",filename);
		return ;
	}

	if(cmd_count >2) {
		//recover 옵션과 옵션인자수를 잘못 입력 했을 경우
		if((cmd_count != 4) || (strcmp(cmd[2],"-n") != 0)) {
			fprintf(stderr,"Usage : recover <FILENAME> [OPTION] <NEWFILE>\n");
			return ;
		}

		realpath(cmd[3] , real_path3);
		if(access(real_path3,F_OK) == 0) {
			fprintf(stderr,"%s file is exist\n",cmd[3]);
			return ;
		}
		flag_n = 1;
	}
	//백업 중지
	pthread_cancel((nodeInfo->backup_info->tid));

	idx = 0;

	fprintf(stdout,"0. exit\n");
	while(idx < dir_count) {
		memset(real_path2,0x00,sizeof(real_path2));
		sprintf(real_path2,"%s/%s",backupdir,dirlist[idx]->d_name);
		if(stat(real_path2,&statinfo) == -1) 
			fprintf(stdout,"stat error\n");
		result = strrchr(dirlist[idx]->d_name,'_');
		recover_count++;
		fprintf(stdout,"%d. %s\t",recover_count,&result[1]);
		fprintf(stdout,"%ldbytes\n",statinfo.st_size);
		idx++;
	}
	int num = -1;

	while(1)
	{
		fprintf(stdout,"Choose file to recover : ");
		scanf("%d",&num);
		clear_stdin();
		if(0<=num && num <=recover_count)break;
	}

	if(num != 0) {
		memset(real_path2,0x00,sizeof(real_path2));
		sprintf(real_path2,"%s/%s",backupdir,dirlist[num-1]->d_name);

		if(flag_n ==  0) {
			sprintf(cp_cmd,"cp -rf %s %s",real_path2,real_path);
		}
		else 
			sprintf(cp_cmd,"cp %s %s",real_path2,real_path3);

		FILE *fp;
		char buf[BUFSIZE] = {};

		system(cp_cmd);
		makelog_msg(l_msg,real_path2,3);
		log_msg(l_msg," recoverd\n");
		fprintf(stdout,"Recovery success\n");

		if(flag_n == 1){ 
			fp = fopen(real_path3,"r");
			fprintf(stdout,"<%s>\n",real_path3);
		}
		else {
			fp = fopen(nodeInfo->backup_info->pathname,"r");
			fprintf(stdout,"<%s>\n",nodeInfo->backup_info->pathname);
		}
		if(fp != NULL) {
			while(!feof(fp)) {
				memset(buf,0x00,sizeof(buf));
				fgets(buf,sizeof(buf),fp);
				puts(buf);
				if( feof(fp) == 0) break;
			}
		}
	}
	link_remove(nodeInfo->backup_info->pathname,0);
	pthread_cancel((nodeInfo->backup_info->tid));
	for(idx = 0; idx < dir_count; idx++)
		free(dirlist[idx]);
}
//디렉토리 탐색중 recover할 파일의 이름만 골라서 dirlist에 저장한다
int make_recover_list(const struct dirent *info) 
{
	char info_realpath[BUFSIZE]= {};
	char cmd_realpath[BUFSIZE]= {};
	struct stat statinfo;
	char *result;

	if(strncmp(info->d_name,".",sizeof(info->d_name)) == 0)
		return 0;
	if(strncmp(info->d_name,"..",sizeof(info->d_name)) == 0)
		return 0;

	sprintf(info_realpath,"%s/%s",backupdir,info->d_name);
	if(stat(info_realpath, &statinfo) == -1) {
		fprintf(stderr,"stat error in make_recover_list\n");
	}
	if(!S_ISREG(statinfo.st_mode))return 0;

	realpath(cmd[1],cmd_realpath);
	result = strrchr(cmd_realpath,'/');

	if(strncmp(info->d_name, &result[1],sizeof(char) * strlen(&result[1]))== 0) {
		if(info->d_name[strlen(result)-1] == '_'){
			return 1;
		}
	}

	return 0;
}


