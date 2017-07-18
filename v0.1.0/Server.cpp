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
	char buf_1[BUFFER_SIZE];
	ServerSock mServer_1;
	ClientSock mClient_1;
	mServer_1.Create(false);

	mServer_1.Bind(SERVER_1_PORT);
	
	// 等待客户端连接
  	printf("socket_1_process is waiting for a connection\n");
	
	while(mServer_1.Listen())
	{	
		mServer_1.Accept(mClient_1);
		printf("accepted\n");
		while(mClient_1.m_bConnected)
		{
			printf("connected!\n");			
			memset(buf_1, '\0', sizeof(buf_1));

			if(mClient_1.Recv(buf_1, BUFFER_SIZE) > 0)
			{
				printf("%s\n",buf_1);
			}
			else
			{
				mClient_1.Close();
				mClient_1.m_bConnected = false;
			}		
			if(!strcmp(buf_1,"end"))
			{
				mClient_1.Close();
				mClient_1.m_bConnected = false;
			}
			sleep(1);
		}
	}

}
void* socket_2_process(void*)
{
	char buf_2[BUFFER_SIZE];
    ServerSock mServer_2;
	ClientSock mClient_2;
	mServer_2.Create(false);

	mServer_2.Bind(SERVER_2_PORT);

	
	// 等待客户端连接
  	printf("socket_2_process is waiting for a connection\n");
	while(mServer_2.Listen())
	{	
		mServer_2.Accept(mClient_2);
		printf("accepted\n");
		while(mClient_2.m_bConnected)
		{
			printf("connected!\n");
			memset(buf_2, '\0', sizeof(buf_2));
			if(mClient_2.Recv(buf_2, BUFFER_SIZE) > 0)
			{
				printf("%s\n",buf_2);
			}
			else
			{
				mClient_2.Close();
				mClient_2.m_bConnected = false;
			}
		
			if(!strcmp(buf_2,"end"))
			{
				mClient_2.Close();
				mClient_2.m_bConnected = false;
			}
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
