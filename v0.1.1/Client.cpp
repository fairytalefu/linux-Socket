#include"Socket.h"

int openAPicture()
{
	char buffer[256];
	int fd,flen,size;
	FILE * fp;
	char file_name[] = "dog.jpg";
	fp = fopen("/home/fly/test/linux-Socket/v0.1.1/dog.jpg","rb");
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
	char file_name[] = "dog.jpg";
	fp = fopen("/home/fly/test/linux-Socket/v0.1.1/dog.jpg","rb");
	if(NULL == fp) 
	{ 
		printf("File:%s Not Found\n", file_name); 
	} 
	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	//strncpy(buf_1,file_name,strlen(file_name));
	printf("length of the picture is %d\n", flen);
	mClient.Connect("127.0.0.1", SERVER_1_PORT);
	
	
	fseek(fp, 0, SEEK_SET);
	sprintf(buf_1,"%d",flen);
	if(mClient.Send(buf_1, strlen(buf_1)+1))
		printf("the file size\n");
	/*******************************************/
	int flag = 1;
	//printf("Client1 >: ");
	while(1)
	{
		while(mClient.m_bConnected && flag)
		{	
			
			while(!feof(fp))
			{
			 	size += fread(buffer, 1, 1024, fp);
				mClient.Send(buffer, 1024);
				fseek(fp, 0, size);
			}
			flag = 0;
			fclose(fp);
			printf("send the file OK!\n");
		
		}
	}	

    return 0;
}
