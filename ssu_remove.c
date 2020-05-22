#include "ssu_backup.h"

void ssu_remove(char *remove)
{
	char filename[BUFSIZE] = {};
	if(cmd_count !=2) {
		fprintf(stderr,"Usage : remove <FILENAME> [OPTION]\n");
		return ;
	}

	if(strcmp(remove,"-a") == 0) {
		link_remove_all();
	}
	else 
	{
		realpath(remove,filename);
		if(access(filename,F_OK) != 0) {
			fprintf(stderr,"%s doesn't exist\n",remove);
			return ;
		}
		if(link_remove(filename,1) == 0) {
			fprintf(stderr,"%s doesn't exist\n",remove);
			return ;
		}
	}
}
