#include"Socket.h"
#include <semaphore.h>
#include <pthread.h>

pthread_mutex_t sig_mutex = PTHREAD_MUTEX_INITIALIZER;
/*socket_1 command queue*/
sem_t QueueLen;
sem_t signal_1;
sem_t signal_2;
/*photo capture control*/
sem_t photo_sem;

/*thread ID*/
pthread_t queue_id;
pthread_t socket_1_id;
pthread_t socket_2_id;

void* socket_1_process(void*)
{
	ServerSock mServer_1;
	ClientSock mClient_1;
	char message[BUFFER_SIZE];
	mServer_1.Create(false);
	
	mServer_1.Bind(SERVER_1_PORT);
	FILE * fp;
	fp = fopen("/home/fly/file.jpg","wb+");
	fseek(fp, 0, SEEK_SET);
	// 等待客户端连接
  	printf("socket_1_process is waiting for a connection\n");
	
	while(mServer_1.Listen())
	{	
		mServer_1.Accept(mClient_1);
		printf("Accepted CLinet1.\n");
		pthread_mutex_lock(&sig_mutex);
		mClient_1.Recv(message, 256);
		pthread_mutex_unlock(&sig_mutex);
		printf("file info is %s\n",message);
		int length  = atoi(message),size = 0;
		int size_buf = length % BUFFER_SIZE;
		int i = 0, j = length / BUFFER_SIZE;
		int flag = 1;
		while(mClient_1.m_bConnected && flag)
		{			
			
			while(size < length)
			{
				if(i < j)
				{
					
					mClient_1.RecvPicture(message, BUFFER_SIZE);
					
					fwrite(&message, BUFFER_SIZE, 1, fp);
					size += BUFFER_SIZE;	
					fseek(fp, 0, size);
					i++;
					
				}
				else
				{
					
				
					mClient_1.RecvPicture(message, size_buf);
					fwrite(&message, size_buf, 1, fp);
				
					size += size_buf;	
					fseek(fp, 0, size);
				}
			}
			fclose(fp);
			flag = 0;
			printf("save picture ok\n");
			//printf("%s\n",message);
			sleep(1);
		}
	}

}
void* socket_2_process(void*)
{
    	ServerSock mServer_2;
	ClientSock mClient_2;
	mServer_2.Create(false);

	mServer_2.Bind(SERVER_2_PORT);
	// 等待客户端连接
	char buffer[BUFFER_SIZE];
	int fd,flen,size;
	FILE * fp;
	char file_name[] = "dog.jpg";
	fp = fopen("/home/fly/test/linux-Socket/v0.1.1/logo.jpg","rb");
	if(NULL == fp) 
	{ 
		printf("File:%s Not Found\n", file_name); 
	} 
	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	
	fseek(fp, 0, SEEK_SET);
	sprintf(buffer,"%d",flen);
	
	/*******************************************/
	int flag = 1;
  	printf("socket_2_process is waiting for a connection\n");
	while(mServer_2.Listen())
	{	
		mServer_2.Accept(mClient_2);
		printf("Accepted Client2\n");
		mClient_2.Send(buffer, 256);
		printf("size is %s\n",buffer);
		while(mClient_2.m_bConnected && flag)
		{
			
			
			while(!feof(fp))
			{
			 	size += fread(buffer, 1, 1024, fp);
				mClient_2.Send(buffer, 1024);
				fseek(fp, 0, size);
			}
			flag = 0;
			fclose(fp);
			printf("send the file OK!\n");
			sleep(1);
		}
		
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;	
	void *socket_1_process_result;
	void *socket_2_process_result;

	/*initialize the semphore*/
	sem_init(&QueueLen,0,0);
	sem_init(&photo_sem,0,0);
	sem_init(&signal_1,0,0);
	sem_init(&signal_2,0,0);
	
	ret = pthread_create( &socket_1_id,NULL,socket_1_process,NULL );
	if(ret != 0)
	{
		perror("socket_1 Thread create failure");
		exit(-1);
	}
	
	ret = pthread_create( &socket_2_id,NULL,socket_2_process,NULL );
	if(ret != 0)
	{
		perror("socket_2 Thread create failure");
		exit(-1);
	}
	ret = pthread_join( socket_1_id,&socket_1_process_result);
	if(ret != 0)
	{
		perror("socket_1 Thread join failure");
		exit(-1);
	}
	ret = pthread_join( socket_2_id,&socket_2_process_result);
	if(ret != 0)
	{
		perror("socket_1 Thread join failure");
		exit(-1);
	}
    return 0;
}
