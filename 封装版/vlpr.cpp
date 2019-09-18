/*********************************************************************************
  *Copyright(C),ywx
  *FileName:  vlpr.cpp
  *Author:  ywx
  *Version:  1.0.1
  *Date: Sep,2019 18th
  *Description:vlpr 类的实现
  *Others:
	  使用前请先配置winsock：
	  项目属性->连接器->输入->附加依赖项里加入ws2_32.lib
	  项目->属性->C/C++->语言->符合模式：选择否
  *Function List:
	 1.char * ReadFile(char *pathpic, int &pic_len) 读取图片
	 2.char *  sendPic(char* addr, char * host, char *pathpic, char* picname, int port, string username, string psw) 发送图片
	 3.UnicodeToUtf8 编码转换(保留函数暂时无用)
  *History:  //修改历史记录列表，每条修改记录应包含修改日期、修改者及修改内容简介
	 1.Date:Sep,2019 17th
	   Author:ywx
	   Modification:initial commit
	 2.Date:Sep,2019 18th
	   Author:ywx
	   Modification:封装vlpr类，原lpr.cpp变为vlpr.cpp
**********************************************************************************/

#include <winsock.h>
#include "pch.h"
#include "vlpr.h"

#pragma warning(disable:4996)
using namespace std;

vlpr::vlpr()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) //异步套接字启动命令
		// 版本（次，主）    //返回socket实现细节信息
	{
		system("WSAStartup failed!\n");
		system("pause");
	}
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("152.32.134.49");
	servAddr.sin_port = htons((short)8801);
	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
}


vlpr::~vlpr()
{
	WSACleanup();
}


char* vlpr::UnicodeToUtf8(const wchar_t* unicode)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
	return szUtf8;
}


char * vlpr::ReadFile(char *pathpic, int &pic_len) {
	//将图片读取出来
	FILE *fp = fopen(pathpic, "r");     //打开文件
	if (!fp) {
		MessageBoxA(NULL, "没有找到文件位置", 0, 0);
		return NULL;
	}
	fseek(fp, 0, SEEK_END);  //一直寻找到文件尾部
	pic_len = ftell(fp);  //得到图片的长度
	rewind(fp);  //rewind 将文件指针指向开头
	char *pic_buf = new char[pic_len + 1];  //开辟一个空间在堆上
	memset(pic_buf, 0, pic_len + 1);  //清空文件指针
	//读取文件内容
	fread(pic_buf, sizeof(char), pic_len, fp);
	//测试将文件再保存于D:中
	/*
	MessageBoxA(NULL, "文件开始", 0, 0);
	FILE *fpw = fopen("C:\\AA.jpg","wb");
	fwrite(pic_buf,sizeof(char), pic_len, fpw);
	fclose(fpw); //关闭文件流
	MessageBoxA(NULL, "文件结束", 0, 0);
	*/
	fclose(fp);

	return pic_buf;
}

char * vlpr::sendPic(char* addr, char * host, char *pathpic, char* picname, int port, string username, string psw) {

	//先读取文件流
	//实名图片读取,等级图片读取
	int Spic_len, Dpic_len;
	char *Spic_data = NULL;

	Spic_data = ReadFile(pathpic, Spic_len);
	std::string header("");
	std::string content("");        //实名文件
	std::string nex_content("");    //等级文件
	std::string u_content("");      //用户名
	std::string p_content("");        //密码

	//----------------------post 头开始--------------------------------  
	header += "POST ";
	header += addr;
	header += " HTTP/1.1\r\n";
	header += "Host: ";
	header += host;
	header += "\r\n";
	header += "Connection: Keep-Alive\r\n";
	header += "Accept: */*\r\n";
	header += "Pragma: no-cache\r\n";
	header += "Content-Type: multipart/form-data;boundary=71b23e4066ed\r\n";

	//用户名数据表单
	u_content += "--71b23e4066ed\r\n";
	u_content += "Content-Disposition: form-data; name=\"u\"\r\n\r\n";
	u_content += username + "\r\n";

	//密码数据表单
	p_content += "--71b23e4066ed\r\n";
	p_content += "Content-Disposition: form-data; name=\"p\"\r\n\r\n";
	p_content += psw + "\r\n";

	//发送文件数据
	content += "--71b23e4066ed\r\n";
	content += "Content-Disposition: form-data; name=\"picurl\"; filename=\"";
	content += picname;
	content += "\"\r\n";
	content += "Content-Type: image/jpeg \r\n\r\n";


	//post尾时间戳  
	std::string strContent("\r\n--71b23e4066ed--\r\n");
	char temp[64] = { 0 };
	//注意下面这个参数Content-Length，这个参数值是：http请求头长度+请求尾长度+文件总长度
	// 就分块传送 
	sprintf(temp, "Content-Length: %d\r\n\r\n",
		content.length() + nex_content.length() + p_content.length() + u_content.length() + Spic_len + strContent.length());
	header += temp;
	std::string str_http_request;
	str_http_request.append(header);

	//----------------------post 头结束-----------------------------------
	//发送post头  
	send(sHost, str_http_request.c_str(), str_http_request.length(), 0);
	char fBuff[1024];
	int buffsize = 1024; // 每个数据包存放文件的buffer大小  
	int nStart;//记录post初始位置  
	int nSize;//记录剩余文件大小  
	Sleep(0.2);
	//发送用户名表单
	send(sHost, u_content.c_str(), u_content.length(), 0);
	Sleep(0.2);
	//发送密码表单
	send(sHost, p_content.c_str(), p_content.length(), 0);
	Sleep(0.2);
	//发送尾部
	//发送格式
	send(sHost, content.c_str(), content.length(), 0);
	Sleep(0.2);
	send(sHost, Spic_data, Spic_len, 0);
	Sleep(0.2);

	//如果数据太大，可以使用如下的方式，切割文件发送数据
	 /*
	 for (int i = 0; i < Spic_len; i += bufsize)
	 {
		 nStart = i;
		 if (i + bufsize + 1> Spic_len){
			 nSize = Spic_len - i;
		 }
		 else{
			 nSize = bufsize;
		 }

		 memcpy(fBuff, Spic_data + nStart, nSize);
		 ::send(sHost, fBuff, nSize, 0);
		 Sleep(0.2);   //防止毡包
	 }

	 */

	send(sHost, strContent.c_str(), strContent.length(), 0);

	if (Spic_data == NULL)
	{
		MessageBox(NULL, L"文件数据为空", 0, 0);
		return "NULL";
	}

	char recv_str[40960] = { 0 };
	char recv_str2[40960] = { 0 };
	Sleep(1);
	if (recv(sHost, recv_str, sizeof(recv_str), 0) == -1)
	{
		MessageBox(NULL, L"获取返回值失败！", 0, 0);
		return "NULL";
	}

	Sleep(1);
	if (recv(sHost, recv_str2, sizeof(recv_str), 0) != -1)
	{
		strcat(recv_str, recv_str2);
		memset(recv_str2, 0, sizeof(recv_str2));
	}
	//printf("%s", recv_str);
	//释放内存
	delete Spic_data;

	//查找返回值
	int i = 0, x = 0;;
	while (recv_str[i])
	{
		if (recv_str[i] == '\n')
		{
			x++;
			if (x >= 5)
				break;
		}
		i++;
	}
	if (x < 5)
	{

		return "NULL";
	}
	else
	{
		memcpy(recv_str2, recv_str + i + 1, strlen(recv_str) - i + 1);
	}
	return recv_str2;
}
