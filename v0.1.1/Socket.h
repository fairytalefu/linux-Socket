#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define WELCOME_MESSAGE "Welcome to server!"
#define SERVER_1_PORT 9997
#define SERVER_2_PORT 9998
#define SERVER_3_PORT 9999
#define CLIENT_1_PORT 20001
#define CLIENT_2_PORT 20002

class BaseSock
{
	/*本类和继承类之外不可见*/
public:
	bool m_bUDP;
	bool m_bConnected;
	char m_cHost[BUFFER_SIZE];
	unsigned short m_nPort;
	int m_nOpt;
	
public:
	BaseSock();
	virtual ~BaseSock();
	
	int m_nSockfd;
	bool Create(bool bUDP=false);
	virtual bool Bind(unsigned short nPort);
	virtual void Close();

	virtual bool Send(const char *buf, long buflen);
	virtual long Recv(char *buf, long buflen);
	virtual long RecvPicture(char *buf, long buflen);
	virtual bool Sendto(const char *buf, int len, const struct sockaddr_in* toaddr);
	virtual long RecvFrom(char *buf, int len, struct sockaddr_in *fromaddr);
};

class ClientSock : public BaseSock
{
public:
	ClientSock();
	virtual ~ClientSock();

	virtual bool Connect(const char *host, unsigned short port);

	friend class ServerSock;
};


class ServerSock :  public BaseSock
{
public:
	ServerSock();
	virtual ~ServerSock();

	virtual bool Listen();
	
	virtual bool Accept(ClientSock &client);

	friend class ClientSock;


};
#endif
