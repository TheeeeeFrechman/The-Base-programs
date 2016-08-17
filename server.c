#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "sock_head.h"
#include "mysql.h"
#pragma comment(lib, "libmysql.lib")
int  process_client(const int connfd, const struct sockaddr_in client)
{
	MYSQL mysql_connection;
	MYSQL_RES *queryResult;
	MYSQL_ROW row;
	int rowCount;

	char buffer[MAXDATASIZE];
	char tempBuffer[MAXDATASIZE] = {""};
	int bufferLength = 0;

	char *sqlStatement;
	char option;
	int actuallyReceivedBytes = 1;
	int originalReceivedBytes = 0;
	int returnValue;     //接收函数的返回值

	student *addStudentInfo;

	addStudentInfo = (student *)malloc(sizeof(student));
	if (addStudentInfo == NULL)
	{
		printf("malloc addStudentInfo failed.\n");
		return -1;
	}

	sqlStatement = (char *)malloc(sizeof(char)*sqlStatementLen);
	if (sqlStatement == NULL)
	{
		printf("malloc sqlStatement failed.\n");
		return -1;
	}

	printf("You got a connection from %s.\n", inet_ntoa(client.sin_addr));

	while (1)        //connect mysql
	{
		mysql_init(&mysql_connection);
		if (mysql_real_connect(&mysql_connection, "localhost", "root", "12345", "students", 0, NULL, 0))
		{
			printf("connect mysql sucess.\n");
			break;
		}
		else
		{
			printf("connect mysql failed.\n");
			return -1;
		}
	}

	while (1)
	{
		actuallyReceivedBytes = server_recv(connfd,buffer,MAXDATASIZE,option,sqlStatement);
		strncpy(sqlStatement, (buffer + bufferHeadLength), (actuallyReceivedBytes - bufferHeadLength));
		printf("received one sql statement is %s\n",sqlStatement);
				printf("there!\n");
		option = buffer[SizeOfInt];
		switch (option)
		{
			case 0x01:
			{
				case_add(connfd,mysql_connection,sqlStatement,buffer,bufferHeadLength);
				break;
			}
			case 0x02:
			{
				case_delete(connfd,mysql_connection,sqlStatement,buffer,bufferHeadLength);
				break;
			}
			case 0x03:
			{
				case_updata(connfd,mysql_connection,sqlStatement,buffer,bufferHeadLength);
				break;
			}
			case 0x04:
			{
				case_qurey(connfd,mysql_connection,sqlStatement,buffer,bufferHeadLength,addStudentInfo);
				break;
			}   //case 0x04 end
			default: printf("received a wrong option.\n");	break;
		}    //switch end
	}    //while(1) end
	mysql_close(&mysql_connection);
	free(sqlStatement);
	sqlStatement = NULL;
	free(addStudentInfo);
	addStudentInfo = NULL;
	return 1;
}   //process_client end
int case_add(int connfd,MYSQL con,char *sqlStatement,char *buffer,int bufferheadlen)
{
	int returnValue;
	returnValue = mysql_query(&con, sqlStatement);
                                if (returnValue != 0)
                                {
                                        printf("insert data failed.\n");

                                        strncpy((buffer + bufferheadlen), "insert failed.", strlen("insert failed."));
                                        server_send_buffer(connfd, buffer, strlen("insert failed."), 0x01);
                                }
                                else
                                {
                                        printf("insert one data sucess.\n");
                                        strncpy((buffer + bufferheadlen), "insert sucess.", strlen("insert sucess."));
                                        server_send_buffer(connfd, buffer, strlen("insert sucess."), 0x01);
                                }
	return 1;
}
int case_delete(int connfd,MYSQL con,char *sqlStatement,char *buffer,int bufferheadlen)
{
        int returnValue;
        returnValue = mysql_query(&con, sqlStatement);
                                if (returnValue != 0)
                                {
					printf("delete data failed.\n");
                                        strncpy((buffer + bufferheadlen), "delete failed.", strlen("insert failed."));
                                        server_send_buffer(connfd, buffer, strlen("delete failed."), 0x01);
                                }
                                else
                                {
                                        printf("delete one data sucess.\n");
                                        strncpy((buffer + bufferheadlen), "delete sucess.", strlen("insert sucess."));
                                        server_send_buffer(connfd, buffer, strlen("delete sucess."), 0x01);
                                }
        return 1;
}
int case_updata(int connfd,MYSQL con,char *sqlStatement,char *buffer,int bufferheadlen)
{
        int returnValue;
        returnValue = mysql_query(&con, sqlStatement);
                                if (returnValue != 0)
                                {
                                        printf("updata data failed.\n");
                                        strncpy((buffer + bufferheadlen), "updata failed.", strlen("insert failed."));
                                        server_send_buffer(connfd, buffer, strlen("updata failed."), 0x01);
                                }
                                else
                                {
                                        printf("updata one data sucess.\n");
                                        strncpy((buffer + bufferheadlen), "updata sucess.", strlen("insert sucess."));
                                        server_send_buffer(connfd, buffer, strlen("updata sucess."), 0x01);
                                }
        return 1;
}
int case_qurey(int connfd,MYSQL con,char *sqlStatement,char *buffer,int bufferheadlen,student *addStudentInfo)
{
	int returnValue;
	MYSQL_RES *queryResult;
	MYSQL_ROW row;
	int rowCount;
	returnValue = mysql_query(&con, sqlStatement);
                                if (returnValue != 0)
                                {
                                        printf("query failed.\n");
                                        strncpy((buffer + bufferheadlen), "query failed.", strlen("query failed."));
                                        server_send_buffer(connfd, buffer, strlen("query failed."), 0x04);
                                }
                                else
                                {
                                        queryResult = mysql_store_result(&con);

                                        rowCount = mysql_num_rows(queryResult);

                                        if(rowCount == 0)
                                        {
                                                printf("No record.\n");
                                                strncpy((buffer + bufferheadlen), "No record.", strlen("No record."));
                                                server_send_buffer(connfd, buffer, strlen("No record."), 0x04);
						return -1;
                                        }

                    			while (row = mysql_fetch_row(queryResult))
                    			{
                                                returnValue = queryResultFun(row, addStudentInfo);
                                                if (returnValue == 1)
                                                {
                                                        snprintf(sqlStatement, sqlStatementLen, "update student set averagescore = '%f' where studentnumber = '%s'",addStudentInfo->s_averageScore, addStudentInfo->s_studentNumber);
                                                        returnValue = mysql_query(&con, sqlStatement);
                                                        if (returnValue != 0)
                                                        {
                                                                printf("update averagescore data failed.\n");
                                                        }
                                                }
                                                memcpy((buffer + bufferHeadLength), addStudentInfo, sizeof(student));
                                                server_send_buffer(connfd, buffer, sizeof(student), 0x04);
		    			}
                                        mysql_free_result(queryResult);
				}    
	return 1;

}
int server_recv(int connfd,char *buffer,int MAXDATASIZEi,int option,char *sqlStatement)
{
	size_t sqlStatementLen = 300;
	memset(buffer, 0, sizeof(buffer));
        memset(sqlStatement, 0, sqlStatementLen);
        int actuallyReceivedBytes = 1;
        int originalReceivedBytes = 0;

                do
                {
                        if ((actuallyReceivedBytes = recv(connfd, (buffer + originalReceivedBytes), MAXDATASIZEi, 0)) == 0)
                        {
                                printf("client closed.\n");
                                close(connfd);
                                return -1;
                        }
                        else
                        {
                                printf("actually Received %d Bytes.\n", actuallyReceivedBytes);
                                originalReceivedBytes = ntohl(*(int *)buffer);
                                printf("original should Received %d Bytes.\n", originalReceivedBytes);
                                if (actuallyReceivedBytes != originalReceivedBytes)
                                {
                                        originalReceivedBytes += actuallyReceivedBytes;
                                        continue;
                                }
                        }
                }while (actuallyReceivedBytes != originalReceivedBytes);

                buffer[actuallyReceivedBytes+1] = '\0';
		return actuallyReceivedBytes;
}
int server_send_buffer(int connfd, char *buffer, int Length, char option)
{
	int bufferLength;
	int returnvalus;
	int max = 0;
	bufferLength = bufferHeadLength + Length;
	(*(int *)buffer) = htonl(bufferLength);
	buffer[SizeOfInt] = option;
	while(bufferLength > max)
	{
	returnvalus = send(connfd, buffer, bufferLength, 0);
	max += returnvalus;
	if(returnvalus < 0)
		{
		printf("server send fail! \n");
		return -1;
		}
	}
	return 1;
}

int queryResultFun(MYSQL_ROW row, student *addStudentInfo)
{
	int loopIndex = 0;
	int returnValue = 0;
	float theTureAverageScore;

	strncpy(addStudentInfo->s_studentName, row[loopIndex], strlen(row[loopIndex]));
	++loopIndex;

	addStudentInfo->s_studentAge = atoi(row[loopIndex]);
	++loopIndex;

	strncpy(addStudentInfo->s_studentNumber, row[loopIndex], strlen(row[loopIndex]));
	++loopIndex;

	addStudentInfo->s_ChineseScore = atof(row[loopIndex]);
	++loopIndex;

	addStudentInfo->s_EnglishScore = atof(row[loopIndex]);
	++loopIndex;

	addStudentInfo->s_MathScore = atof(row[loopIndex]);
	++loopIndex;

	addStudentInfo->s_averageScore = atof(row[loopIndex]);

	theTureAverageScore = (addStudentInfo->s_ChineseScore + addStudentInfo->s_EnglishScore + addStudentInfo->s_MathScore) / 3;
	if (theTureAverageScore != addStudentInfo->s_averageScore)
	{
		addStudentInfo->s_averageScore = theTureAverageScore;
		returnValue = 1;
	}

	return returnValue;
}

/*
*描述：线程的执行函数入口
*/
void *thread_func(void *arg)
{
	struct ARG *clientInfo;
	clientInfo = (struct ARG *)arg;

	process_client(clientInfo->connfd, clientInfo->client);
	pthread_exit(NULL);
	return;
}

/*
*初始化socket,但不连接
*/
int init_server_socket()
{
	int listenfd;
	int opt;
	struct sockaddr_in server;

	opt = SO_REUSEADDR;


	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("creat sokcet failed.\n");
		return -1;
	}

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		printf("bind socket failed.\n");
		return -1;
	}

	if (listen(listenfd, BACKLOG) == -1)
	{
		printf("listen socket error.\n");
		return -1;
	}

	return listenfd;
}


int main()
{
	int listenfd;
	int connectfd;
	struct sockaddr_in client;
	struct ARG *arg;
	socklen_t len;
	pthread_t tid;
	static pthread_mutex_t testlock;
	pthread_mutex_init(&testlock, NULL);
	len = sizeof(client);
	arg = (struct ARG *)malloc(sizeof(struct ARG));
	if (arg == NULL)
	{
		printf("malloc ARG failed.\n");
		return -1;
	}

	listenfd = init_server_socket();
	if(listenfd < 0)
	{
		printf("listenfd is fail\n");
		return -1;
	}
	while (1)
	{
		if ((connectfd = accept(listenfd, (struct sockaddr *)&client, &len)) == -1)
		{
			printf("accept socket error.\n");
			return -1;
		}
		pthread_mutex_lock(&testlock);
		arg->connfd = connectfd;
		memcpy((void *)&arg->client, &client, sizeof(client));

		if (pthread_create(&tid, NULL, thread_func, (void *)arg))
		{
			printf("thread create failed.\n");
			return -1;
		}
		pthread_mutex_unlock(&testlock);
	}
	printf("socket connect success\n");
	free(arg);
	pthread_mutex_destroy(&testlock);
	arg = NULL;
	close(listenfd);
	return 0;
}
