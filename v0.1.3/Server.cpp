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
pthread_t handle_image_event_id;

//send Text message
void* handle_image_event_process(void *)
{
	char msg[256];
	ServerSock mServer;
	ClientSock mClient;
	char message[BUFFER_SIZE];
	mServer.Create(false);
	
	mServer.Bind(SERVER_3_PORT);
	
	// 等待客户端连接
  	printf("handle_image_event_process is waiting for a connection\n");
	
	while(mServer.Listen())
	{
		mServer.Accept(mClient);
		printf("Accepted handle image socket.\n");
		
		while(mClient.m_bConnected)
		{
			
			if(mClient.Recv(msg,256) <=0 )
			{
				mClient.Close();
				mClient.m_bConnected = false;			
			}
			printf("handle image signal is %s\n",msg);
			
			strcpy(msg,"receive OK");
			mClient.Send(msg, 256);
			memset(&msg,'\0',256);
			
		}
		sleep(1);	
	}
	
}
//send  processed picture
void* socket_1_process(void*)
{
	ServerSock mServer_1;
	ClientSock mClient_1;
	char message[BUFFER_SIZE];
	mServer_1.Create(false);
	char buf_1[64];
	mServer_1.Bind(SERVER_1_PORT);
	FILE * fp;
	
	// 等待客户端连接
  	printf("socket_1_process is waiting for a connection\n");
	int length  = 0,sum=0,size = 0;
	int flag = 1;
	while(mServer_1.Listen())
	{	
		mServer_1.Accept(mClient_1);
		printf("Accepted CLinet1.\n");
		sum=0;
		size = 0;
		flag = 1;
		bzero(message,sizeof(message));
		fp = fopen("/home/fly/file.jpg","wb+");
		fseek(fp, 0, SEEK_SET);
		while(mClient_1.m_bConnected && flag)
		{			
			
			mClient_1.Recv(buf_1, 64);
			printf("file info is %s\n",buf_1);
			length  = atoi(buf_1);
			while(size = recv(mClient_1.m_nSockfd, message, sizeof(message), 0))
			{
				if(size < 0)
				{
					break;					
				}
				if(sum  < length)
				{
					fwrite(&message,sizeof(char), size,fp);
					sum += size;
					printf("sum size = %d\n",sum);
				}
				if(sum == length)
					break;
			}
			printf("sum------------------------ size = %d\n",sum);
			flag = 0;
			fclose(fp);
			printf("save picture ok\n");
			//printf("%s\n",message);
			mClient_1.m_bConnected =false;
			mClient_1.Close();
		}
		sleep(1);
	}
	

}
//receive picture from clent1(VideoCapture)
void* socket_2_process(void*)
{
    	ServerSock mServer_2;
	ClientSock mClient_2;
	mServer_2.Create(false);
	char len_buf[10];
	mServer_2.Bind(SERVER_2_PORT);
	// 等待客户端连接
	char buffer[BUFFER_SIZE];
	int fd,flen,size;
	FILE * fp;
	char file_name[] = "logo.jpg";
	fp = fopen("/home/fly/logo.jpg","rb");
	if(NULL == fp) 
	{ 
		printf("File:%s Not Found\n", file_name); 
	} 
	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	
	fseek(fp, 0, SEEK_SET);
	sprintf(len_buf,"%d",flen);
	
	/*******************************************/
	int flag = 1;
  	printf("socket_2_process is waiting for a connection\n");
	while(mServer_2.Listen())
	{	
		fp = fopen("/home/fly/logo.jpg","rb");
		fseek(fp, 0, SEEK_SET);
		mServer_2.Accept(mClient_2);
		printf("Accepted Client2\n");
		mClient_2.Send(len_buf, 256);
		printf("size is %s\n",len_buf);
		while(mClient_2.m_bConnected && flag)
		{
			
			
			while(!feof(fp))
			{
			 	size += fread(buffer, 1, 1024, fp);
				mClient_2.Send(buffer, 1024);
				fseek(fp, 0, size);
			}
			flag = 0;
			printf("send the file OK!\n");
			
		}
		fclose(fp);
		sleep(1);
	}
}
//process the picture thread



int main(int argc, char *argv[])
{
	int ret = 0;	
	void *socket_1_process_result;
	void *socket_2_process_result;
	void *handle_image_event_process_result;
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
	ret = pthread_create( &handle_image_event_id,NULL,handle_image_event_process,NULL );
	if(ret != 0)
	{
		perror("socket_2 Thread create failure");
		exit(-1);
	}
	ret = pthread_join( socket_1_id, &socket_1_process_result);
	if(ret != 0)
	{
		perror("socket_1 Thread join failure");
		exit(-1);
	}
	ret = pthread_join( socket_2_id, &socket_2_process_result);
	if(ret != 0)
	{
		perror("socket_2 Thread join failure");
		exit(-1);
	}
	ret = pthread_join( handle_image_event_id, &handle_image_event_process_result);
	if(ret != 0)
	{
		perror("handle_image_event Thread join failure");
		exit(-1);
	}
    return 0;
}
