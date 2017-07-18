SHELL=/bin/sh
TCP_SRV_BIN = srv
TCP_CLN_BIN = cli
TCP_CLN_2_BIN = cli_2
TCP_CLN_SRC = Client.cpp
TCP_CLN_2_SRC = Client_2.cpp
TCP_SRV_SRC = Server.cpp
TCP_SOCKET_SRC = Socket.cpp
all: cross_tcp
cross_tcp: $(TCP_CLN_SRC) $(TCP_SRV_SRC) $(TCP_SOCKET_SRC) $(TCP_CLN_2_SRC)
	g++ $(TCP_SRV_SRC) $(TCP_SOCKET_SRC) -lpthread -o $(TCP_SRV_BIN)
	g++ $(TCP_CLN_SRC) $(TCP_SOCKET_SRC) -o $(TCP_CLN_BIN)
	g++ $(TCP_CLN_2_SRC) $(TCP_SOCKET_SRC) -o $(TCP_CLN_2_BIN)
clean:
	rm -f $(TCP_CLN_BIN) $(TCP_SRV_BIN) $(TCP_CLN_2_BIN) *.o
