
#ifndef STUDENT_H
#define STUDENT_H

#define PORT 8888
#define BACKLOG 5
#define MAXDATASIZE 1000000

#include "mysql.h"
size_t sqlStatementLen = 300;
int bufferHeadLength = sizeof(int) + sizeof(char);
int SizeOfInt = sizeof(int);

#pragma (push, 1)
typedef struct Student
{
	char s_studentName[20];
	int	s_studentAge;
	char s_studentNumber[10];
	float s_ChineseScore;
	float s_EnglishScore;
	float s_MathScore;
	float s_averageScore;
}student;
struct ARG
{
	int connfd;
	struct sockaddr_in client;
};
int case_one();
int case_twe();
int case_three();
int case_four();
int send_process(FILE *fp, int sockfd);
int receive_process(int sockfd);
int chooseoption();
int addOneRecord(student *NewStudent);
int deleteOneRecord(student *NewStudent);
int updateOneRecord(student *NewStudent, char *updateStatement);
int queryRecord(student *NewStudent);
int queryByStudentNumberFirst(const student *NewStudent, char buffer[], int sockfd);
int client_send_buffer(int sockfd,const char *sqlSatement, char option,char *buffer);
int init_client_socket(int argc, char *argv[]);
int server_recv(int sockfd, char *Buffer, int buff_len,int flags);
int process_client(const int connfd, const struct sockaddr_in client);
int server_send_buffer(int connfd, char *buffer, int Length, char option);
int queryResultFun(MYSQL_ROW row, student *addStudentInfo);
void *thread_func(void *arg);
int init_server_socket();
#endif
