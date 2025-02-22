#include "./UserData.h"

std::string UserData::getName() noexcept {
	return this->name;
}

void UserData::setName(std::string name) noexcept {
	this->name = name;
}

SOCKET UserData::getSocket() noexcept {
	return this->socket;
}

void UserData::setSocket(SOCKET sock) {
	if (this->socket != INVALID_SOCKET) {
		this->socket = sock;
	} else {
		throw std::exception("ERR::Socket is fully");
	}
}