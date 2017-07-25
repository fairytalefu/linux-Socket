#include"Socket.h"
#define MAXLINE 1492
#define FILE_NAME_LEN 64
struct fileInfo{
    char fileName[FILE_NAME_LEN];
    int len;
};
int getFileInfo(struct fileInfo *file_info)
{
	int flen;
        FILE * fp;
	char file_name[64] = "/home/fly/1.zip";
	strcpy(file_info->fileName,file_name);
	fp = fopen(file_name,"r");
	if(NULL == fp) 
	{ 
		printf("File:%s Not Found\n", file_name); 
	} 
	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	fclose(fp);
	//strncpy(buf_1,file_name,strlen(file_name));
	printf("---------------------------\n");
	printf("length of the picture is  %d\n", flen);
	file_info->len = flen;
	return 1;

}

int openAPicture()
{
	char buffer[256];
	int fd,flen,size;
	FILE * fp;
	char file_name[] = "dog.jpg";
	fp = fopen("/home/fly/1.zip","rb");
	if(NULL == fp) 
	{ 
		printf("File:%s Not Found\n", file_name); 
	} 
	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	printf("length of the picture is %d\n", flen);
	fseek(fp, 0, SEEK_SET);
	while(!feof(fp))
	{
	 	size += fread(buffer, 1, 256, fp);
		fseek(fp, 0, size);
	}
	printf("size of the picture is %d\n", size);
	fclose(fp);
	return 1;
}

int main(int argc, char *argv[])
{
 	char buf_1[BUFFER_SIZE];
	ClientSock mClient;
	mClient.Create(false);

	mClient.Bind(CLIENT_1_PORT);
	
	/********************************************/
	char buffer[1024];
	int fd,flen,size;
	FILE * fp;
	struct fileInfo file_info;
	int status = getFileInfo(&file_info);
	if(status < 0)
	{
		printf("error\n");
		exit(-1);
	}
	fp = fopen(file_info.fileName,"r+");
	if (fp == NULL)  
        {  
            printf("File:\t%s Not Found!\n", file_info.fileName);  
        } 
	/*******************************************/
	mClient.Connect("192.168.1.117", SERVER_1_PORT);
	 
	int flag = 1;
	printf("the file name is %s\n",file_info.fileName);
	//printf("Client1 >: ");
	while(1)
	{
		
		while(mClient.m_bConnected && flag)
		{	
			
			flen = file_info.len;
			sprintf(buf_1,"%d",flen);
			if(mClient.Send(buf_1, strlen(buf_1)+1))
				printf("the file size%s\n",buf_1);
			while((size = fread(buffer, sizeof(char), 1024, fp))>0)
			{
				mClient.Send(buffer, size);
			}
			flag = 0;
			printf("send the file OK!\n");
			fclose(fp);
			mClient.m_bConnected=false;
			mClient.Close();
		}
		sleep(1);
		break;
	}	

    return 0;
}
