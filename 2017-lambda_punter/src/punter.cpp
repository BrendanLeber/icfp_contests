#include <array>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#include "json.hpp"
#pragma clang diagnostic pop


int connect_to_game_server(std::string port);
std::string format_data(nlohmann::json& bundle);
void handshake(int sock);


int connect_to_game_server(std::string port)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; //SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* result;
	int s = getaddrinfo("punter.inf.ed.ac.uk", port.c_str(), &hints, &result);
	if (s != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(s) << '\n';
		return -1;
	}
	
	int sock = -1;
	std::array<char, INET_ADDRSTRLEN> addr;
	std::cerr << "attempting to connect to " 
		<< inet_ntop(result->ai_addr->sa_family, result->ai_addr->sa_data, addr.data(), addr.size())
		<< '\n';

	sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sock == -1) {
		std::cerr << "socket: " << strerror(errno) << '\n';
	}
	else if (connect(sock, result->ai_addr, result->ai_addrlen) != 0) {
		std::cerr << "connect: " << strerror(errno) << '\n';
		close(sock);
		sock = -1;
	}
	else {
		std::cerr << "connected.\n";
	}

	freeaddrinfo(result);

	return sock;
}

std::string format_data(nlohmann::json& bundle)
{
	std::stringstream ss_bundle;
	ss_bundle << bundle;
	auto str_bundle = ss_bundle.str();

	std::stringstream ss_data;
	ss_data << str_bundle.size() << ':' << str_bundle;

	return ss_data.str();
}

void handshake(int sock)
{
	nlohmann::json announcement;
	announcement["me"] = "the_holly_court";

	auto data = format_data(announcement);

	// send announcement
	if (sock != 0) {
		auto cch = send(sock, data.data(), data.size(), 0);
		if (cch == -1) {
			std::cerr << "send: " << strerror(errno) << '\n';
			exit(EXIT_FAILURE);
		}
	}
	else {
		std::cout << data << '\n';
	}

	// receive response
	if (sock != 0) {
		// TODO(bml) setup receive buffer
		size_t cch = 10;
		std::vector<char> buff(cch);
		auto ret = recv(sock, buff.data(), cch, 0);
		if (ret == -1) {
			std::cerr << "recv: " << strerror(errno) << '\n';
			exit(EXIT_FAILURE);
		}
	}
	else {
		int num_chars;
		std::cin >> num_chars;
	}

	// TODO(bml) compare response to announcement
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Syntax: " << argv[0] << " <port>\n";
		return EXIT_FAILURE;
	}

	int sock = 0;
	if (std::stoi(argv[1]) != 0) {
		sock = connect_to_game_server(argv[1]);
		if (sock <= 0) {
			return EXIT_FAILURE;
		}
	}

	handshake(sock);
	
	if (sock != 0) {
		close(sock);
		//sock = 0;
	}

	return EXIT_SUCCESS;
}
