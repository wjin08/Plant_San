#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <mysql/mysql.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#define ARR_CNT 5

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

char name[NAME_SIZE] = "[Default]";
char msg[BUF_SIZE];
char msg2[BUF_SIZE] = {0};
int str_len;
int write_flag = 0;
void finish_with_error(MYSQL *con)
{
		    fprintf(stderr, "%s\n", mysql_error(con));
			    mysql_close(con);
				    exit(1);
}

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void* thread_return;

	if (argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	sprintf(name, "%s", argv[3]);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	sprintf(msg, "[%s:PASSWD]", name);
	write(sock, msg, strlen(msg));
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);

	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

	if(sock != -1)
		close(sock);
	return 0;
}


void* send_msg(void* arg)
{
	int* sock = (int*)arg;
	int str_len;
	int ret;
	fd_set initset, newset;
	struct timeval tv;
	char name_msg[NAME_SIZE + BUF_SIZE + 2];

	FD_ZERO(&initset);
	FD_SET(STDIN_FILENO, &initset);

	fputs("Input a message! [ID]msg (Default ID:ALLMSG)\n", stdout);
	while (1) {
		memset(msg, 0, sizeof(msg));
		name_msg[0] = '\0';
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		newset = initset;
		ret = select(STDIN_FILENO + 1, &newset,NULL, NULL, &tv);
		if (FD_ISSET(STDIN_FILENO, &newset))
		{
			fgets(msg, BUF_SIZE, stdin);
			if (!strncmp(msg, "quit\n", 5)) {
				*sock = -1;
				return NULL;
			}
			else if (msg[0] != '[')
			{
				strcat(name_msg, "[ALLMSG]");
				strcat(name_msg, msg);
			}
			else
				strcpy(name_msg, msg);
			if (write(*sock, name_msg, strlen(name_msg)) <= 0)
			{
				*sock = -1;
				return NULL;
			}
		}
		if (ret == 0)
		{
			if (*sock == -1)
				return NULL;
		}
	}
}

void* recv_msg(void* arg)
{
		
	fd_set initset, newset;
	FD_ZERO(&initset);
	FD_SET(STDIN_FILENO, &initset);

	struct timeval tv;
	MYSQL* conn;
	MYSQL_ROW sqlrow;
	MYSQL_RES *result;
	int res;
	char sql_cmd[200] = { 0 };
    char new_db[75] = {0};
	char* host = "localhost";
	char* user = "server";
	char* pass = "server_pw";
	char* dbname = "mini_project";
	int ret;
	int* sock = (int*)arg;
	int i;
	char* pToken;
	char* pArray[ARR_CNT] = { 0 };
	char lin_msg[BUF_SIZE];
	char name_msg[NAME_SIZE + BUF_SIZE + 1];
	int str_len;

	int cds;
	int water;
	int happyness;
	conn = mysql_init(NULL);

	puts("MYSQL startup");
	if (!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
	{
		fprintf(stderr, "ERROR : %s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	else
		printf("Connection Successful!\n\n");

	while (1) {
		memset(name_msg, 0x0, sizeof(name_msg));
		str_len = read(*sock, name_msg, NAME_SIZE + BUF_SIZE);
		if (str_len <= 0)
       	{
           	*sock = -1;
       		 return NULL;
       	}
		//name_msg[str_len-1] = 0;
		fputs(name_msg, stdout);
       	pToken = strtok(name_msg, "[:@]");
       	i = 0;
       	while (pToken != NULL)
       	{
      	 	pArray[i] = pToken;
       	    if ( ++i >= ARR_CNT)
       		 	 break;
 	      	pToken = strtok(NULL, "[:@]");
       	}

		

		/*******************  데이터베이스에 query하는 명령어 시작   **********************/

		// char sql_cmd[200] = { 0 }; query명령 버퍼  

		/***    SENSOR_VALUE_INSERT_CODE_START    ********************************/
		if(!strcmp(pArray[1],"SENSOR") && (i == 5)){
			   cds = atoi(pArray[2]);
			   water = atoi(pArray[3]);
			   happyness = atoi(pArray[4]);
  			sprintf(sql_cmd, "insert into SWJ_ARD (name,time,cds,water,happyness) values(\"%s\",now(),%d,%d,%d)",pArray[0],cds,water,happyness);

			//sprintf(sql_cmd, "update plant_1 set cds=%d, water=%d, happyness=%d where id=1",cds,water,happyness);

			res = mysql_query(conn, sql_cmd);
			if (!res)
				printf("inserted %lu rows\n", (unsigned long)mysql_affected_rows(conn));
			else
            {
               // sprintf(new_db, "create table `%s` (id int not null auto_increment, name varchar(20), `time` time, cds int, water int, happyness int, primary key (id) ) default charset=utf8",pArray[0]);

                //mysql_query(conn, new_db);
                //mysql_query(conn, sql_cmd);
				fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
            }
		}

		/***    SENSOR_VALUE_INSERT_CODE_END      ********************************/

		/***    LAMP_VALUE_OUTPUT_CODE_START      ********************************/
		else if(!strcmp(pArray[1],"GETDB") && (i == 3)){
			if (mysql_query(conn, "select * FROM device"))
			{
        		finish_with_error(conn);
   			}	
			result = mysql_store_result(conn);
			if (result == NULL)
		    {
		        finish_with_error(conn);
		    }
			sqlrow = mysql_fetch_row(result);
			sprintf(msg2,"[%s]%s@%s@%s", pArray[0], pArray[1], pArray[2],sqlrow[4]);
			//printf("%s\n",msg);
			write(*sock,msg2,strlen(msg2));
			mysql_free_result(result);
        }
		/***    LAMP_VALUE_OUTPUT_CODE_END    ************************************/

		/***    LAMP_VALUE_INPUT_CODE_START   ************************************/
        

		else if(!strcmp(pArray[1],"SETDB") && (i == 6)){
            printf("hello\n");
		    sprintf(sql_cmd, "update user_id_pw set user_id=%s, user_pw=%s where id=%d",pArray[2],pArray[3],pArray[4]);

            res = mysql_query(conn, sql_cmd);
            if (!res)
                printf("inserted %lu rows\n", (unsigned long)mysql_affected_rows(conn));
            else
                fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));

			sprintf(lin_msg,"[%s]%s@%s",pArray[5],pArray[2],pArray[3]);
			write(*sock,lin_msg,strlen(lin_msg));	
        }
		/***    LAMP_VALUE_INPUT_CODE_END     ************************************/
		//else
			//fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
	}




		/*******************  데이터베이스에 query하는 명령어 종료   **********************/
	mysql_close(conn);
}

void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
