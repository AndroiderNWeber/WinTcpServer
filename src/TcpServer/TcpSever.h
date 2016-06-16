/**********************************************************

   *��    ����TCP��������
   *��    �ã�Ϊ��TCP�ͻ����ṩ����
   *ע�����1.���TCP�ͻ��˼�����Ϊ64
              2.�������������ƣ��ܹ���Ⲣ�������߶Ͽ�����
			    ���˽����쳣���ͻ�������崻���ϵ�����

**********************************************************/

#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#pragma comment(lib, "WS2_32")

#include <WinSock2.h>
#include <windows.h>
#include <vector>
#include "ITcpDataProcess.h"
#include "TaskTcpDataRecv.h"
#include "ThreadManager.h"
#include "../FundationClass/Thread/Thread.h"

using namespace std;

class TcpServer
{
public:
	TcpServer(ITcpDataProcess *pTcpDataProcess);
	~TcpServer();

	//��ʼ�����ͻ���
	void StartListen(const int &iPort);

	//ֹͣ�����ͻ���
	void StopListen();

	//��ͻ��˷�������
	static bool SendData(const char *pTcpData, 
                         const int &iLen, 
                         const SOCKET &recvSocket);

private:
	//��ʼ��Socket��WS2_32.dll
	void InitSocketLib(const BYTE &byMinorVersion = 2, 
                       const BYTE &byMajorVersion = 2);

private:
	//�ͻ��˼���Socket
	SOCKET m_socketListen;

	//�������˺Ϳͻ��˵�ַ��Ϣ
	sockaddr_in m_addrServer;
	sockaddr_in m_addrClient;

	//�ͻ��˼���ֹͣ��ʶ
	bool m_bIsStopListen;

	//TCP���ݻ�ȡ�������
	ITaskProcess *m_pTaskProcess;

	//���ӷ����̹߳���������
	ThreadManager *m_pThreadManager;

	//���ͻ��˼�����
	static const int MAX_CLIENT_LISTEN_NUM = 64;
};

#endif