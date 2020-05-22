#include "ssu_backup.h"

void ssu_compare(char *filename1, char *filename2)
{
	struct stat statinfo1;
	struct stat statinfo2;
	char f1_realpath[BUFSIZE] = {};
	char f2_realpath[BUFSIZE] = {};
	if(cmd_count != 3) {
		fprintf(stdout,"Usage : compare <FILENAME1> <FILENAME2>\n");
		return ;
	}
	realpath(filename1, f1_realpath);
	realpath(filename2, f2_realpath);

	if(access(f1_realpath,F_OK) !=0) {
		fprintf(stderr,"%s doesn't exist\n",filename1);
		return ;
	}
	if(access(f2_realpath,F_OK) !=0) {
		fprintf(stderr,"%s doesn't exist\n",filename2);
		return ;
	}

	stat(f1_realpath,&statinfo1);
	stat(f2_realpath,&statinfo2);

	if(!S_ISREG(statinfo1.st_mode)) {
		fprintf(stderr,"%s is not regular file\n",filename1);
		return ;
	}
	if(!S_ISREG(statinfo2.st_mode)) {
		fprintf(stderr,"%s is not regular file\n",filename2);
		return ;
	}


	if( (statinfo1.st_size == statinfo2.st_size) && (statinfo1.st_mtime == statinfo2.st_mtime)) {
		fprintf(stdout,"%s and %s are samefile!\n",filename1,filename2);
	}
	else {
		fprintf(stdout,"%-40s\t",filename1);
		fprintf(stdout,"%-40ld\t",statinfo1.st_mtime);
		fprintf(stdout,"%-ldbytes\n",statinfo1.st_size);

		fprintf(stdout,"%-40s\t",filename2);
		fprintf(stdout,"%-40ld\t",statinfo2.st_mtime);
		fprintf(stdout,"%-ldbytes\n",statinfo2.st_size);
	}

}


