#ifndef USER_DATA_H
#define USER_DATA_H

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <exception>

#pragma comment(lib, "Ws2_32.lib")

class UserData {

	public:
		UserData(std::string name, SOCKET sock = INVALID_SOCKET) noexcept : name(name), socket(sock) {}
		UserData() noexcept {}

		std::string getName() noexcept;
	 	void setName(std::string name) noexcept;

		SOCKET getSocket() noexcept;
		void setSocket(SOCKET sock);

	private:
		std::string name;
		SOCKET socket;

};

#endif 
