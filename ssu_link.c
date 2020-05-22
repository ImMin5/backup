#include "ssu_backup.h"

pthread_mutex_t mutex_list = PTHREAD_MUTEX_INITIALIZER;
int add_flag = 0;

void link_init() {
	list = (struct List*)malloc(sizeof(struct List));
	if(list == NULL) {
		fprintf(stderr,"linked error\n");
		return ;
	}
	else {
		list->tail = (struct Node*)malloc(sizeof(struct Node));
		list->head = (struct Node*)malloc(sizeof(struct Node));
	}
}

void link_add(struct Backup_info *info) {
	struct Node *preNode= list->head;
	struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
	struct Backup_info *newinfo = (struct Backup_info*)malloc(sizeof(struct Backup_info));

	list->index ++;
	//newNode에 데이터 삽입
	memcpy(newinfo,info, sizeof(struct Backup_info));
	newNode->backup_info = newinfo;

	if(backup_count ==  0) {
		list->head->next = newNode;
		newNode->prev = list->head;
		newNode->next = list->tail;
		list->tail->prev = newNode;
	}
	else {
		while(preNode->next->next != NULL)
			preNode = preNode->next;

		newNode->next = preNode->next;
		preNode->next = newNode;
		list->tail->prev = newNode;
		newNode->prev = preNode;
	}
	backup_count++;
}

int link_remove(char *remove,int flag) {
	struct Node *curNode = list->head;
	struct Node *prevNode= list->head;
	char filename[BUFSIZE] = {};
	char *result = NULL;
	char remove_msg[BUFSIZE*2] = {};


	result = strrchr(remove,'/');
	memcpy(filename, &result[1], sizeof(char) * strlen(&result[1]));

	while((curNode->next->next) != NULL) {
		curNode = curNode->next;

		if(strncmp(curNode->backup_info->pathname,remove, sizeof(char)* strlen(curNode->backup_info->pathname)) == 0) {

			prevNode = curNode->prev;
			prevNode->next = curNode->next;
			curNode->next->prev = prevNode;
			pthread_cancel(curNode->backup_info->tid);
			makelog_msg(remove_msg,curNode->backup_info->pathname,0);
			if(flag)log_msg(remove_msg," deleted\n");
			backup_count--;
			free(curNode);
			return 1;
		}
	}

	return 0;
}

void link_remove_all()
{
	struct Node *curNode = list->head;
	char remove_msg[BUFSIZE*2] = {};

	if(backup_count == 0) {
		fprintf(stderr,"backupfile doesn't exist\n");
		return;
	}
	while((curNode->next->next) != NULL) {
		curNode = curNode -> next;
		pthread_cancel(curNode->backup_info->tid);
		pthread_join(curNode->backup_info->tid,NULL);

		makelog_msg(remove_msg,curNode->backup_info->pathname,0);
		log_msg(remove_msg," deleted\n");
		backup_count--;
	}
	backup_count = 0;
	list->head->next = NULL;
	list->tail->prev = NULL;
	return ;
}

struct Node* link_search(char *search){
	struct Node *newNode = list->head;
	char filename[BUFSIZE] = {};
	char filename2[BUFSIZE] = {};
	char searchname[BUFSIZE] = {};
	char *result = NULL;

	if( (result= strrchr(search,'/')) != NULL) {
		memcpy(filename2,&result[1], sizeof(char) * strlen(&result[1]));
		realpath(search,searchname);

	}
	else {
		realpath(search,searchname);
		memcpy(filename2,search,sizeof(char) * strlen(search));
	}

	if(backup_count == 0) return NULL;
	while((newNode->next->next) != NULL) {
		newNode = newNode->next;
		result = strrchr(newNode->backup_info->pathname,'/');
		memcpy(filename, &result[1], sizeof(char) * strlen(&result[1]));

		if(strncmp(searchname, newNode->backup_info->pathname, sizeof(char) * strlen(newNode->backup_info->pathname)) == 0){
			return newNode;
		}
		else if(strncmp(filename,filename2,sizeof(char) * strlen(filename)) == 0) { 
			return newNode;
		}
	}

	return NULL;
}

void link_print() {
	struct Node *newNode = list->head;

	if(backup_count == 0) return;
	while((newNode->next->next) != NULL){
		newNode = newNode->next;
		fprintf(stdout,"%-80s\t",newNode->backup_info->pathname);
		for(int i =0; i<4;i++)
			if(newNode->backup_info->flag[i] != 0) fprintf(stdout,"%c",newNode->backup_info->flag[i]);
		fprintf(stdout,"\t%-5d\n",newNode->backup_info->period);
	}
}
void link_exit() {
	struct Node *newNode = list->head;
	if(backup_count == 0) return;
	else {
		for(int i = 0; i < backup_count;i++) {
			newNode = newNode->next;
			pthread_cancel((newNode->backup_info->tid));
		}
	}
}

void link_free() {
	struct Node *curNode = list->head->next;
	struct Node *nextNode;


	for(int i =0; i< backup_count;i++) 
	{
		if((curNode -> next) != NULL)
			nextNode = curNode->next;
		free(curNode);
		curNode = nextNode;
	}
	if(list != NULL) {
		free(list->head);
		free(list->tail);
		free(list);
	}
}
