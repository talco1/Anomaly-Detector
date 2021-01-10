
#include "Server.h"

SocketIO::SocketIO(int socketId):socketId(socketId){}

SocketIO::~SocketIO(){}

string SocketIO::read() {
	char buff = '0';
	string str;
	while(true) {
		int n = recv(socketId, &buff, 1, 0);
		if (buff == '\n') { //read one line  until \n
			break;
		}
		str += buff;
	}
	return str;
}

void SocketIO::read(float* f) {
	char buff = '0';
	string str;
	while(true) {
		int n = recv(socketId, &buff, 1, 0);
		if (buff == '\n') {
			break;
		}
		str += buff;
	}
	*f = stof(str);

}
void SocketIO::write(string text) {
	send(socketId, text.c_str(), strlen(text.c_str()), 0);
}

void SocketIO::write(float f) {
	string text = to_string(f);
	send(socketId, text.c_str(), strlen(text.c_str()), 0);
}



Server::Server(int port){
	fd = socket(AF_INET, SOCK_STREAM, 0); // save the file descriptor
	if (fd < 0) {
		throw system_error(errno, std::generic_category(), "Socket failed");
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	//bind and listen
	if(bind(fd, (struct sockaddr*)&server, sizeof(server)) < 0) {	
		throw system_error(errno, std::generic_category(), "bind failed");
	}
	if (listen(fd, 1) < 0) {
		throw "listen faild";
	}
	toStop = false; //initialize flag for loop
}

void Server::start(ClientHandler& ch){
	t = new thread([&ch, this]() {
		socklen_t sizeOfClient = sizeof(client);
		// handle multiple clients
		while (!toStop) {
			//set select to set timeout for the accept
			fd_set rfds;
			struct timeval tv;
			int retval;
			FD_ZERO(&rfds);
			FD_SET(fd, &rfds);
			tv.tv_sec = 5; // 5 seconds
			tv.tv_usec = 0;
			retval = select(fd+1, &rfds, NULL,NULL, &tv);
			if (retval == -1) {
				break; //error in select
			} else if (retval) {
				int aClientId = accept(fd, (struct sockaddr*)&client, &sizeOfClient);
				if (aClientId < 0) {
					throw system_error(errno, std::generic_category(), "accept falied");
				}
				ch.handle(aClientId);
				close(aClientId);
			} // else, no data in 5 seconds
		}
	} );
}

void Server::stop(){
	toStop = true; //change flag for while loop to stop
	t->join(); // do not delete this!
	close(fd);
}

Server::~Server() {
}