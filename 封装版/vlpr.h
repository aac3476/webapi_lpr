#pragma once
#include<string>
#include <winsock.h>
class vlpr
{
public:
	vlpr();
	~vlpr();
	char * sendPic(char* addr, char * host, char *pathpic, char* picname, int port, std::string username, std::string psw);
private:
	int sHost;
	SOCKADDR_IN  servAddr;
	int retVal;
	_Out_ char *rebuf;
	char * ReadFile(char *pathpic, int &pic_len);
	char* UnicodeToUtf8(const wchar_t* unicode);
};

