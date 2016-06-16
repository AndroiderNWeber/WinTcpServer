#include "TcpSever.h"

TcpServer::TcpServer(ITcpDataProcess *pTcpDataProcess)
{
	m_bIsStopListen = false;

	//TCP�ͻ��˼�������
	m_pTaskProcess = new TaskTcpDataRecv(pTcpDataProcess);

	//��ȡ���ӷ����̹߳���������
	m_pThreadManager = ThreadManager::GetInstance();

	//��ʼ��Socket��WS2_32.dll
	InitSocketLib();
}

TcpServer::~TcpServer()
{
	//�رտͻ��˼���Socket
	if (INVALID_SOCKET != m_socketListen)
	{
		closesocket(m_socketListen);
	}

	//�����Socket��WS2_32.dll�İ󶨲��ͷ���Դ
	WSACleanup();

	//�������ӷ����̹߳���������
	delete m_pThreadManager;

	//����TCP�ͻ��˼�������
	delete m_pTaskProcess;
}

void TcpServer::StartListen(const int &iPort)
{
	if (0 > iPort)
	{
		return;
	}

	//��ʼ���ͻ��˼���Socket
	m_socketListen = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == m_socketListen)
	{
		cout << "socket() failed with error: " << WSAGetLastError();

		return;
	}

	//�󶨷������˵�ַ��Ϣ
	m_addrServer.sin_family = AF_INET;
	m_addrServer.sin_port = htons(iPort);
	m_addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (SOCKET_ERROR == bind(m_socketListen, (SOCKADDR *)&m_addrServer, 
                             sizeof(SOCKADDR)))
	{
		cout << "bind() failed with error: " << WSAGetLastError();

		return;
	}

	//����Socket����ģʽΪ��������ulMode: Ϊ0����ģʽ !0Ϊ������ģʽ��
	u_long ulMode = 1;
	if (SOCKET_ERROR == ioctlsocket(m_socketListen, FIONBIO, &ulMode))
	{
		cout << "Set accept() non-block mode failed with error: " 
			<<  WSAGetLastError();

		return;
	}

	//��ʼ�����ͻ�������
	listen(m_socketListen, MAX_CLIENT_LISTEN_NUM);

	if (NULL!=m_pThreadManager && NULL!=m_pTaskProcess)
	{
		//ѭ�����տͻ�������
		int iAddrLen = sizeof(m_addrClient);
		IThread *pServiceThread = NULL;
		while (false == m_bIsStopListen)
		{
			//���տͻ�������
			SOCKET newSocket = accept(m_socketListen, 
                                     (SOCKADDR *)&m_addrClient, 
									  &iAddrLen);
			//û�������ӵ���
			if (SOCKET_ERROR == newSocket)
			{
				Sleep(1);
				continue;
			}
			//�����ӵ���
			else
			{
				//�������ӵ�Socket�������̴߳���
				pServiceThread = new Thread(m_pTaskProcess, (void *)newSocket);
				pServiceThread->Start();

				//�������ӷ����̷߳����̹߳�����
				m_pThreadManager->AddThread(pServiceThread);
			}
		}
	}
}

void TcpServer::StopListen()
{
	//ֹͣ�����ͻ�������
	m_bIsStopListen = true;

	//�˳��Ѵ��������ӷ����߳�
	m_pThreadManager->ExitThread();
}

bool TcpServer::SendData(const char *pTcpData, const int &iLen, 
	                     const SOCKET &recvSocket)
{
	if (NULL==pTcpData || 0>=iLen || NULL==recvSocket || 
		INVALID_SOCKET==recvSocket)
	{
		return false;
	}

	//��������
	send(recvSocket, pTcpData, iLen, 0);

	return true;
}

void TcpServer::InitSocketLib(const BYTE &byMinorVersion, 
                              const BYTE &byMajorVersion)
{
	WSADATA wsaData;
	WORD wSockVersion = MAKEWORD(byMinorVersion, byMajorVersion);

	//��ʼ��Socket��WS2_32.dll
	if (0 != WSAStartup(wSockVersion, &wsaData))
	{
		exit(0);
	}
}