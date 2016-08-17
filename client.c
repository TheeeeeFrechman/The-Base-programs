#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <time.h>
#include "sock_head.h"
#include "mysql.h"

int send_process(FILE *fp, int sockfd)
{
	int option;
	int queryMode;
	student *NewStudent;
	char *sqlStatement;
	char *updateStatement;
	char buffer[MAXDATASIZE];
	int updateStatementLen;
	int returnValue;
	NewStudent = (student *)malloc(sizeof(student));
	if (NewStudent == NULL)
	{
		printf("malloc NewStudent failed.\n");
		return -1;
	}

	sqlStatement = (char *)malloc(sizeof(char)*sqlStatementLen);
	if (sqlStatement == NULL)
	{
		printf("malloc sqlStatement failed.\n");
		return -1;
	}

start:
	while(1)
	{
		memset(buffer, 0, sizeof(buffer));
		memset(sqlStatement, 0, sqlStatementLen);

		option = chooseoption();
		switch (option)
		{
			case 1:                              //增加一条学生信息
			{
				case_add(NewStudent,buffer,sockfd,sqlStatement);
				goto start;
				break;
			}  //case 1 end
			case 2:                             //删除一条学生信息
			{
					case_delete(NewStudent,buffer,sockfd,sqlStatement);
					goto start;
				break;
			}  //case 2 end
			case 3:                          //更新一条学生信息
			{
					case_update(NewStudent,buffer,sockfd,sqlStatement,updateStatement);
					goto start;
				break;
			}  //case 3 end
			case 4:                          //查询一条学生信息
			{
					
				case_query(NewStudent,buffer,sockfd,sqlStatement);
				break;
			}  //case 4 end
			default : break;
		}   //switch end

		returnValue = receive_process(sockfd);
		if(returnValue == -1)
		{
			printf("receive_process () malloc failed.\n");
		}
	}        //while(1) end;
	free(NewStudent);
	NewStudent = NULL;
	free(sqlStatement);
	sqlStatement = NULL;
	return 1;
}  //process() end
int case_add(student *NewStudent,char *buffer,int sockfd,char *sqlStatement)
{
	int returnValue;
	                                addOneRecord(NewStudent);

                                if ((returnValue = queryByStudentNumberFirst(NewStudent, buffer, sockfd)) == 1)
                                {
                                        system("clear");
                                        printf("The student number = %s's student is already exits, can't add.\n", NewStudent->s_studentNumber);
                                        return -1;
                                }

                memset(buffer, 0, sizeof(buffer));
                memset(sqlStatement, 0, sqlStatementLen);

                returnValue = snprintf(sqlStatement, sqlStatementLen, "insert into student(studentname,age,studentnumber,chinesescore,englishscore,mathscore,averagescore)values('%s','%d','%s','%f','%f','%f','%f')",NewStudent->s_studentName,NewStudent->s_studentAge,
                                                            NewStudent->s_studentNumber,NewStudent->s_ChineseScore,NewStudent->s_EnglishScore,
                                                            NewStudent->s_MathScore,NewStudent->s_averageScore);
                if (returnValue < 0)
                {
                    printf("sprintf add sqlStatement error.\n");
                    return -1;
                }
                else
                {
                    client_send_buffer(sockfd, sqlStatement, 0x01, buffer);
                }
		return 1;

}
int case_delete(student *NewStudent,char *buffer,int sockfd,char *sqlStatement)
{
	int returnValue;
	 deleteOneRecord(NewStudent);

                                if ((returnValue = queryByStudentNumberFirst(NewStudent, buffer, sockfd)) == 0)
                                {
                                        system("clear");
                                        printf("The student number = %s's student is not exits, can't delete.\n", NewStudent->s_studentNumber);
                                        return -1;
                                }

                memset(buffer, 0, sizeof(buffer));
                memset(sqlStatement, 0, sqlStatementLen);

                                returnValue = snprintf(sqlStatement, sqlStatementLen, "delete from student where studentNumber = '%s'", NewStudent->s_studentNumber);
                                if (returnValue < 0)
                                {
                                        printf("sprintf delete sqlStatement error.\n");
                                        return -1;
                                }
                                else
                                {
                                        client_send_buffer(sockfd, sqlStatement, 0x02, buffer);
                                }
					return 1;
}
int case_update(student *NewStudent,char *buffer,int sockfd,char *sqlStatement,char *updateStatement)
{
	int returnValue;
	int updateStatementLen;
	 updateStatement = (char *)malloc(sizeof(char)*100);
                                if (updateStatement == NULL)
                                {
                                        printf("malloc updateStatement failed.\n");
                                        return -1;
                                }

                memset(updateStatement, 0, 100);
                                updateOneRecord(NewStudent, updateStatement);

                                if ((returnValue = queryByStudentNumberFirst(NewStudent, buffer, sockfd)) == 0)
                                {
                                        system("clear");
                                        printf("The student number = %s's student is not exits, can't update.\n", NewStudent->s_studentNumber);
                                        return -1;
                                }

                memset(buffer, 0, sizeof(buffer));
                memset(sqlStatement, 0, sqlStatementLen);

                                updateStatementLen = strlen(updateStatement);
                                updateStatement[updateStatementLen - 1] = ' ';                                                //将最后一个set语句的","改为空格
                                strncpy(sqlStatement, "update student set ", strlen("update student set "));
                                strncat(sqlStatement, updateStatement, updateStatementLen);

                                returnValue = snprintf(updateStatement, 100, "where studentnumber = '%s'", NewStudent->s_studentNumber);
                                if (returnValue < 0)
                                {
                                        printf("update where studentnumber statement error.\n");
                                        return -1;
                                }
                                else
                                {
                                        strncat(sqlStatement, updateStatement, strlen(updateStatement));                   //连接后面的where语句到sqlStatement组成完整的sql语句
                                        printf("the update statement is %s.\n", sqlStatement);

                                        client_send_buffer(sockfd, sqlStatement, 0x03, buffer);
                                }
                                free(updateStatement);
                                updateStatement = NULL;
	return 1;
}
int case_query(student *NewStudent,char *buffer,int sockfd,char *sqlStatement)
{
	int returnValue;
	int queryMode;
	queryMode = queryRecord(NewStudent);
				if(queryMode == 1)
				{
					returnValue = snprintf(sqlStatement, sqlStatementLen, "select * from student where studentname = '%s'", NewStudent->s_studentName);
				}
				else
				{
					returnValue = snprintf(sqlStatement, sqlStatementLen, "select * from student where studentnumber = '%s'", NewStudent->s_studentNumber);
				}

				if(returnValue < 0)
				{
					printf("select statement error.\n");
					return -1;
				}
				else
				{
					client_send_buffer(sockfd, sqlStatement, 0x04, buffer);
				}
			return 1;
}
int receive_process(int sockfd)
{
	int option;
	int actuallyReceivedBytes;
	int originalReceivedBytes;
	int returnValue = 0;
	char Buffer[MAXDATASIZE];
	student *queryStudentInfo;

	actuallyReceivedBytes = 0;
	originalReceivedBytes = 0;
	queryStudentInfo = (student *)malloc(sizeof(student));
	if (queryStudentInfo == NULL)
	{
		printf("malloc queryStudentInfo failed.\n");
		return -1;
	}
	server_recv(sockfd,Buffer,MAXDATASIZE,option);        
	option = Buffer[SizeOfInt];
	switch (option)
	{
		case 0x01:
		case 0x02:
		case 0x03:
		{
			printf("%s\n", (Buffer + bufferHeadLength));
			break;
		}
		case 0x04:
		{
			queryStudentInfo = (student *)(Buffer + bufferHeadLength);
			if ((strncmp(queryStudentInfo->s_studentName, "No record.", strlen("No record.")) == 0)
				|| strncmp(queryStudentInfo->s_studentName, "query failed.", strlen("query failed.")) == 0)
			{
				returnValue = 0;
				printf("%s\n", queryStudentInfo->s_studentName);
				break;
			}
			else
			{
				returnValue = 1;
				printf("student's name is %s\nstudent's age is %d\nstudent's studentnumber is %s\nstudent's chinesescore is %.2f\nstudent's englishscore is %.2f\nstudent's mathscore is %.2f\nstudent's averagescore is %.2f\n",
						queryStudentInfo->s_studentName, queryStudentInfo->s_studentAge, queryStudentInfo->s_studentNumber,
						queryStudentInfo->s_ChineseScore, queryStudentInfo->s_EnglishScore, queryStudentInfo->s_MathScore, queryStudentInfo->s_averageScore);
			}
			break;
		}
		default : printf("received a wrong option\n"); break;
	}
	//free(queryStudentInfo);
	//queryStudentInfo = NULL;
	return returnValue;
}     //receive_process() end
int chooseoption()
{
	int choose;
	printf("----------+--------------+--------------+-------------\n");
	printf("1.add Msg | 2.delete Msg | 3.updata Msg | 4.query Msg\n");
	printf("----------+--------------+--------------+-------------\n");
	
	do
	{
		printf("\t\t\tmake change:");
		scanf("%d", &choose);
		if ((choose < 1) || (choose > 4))
		{
			printf("input the right change\n");
		}
		else
		{
			break;
		}
	}while ((choose < 1) || (choose > 4));

	return choose;
}
int addOneRecord(student *NewStudent)
{
	float averagescore;

	printf("Input the new student's name:");
	scanf("%s", NewStudent->s_studentName);
	printf("Input the new student's age:");
	scanf("%d", &(NewStudent->s_studentAge));
	printf("Input the new student's studentNumber:");
	scanf("%s", NewStudent->s_studentNumber);
	printf("Input the new student's chinese score:");
	scanf("%f", &(NewStudent->s_ChineseScore));
	printf("Input the new student's english score:");
	scanf("%f", &(NewStudent->s_EnglishScore));
	printf("Input the new student's math score:");
	scanf("%f", &(NewStudent->s_MathScore));

	averagescore = (NewStudent->s_ChineseScore + NewStudent->s_EnglishScore + NewStudent->s_MathScore) / 3;
	NewStudent->s_averageScore = averagescore;
	printf("average score is %.2f\n", NewStudent->s_averageScore);
	return 1;
}
int deleteOneRecord(student *NewStudent)
{
	int returnvalus;
	printf("Input the student's Number which you want to delete:");
	scanf("%s", NewStudent->s_studentNumber);
	if(returnvalus < 0)
	{
		printf("fail to deleteOneRecord\n");
		return -1;
	}
	return 1;
}
int updateOneRecord(student *NewStudent, char *updateStatement)
{
	int choose;
	int tempStatementLen = 50;
	int returnValue;
	char *tempStatement;
	tempStatement = (char *)malloc(sizeof(char)*tempStatementLen);
	if(tempStatement == NULL)
	{
		printf("malloc failed.\n");
		return -1;
	}

	printf("Input the student's Number which you want to update:");
	scanf("%s", NewStudent->s_studentNumber);

	do
	{
		printf("\t\t\tNow choose what you want to update:");
		printf("-------+-------+-----------+-----------+---------+-------\n");
		printf("1.name | 2.age | 3.chinese | 4.English | 5 match | 0.quit\n");
		printf("-------+-------+-----------+-----------+---------+-------\n");
		scanf("%d", &choose);
		switch (choose)
		{
			case 0: break;
			case 1:
			{
				printf("Input number is %s student's name:", NewStudent->s_studentNumber);
				scanf("%s", NewStudent->s_studentName);
				returnValue = snprintf(tempStatement, tempStatementLen, "studentname = '%s',", NewStudent->s_studentName);
				if (returnValue < 0)
				{
					printf("update name statement error.\n");
					break;
				}
				strncat(updateStatement, tempStatement, strlen(tempStatement));
				break;
			}
			case 2:
			{
				printf("Input number is %s student's age:", NewStudent->s_studentNumber);
				scanf("%d", &(NewStudent->s_studentAge));
				returnValue = snprintf(tempStatement, tempStatementLen, "age = '%d',", NewStudent->s_studentAge);
				if (returnValue < 0)
				{
					printf("update age statement error.\n");
					break;
				}
				strncat(updateStatement, tempStatement, strlen(tempStatement));
				break;
			}
			case 3:
			{
				printf("Input number is %s student's chinese score:", NewStudent->s_studentNumber);
				scanf("%f", &(NewStudent->s_ChineseScore));
				returnValue = snprintf(tempStatement, tempStatementLen, "chinesescore = '%f',", NewStudent->s_ChineseScore);
				if (returnValue < 0)
				{
					printf("update chinese score statement error.\n");
					break;
				}
				strncat(updateStatement, tempStatement, strlen(tempStatement));
				break;
			}
			case 4:
			{
				printf("Input number is %s student's english score:", NewStudent->s_studentNumber);
				scanf("%f", &(NewStudent->s_EnglishScore));
				returnValue = snprintf(tempStatement, tempStatementLen, "englishscore = '%f',", NewStudent->s_EnglishScore);
				if (returnValue < 0)
				{
					printf("update english score statement error.\n");
					break;
				}
				strncat(updateStatement, tempStatement, strlen(tempStatement));
				break;
			}
			case 5:
			{
				printf("Input number is %s student's math score:", NewStudent->s_studentNumber);
				scanf("%f", &(NewStudent->s_MathScore));
				returnValue = snprintf(tempStatement, tempStatementLen, "mathscore = '%f',", NewStudent->s_MathScore);
				if (returnValue < 0)
				{
					printf("update math score statement error.\n");
					break;
				}
				strncat(updateStatement, tempStatement, strlen(tempStatement));
				break;
			}
			default: printf("choose wrong!\n");break;
		}  //switch end
	}while (choose != 0);
	free(tempStatement);
	tempStatement = NULL;
	return 1;
}  //updateOneRecord() end

int queryRecord(student *NewStudent)
{
	int choose;

	printf("\t\t\t1.query by name\n");
	printf("\t\t\t2.query by studentNumber\n");
	do
	{
		printf("\t\t\tplease choose:");
		scanf("%d", &choose);
		switch (choose)
		{
			case 1:
			{
				printf("Input the student's name which you want to query:");
				scanf("%s", NewStudent->s_studentName);
				break;
			}
			case 2:
			{
				printf("Input the student's number which you want to query:");
				scanf("%s", NewStudent->s_studentNumber);
				break;
			}
			default : printf("choose wrong!\n"); break;
		}
	}while ((choose != 1) && (choose != 2));
	return choose;
}
int queryByStudentNumberFirst(const student *NewStudent, char *buffer, int sockfd)
{
	int returnValue;
	int bufferLength;
	char *sqlStatement;

	sqlStatement = (char *)malloc(sizeof(char)*sqlStatementLen);
	if (sqlStatement == NULL)
	{
		printf("malloc sqlStatement in queryByStudentNumberFirst() failed.\n");
		return -1;
	}

	returnValue = snprintf(sqlStatement, sqlStatementLen, "select * from student where studentnumber = '%s'", NewStudent->s_studentNumber);
	if (returnValue < 0)
	{
		printf("'firt query' select statement error.\n");
	}
	else
	{
	    printf("--'the first query'\n");
		client_send_buffer(sockfd, sqlStatement, 0x04, buffer);

		returnValue = receive_process(sockfd);
	}
	free(sqlStatement);
	sqlStatement = NULL;
	return returnValue;
}
int server_recv(int connfd,char *buffer,int MAXDATASIZEi,int option)
{
	int actuallyReceivedBytes = 0;
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
int client_send_buffer(int sockfd, const char *sqlStatement, char option, char *buffer)
{
	int bufferLength;
	int client_send;
	int send_max = 0;
	bufferHeadLength = sizeof(int) + sizeof(char);

	bufferLength = bufferHeadLength + strlen(sqlStatement);        //int + char + strlen(sqlStatement)

	(*(int *)(buffer)) = htonl(bufferLength);

	buffer[SizeOfInt] = option;
	memcpy((buffer + bufferHeadLength), sqlStatement, strlen(sqlStatement));
	while(bufferLength > send_max)
	{
	client_send = send(sockfd, buffer, bufferLength, 0);
	send_max += client_send;
	}
	if(client_send < 0)
	{
		printf("fail to client send\n");
		return -1;
	}

	printf("to accomplish %c, send %d bytes to server.\n", buffer[SizeOfInt], bufferLength);
	return 1;
}

int init_client_socket(int argc, char *argv[])
{
	int fd;
	struct hostent *he;
	struct sockaddr_in server;


	if (argc != 2)
	{
		printf("Useage:%s,<IP_Address>\n",argv[0]);
		return -1;
	}

	if ((he = gethostbyname(argv[1])) == NULL)
	{
		printf("gethostbyname error.\n");
		return -1;
	}

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("creat socket failed.\n");
		return -1;
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr*)he->h_addr);

	if (connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{
		printf("connect server error.\n");
		return -1;
	}

	return fd;
}

int main(int argc, char *argv[])
{
	int fd;
	int choose;
	next:
	fd = init_client_socket(argc, argv);
	 if(fd < 0)
                {
                        printf("fail to init client socket\n");
                        printf("do you want try agin?(1/-1)\n");
                        scanf("%d",&choose);
                        if(choose == 1)
                        {
                                goto next;
                        }
                }

	while(1)
	{
		send_process(stdin, fd);
	}

	close(fd);
	return 0;
}
