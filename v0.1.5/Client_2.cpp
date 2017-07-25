#include"Socket.h"

int main(int argc, char *argv[])
{
 	char buf_1[BUFFER_SIZE];
	ClientSock mClient;
	mClient.Create(false);

	mClient.Bind(CLIENT_2_PORT);
	mClient.Connect("127.0.0.1", SERVER_2_PORT);
	FILE * fp;
	fp = fopen("/home/fly/logo1.jpg","wb+");
	fseek(fp, 0, SEEK_SET);
	int flag = 1;
	char file_info[256];
	while(1)
	{	
	
		while(mClient.m_bConnected && flag)
		{
			mClient.Recv(file_info, 256);
			printf("file info is %s\n",file_info);
			int length  = atoi(file_info),size = 0;
			int size_buf = length % BUFFER_SIZE;
			int i = 0, j = length / BUFFER_SIZE;
			printf("file size is %d\n",size_buf);
			while(size < length)
			{
				while(i < j)
				{
					mClient.RecvPicture(buf_1, BUFFER_SIZE);
					fwrite(&buf_1, BUFFER_SIZE, 1, fp);
					size += BUFFER_SIZE;	
					fseek(fp, 0, size);
					i++;
				}
				mClient.RecvPicture(buf_1, size_buf);
				fwrite(&buf_1, size_buf, 1, fp);
				size += size_buf;
				printf("size is %d\n",size);	
				fseek(fp, 0, size);
				
			}
			fclose(fp);
			flag = 0;
			printf("save picture ok\n");
			sleep(1);
		}
		
	}

    return 0;
}
