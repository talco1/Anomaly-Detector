/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <thread>
#include "commands.h"
#include "CLI.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h> 
#include <sys/select.h> 

using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
    public:
    virtual void handle(int clientID)=0;
};

// you can add helper classes here and implement on the cpp file
//SocketIO class
class SocketIO:public DefaultIO{
	int socketId;
public:
	SocketIO(int socketId);
	virtual string read();
	virtual void write(string text);
	virtual void write(float f);
	virtual void read(float* f);
	virtual ~SocketIO();
};

// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler:public ClientHandler{
	public:
    virtual void handle(int clientID){
		SocketIO sockIo(clientID);
		CLI cli(&sockIo);
		cli.start();
    }
};

// implement on Server.cpp
class Server {
	thread* t; // the thread to run the start() method in

	// you may add data members
	int fd;
	sockaddr_in server;
	sockaddr_in client;
	bool toStop;
public:
	Server(int port);
	virtual ~Server();
	void start(ClientHandler& ch);
	void stop();
};

#endif /* SERVER_H_ */