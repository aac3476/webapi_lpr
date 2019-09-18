/*********************************************************************************
  *Copyright(C),ywx
  *FileName:  vlpr.cpp
  *Author:  ywx
  *Version:  1.0.1
  *Date: Sep,2019 18th
  *Description:vlpr ���ʵ��
  *Others:
	  ʹ��ǰ��������winsock��
	  ��Ŀ����->������->����->���������������ws2_32.lib
	  ��Ŀ->����->C/C++->����->����ģʽ��ѡ���
  *Function List:
	 1.char * ReadFile(char *pathpic, int &pic_len) ��ȡͼƬ
	 2.char *  sendPic(char* addr, char * host, char *pathpic, char* picname, int port, string username, string psw) ����ͼƬ
	 3.UnicodeToUtf8 ����ת��(����������ʱ����)
  *History:  //�޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼��޸����ݼ��
	 1.Date:Sep,2019 17th
	   Author:ywx
	   Modification:initial commit
	 2.Date:Sep,2019 18th
	   Author:ywx
	   Modification:��װvlpr�࣬ԭlpr.cpp��Ϊvlpr.cpp
**********************************************************************************/

#include <winsock.h>
#include "pch.h"
#include "vlpr.h"

#pragma warning(disable:4996)
using namespace std;

vlpr::vlpr()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) //�첽�׽�����������
		// �汾���Σ�����    //����socketʵ��ϸ����Ϣ
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
	//��ͼƬ��ȡ����
	FILE *fp = fopen(pathpic, "r");     //���ļ�
	if (!fp) {
		MessageBoxA(NULL, "û���ҵ��ļ�λ��", 0, 0);
		return NULL;
	}
	fseek(fp, 0, SEEK_END);  //һֱѰ�ҵ��ļ�β��
	pic_len = ftell(fp);  //�õ�ͼƬ�ĳ���
	rewind(fp);  //rewind ���ļ�ָ��ָ��ͷ
	char *pic_buf = new char[pic_len + 1];  //����һ���ռ��ڶ���
	memset(pic_buf, 0, pic_len + 1);  //����ļ�ָ��
	//��ȡ�ļ�����
	fread(pic_buf, sizeof(char), pic_len, fp);
	//���Խ��ļ��ٱ�����D:��
	/*
	MessageBoxA(NULL, "�ļ���ʼ", 0, 0);
	FILE *fpw = fopen("C:\\AA.jpg","wb");
	fwrite(pic_buf,sizeof(char), pic_len, fpw);
	fclose(fpw); //�ر��ļ���
	MessageBoxA(NULL, "�ļ�����", 0, 0);
	*/
	fclose(fp);

	return pic_buf;
}

char * vlpr::sendPic(char* addr, char * host, char *pathpic, char* picname, int port, string username, string psw) {

	//�ȶ�ȡ�ļ���
	//ʵ��ͼƬ��ȡ,�ȼ�ͼƬ��ȡ
	int Spic_len, Dpic_len;
	char *Spic_data = NULL;

	Spic_data = ReadFile(pathpic, Spic_len);
	std::string header("");
	std::string content("");        //ʵ���ļ�
	std::string nex_content("");    //�ȼ��ļ�
	std::string u_content("");      //�û���
	std::string p_content("");        //����

	//----------------------post ͷ��ʼ--------------------------------  
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

	//�û������ݱ�
	u_content += "--71b23e4066ed\r\n";
	u_content += "Content-Disposition: form-data; name=\"u\"\r\n\r\n";
	u_content += username + "\r\n";

	//�������ݱ�
	p_content += "--71b23e4066ed\r\n";
	p_content += "Content-Disposition: form-data; name=\"p\"\r\n\r\n";
	p_content += psw + "\r\n";

	//�����ļ�����
	content += "--71b23e4066ed\r\n";
	content += "Content-Disposition: form-data; name=\"picurl\"; filename=\"";
	content += picname;
	content += "\"\r\n";
	content += "Content-Type: image/jpeg \r\n\r\n";


	//postβʱ���  
	std::string strContent("\r\n--71b23e4066ed--\r\n");
	char temp[64] = { 0 };
	//ע�������������Content-Length���������ֵ�ǣ�http����ͷ����+����β����+�ļ��ܳ���
	// �ͷֿ鴫�� 
	sprintf(temp, "Content-Length: %d\r\n\r\n",
		content.length() + nex_content.length() + p_content.length() + u_content.length() + Spic_len + strContent.length());
	header += temp;
	std::string str_http_request;
	str_http_request.append(header);

	//----------------------post ͷ����-----------------------------------
	//����postͷ  
	send(sHost, str_http_request.c_str(), str_http_request.length(), 0);
	char fBuff[1024];
	int buffsize = 1024; // ÿ�����ݰ�����ļ���buffer��С  
	int nStart;//��¼post��ʼλ��  
	int nSize;//��¼ʣ���ļ���С  
	Sleep(0.2);
	//�����û�����
	send(sHost, u_content.c_str(), u_content.length(), 0);
	Sleep(0.2);
	//���������
	send(sHost, p_content.c_str(), p_content.length(), 0);
	Sleep(0.2);
	//����β��
	//���͸�ʽ
	send(sHost, content.c_str(), content.length(), 0);
	Sleep(0.2);
	send(sHost, Spic_data, Spic_len, 0);
	Sleep(0.2);

	//�������̫�󣬿���ʹ�����µķ�ʽ���и��ļ���������
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
		 Sleep(0.2);   //��ֹձ��
	 }

	 */

	send(sHost, strContent.c_str(), strContent.length(), 0);

	if (Spic_data == NULL)
	{
		MessageBox(NULL, L"�ļ�����Ϊ��", 0, 0);
		return "NULL";
	}

	char recv_str[40960] = { 0 };
	char recv_str2[40960] = { 0 };
	Sleep(1);
	if (recv(sHost, recv_str, sizeof(recv_str), 0) == -1)
	{
		MessageBox(NULL, L"��ȡ����ֵʧ�ܣ�", 0, 0);
		return "NULL";
	}

	Sleep(1);
	if (recv(sHost, recv_str2, sizeof(recv_str), 0) != -1)
	{
		strcat(recv_str, recv_str2);
		memset(recv_str2, 0, sizeof(recv_str2));
	}
	//printf("%s", recv_str);
	//�ͷ��ڴ�
	delete Spic_data;

	//���ҷ���ֵ
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
