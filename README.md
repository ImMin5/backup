# backup

## 파일별 함수 설명

### main.c
+ void token_cmd(char *) : 프롬프트에서 입력한 단어를 공백 단위로 배열에 저장한다.
+ void *ssu_prompt() : 프롬프트 출력을 실행하는 함수 새로운 쓰레드를 생성한다.
+ void make_backupdir(char *,DIR *) : 프로그램 실행시 인자가 있을 경우와 없을 경우를 판단하여 백업 디렉토리의 절대 경로를 생성하여 변수에 저장하는 함수
+ void log_msg(char *, char*) : 로그파일에 로그를 출력하는 함수
+ void makelog_msg(char *,char *, int) : 로그파일에 출력할 로그메세지를 출력하는 함수
+ void ls_vim() : ls, vi(m) 입력을 했을 때 시스템함수를 통해 실행시켜주는 함수
+ void Exit(pthread_t) : exit 명령어 입력시 모든 쓰레드를 종료시키고 링크드 리스트로 구성된 백업리스트를 해제한다.
+ void clear_stdin() : 입력버퍼를 비워주는 함수
+ void make_backup_file(char *,char *) : 설정된 백업경로에 백업폴더를 만들어주는 함수
+ int space_in_name(char *) : -d 옵션시 백업에 추가할 파일이름에 공백이 있는지 검사하는 함수

### ssu_add.c
+ void ssu_add() : add 명령어 입력시 실행되는 함수이다. add옵션의 유무와 실행조건이 맞는지 확인하고 에러 메시지를 출력해주고 백업파일을 관리하는 쓰레드를 생성하는 함수
+ void *ssu_add_backup(void *) : 백업파일에 관한 쓰레드 실행 함수이다. 입력된 옵션에 맞게 작동한다.
+ void add_option() : 어떤 옵션이 입력되었는지 확인하고 Bakup_info 구조체에 옵션변수에 저장한다.
+ void check_add_option(struct Backup_info *) :어떤 옵션이 입력되었는지 확인하고 Bakup_info 구조체에 옵션변수에 저장한다.
+ int check_int_float(char *) : 인자로 받은 문자열이 실수인지 정수인지 판별해서 실수이면 0을 리턴하고 정수이면 정수값을 리턴한다.
+ int is_add_option(char *) : 인자로 받은 문자열이 옵션인지 아닌지 확인해주는 함수 add 옵션이면 1 아니면 0을 리턴한다.
+ int option_location(char *) : 인자로 받은 옵션의 배열상 위치를 리턴하는 함수. 옵션 다음의 인자값이 제대로 들어왔는지 확인하기 위해 만들었다.
+ void add_option_n(struct Backup_info *) : n 옵션 실행시 백업디렉토리에서 해당하는 파일이름 리스트를 만들기 위한 함수
+ void add_option_t(struct Backup_info *) :  t 옵션 실행시 백업디렉토리에서 해당하는 파일이름 리스트를 만들기 위한 함수
+ void add_option_d(struct Backup_info *) :   d 옵션 실행시 백업디렉토리에서 해당하는 파일이름 리스트를 만들기 위한 함수
+ int make_option_t_list(const struct dirent *) : t옵션 실행시 scandir에서 사용하는 필터함수
+ int make_option_n_list(const struct dirent *) : t옵션 실행시 scandir에서 사용하는 필터함수
+ int make_option_d_list(const struct dirent *) : t옵션 실행시 scandir에서 사용하는 필터함수
### ssu_link.c
+ void link_init() : 백업리스트의 링크드 리스트를 초기화 해주는 함수
+ void link_add(struct Backup_info *) : add 성공시 백업리스트에 노드를 추가해 주는 함수
+ int link_remove(char*,int) : 백업리스트에서 인자로 받은 이름을  찾아서 삭제해주는 함수
+ void link_remove_all() : reomve -a 옵션을 실행 시켜주는 함수이다 백업리스트에 있는 모든 노드를 삭제하고 쓰레드를 종료 시킨다.
+ struct Node* link_search(char *) : 인자로 받은 파일이름이 백업리스트에 있는지 확인해 주는 함수이다. 찾을 경우 해당 노드를 리턴하고 못찾을 경우 NULL을 리턴한다.
+ void link_print() : list 명령어를 실행시켜주는 함수이다. 백업리스트의 있는 노드의 정보를 읽어서 파일의 절대경로와 적용된 옵션 그리고 PERIOD값을 출력한다.
+ void link_free() : 백업리스트를 할당해제해주는 함수이다.
+ void link_exit() : exit 명령어 입력시 모든 쓰레드를 종료시키고 백업리스트초기화 및 할당 해제 시킨다.
### ssu_compare.c
+ void ssu_compare(char *, char *) : compare 명령어 입력시 실행되는 함수이다. 입력 받은 두인자의 에러 조건을 검사하고 이상 없을 경우 두 파일의 mtime과 파일 사이즈를 비교하여 두 조건이 같으면 같은 파일이라는 메시지를 출력 다르면 mtime과 파일사이즈를 출력한다.

### ssu_recover.c
+ void ssu_recover(char *) : reocver명령어를 입력했을 경우 실행되는 함수이다. 인자로 입력된 파일명이 에러조건에 검사한다. 검사 후 이상없을 경우 복구할 파일을 화면에 출력한다. 이때 해당 백업 쓰레드는 종료된다.
+ int make_recover_list(const struct dirent *) : recover리스트를 만들때 scandir() 사용시 필터에 사용되는 함수이다.

### ssu_reomve.c
+ void ssu_remove(char *) : remove 명령어 실행시 수행되는 함수이다. 인자로 받은 문자열이 파일명일 경우 해당 파일이 백업리스트에 있는지 검사후 삭제, 만약에 -a 옵션이 입력되면 모든 백업리스트를 삭제한다.
