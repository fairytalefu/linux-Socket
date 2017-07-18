#include"Socket.h"

int main(int argc, char *argv[])
{
 	char buf_1[BUFFER_SIZE] = "222222222222222";
	ClientSock mClient;
	mClient.Create(false);

	mClient.Bind(CLIENT_1_PORT);

	mClient.Connect("127.0.0.1", SERVER_1_PORT);
	printf("Client1 >: ");
	while(fgets(buf_1, sizeof(buf_1), stdin) != NULL)
	{	
		if(mClient.Send(buf_1, strlen(buf_1)+1))
			printf("send to server ok\n");
		printf("Client1 >: ");
	}

    return 0;
}
