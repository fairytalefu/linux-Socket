#include"Socket.h"

int main(int argc, char *argv[])
{
 	char buf_1[BUFFER_SIZE] = "11111111111";
	ClientSock mClient;
	mClient.Create(false);

	mClient.Bind(CLIENT_2_PORT);

	mClient.Connect("127.0.0.1", SERVER_2_PORT);
	printf("Client2 >: ");
	while(fgets(buf_1, sizeof(buf_1), stdin) != NULL)
	{	
		if(mClient.Send(buf_1, strlen(buf_1)+1))
			printf("send to server ok\n");
		sleep(1);
		printf("Client2 >: ");
	}

    return 0;
}
