#include "TaskTcpDataRecv.h"

TaskTcpDataRecv::TaskTcpDataRecv(ITcpDataProcess *pTcpDataProcess)
{
	m_pTcpDataProcess = pTcpDataProcess;

	m_pThreadManager = ThreadManager::GetInstance();
}

void TaskTcpDataRecv::TaskPorcess(void *pTask)
{
	if (NULL == pTask)
	{
		return;
	}

	SOCKET recvSocket = (SOCKET)pTask;
	if (NULL==recvSocket || INVALID_SOCKET==recvSocket)
	{
		cout << "Pass a invalid socket to service thread";

		return;
	}

	//�������ݽ���ģʽΪ��������ulMode: 0Ϊ����ģʽ !0Ϊ������ģʽ��
	u_long ulMode = 1;
	if (SOCKET_ERROR == ioctlsocket(recvSocket, FIONBIO, &ulMode))
	{
		cout << "Set recv() not-block mode failed with error: "
			 <<  WSAGetLastError();

		closesocket(recvSocket);

		return;
	}

	//���ڼ��TCP�����Ƿ��쳣�Ͽ��Ķ�ʱ��
	tcp_keepalive inKeepAlive = {0};
	DWORD dwInLen = sizeof(tcp_keepalive);

	tcp_keepalive outKeepAlive = {0};
	DWORD dwOutLen = sizeof(tcp_keepalive);

	//�Ƿ�����tcp_keepalive��ʱ�� 1Ϊ����
	inKeepAlive.onoff = 1;
	//�״�keepalive̽��ǰ�ĵȴ�ʱ�䣨��λ:ms��
	inKeepAlive.keepalivetime = 30000;
	//ÿ��keepalive̽���ʱ��������λ:ms��
	inKeepAlive.keepaliveinterval = 3000;

	DWORD dwBytesReturn = 0;
	//�������TCP�����Ƿ��쳣�Ͽ���tcp_keepalive��ʱ��
	if(SOCKET_ERROR == WSAIoctl(recvSocket, SIO_KEEPALIVE_VALS, 
                                (LPVOID)&inKeepAlive, dwInLen, 
							    (LPVOID)&outKeepAlive, dwOutLen, 
							    &dwBytesReturn, NULL, NULL))
	{
		cout << "Set keep alive TCP Connection failed with error " 
             <<  WSAGetLastError();

		closesocket(recvSocket);

		return;
	}

	if (NULL!=m_pThreadManager && NULL!=m_pTcpDataProcess)
	{
		//ѭ����������
		int iRecvValue = -1;
		unsigned char buffer[MAX_BUFFER_READ_LENGTH];
		memset(buffer, 0, MAX_BUFFER_READ_LENGTH);
		while (false == m_pThreadManager->IsExitThread())
		{
			iRecvValue = recv(recvSocket, (char *)buffer, 
                              MAX_BUFFER_READ_LENGTH, 0);
			//����ѭ��û�����ݵ������������쳣
			if (SOCKET_ERROR == iRecvValue)
			{
				//���߶Ͽ����ͻ��˽����쳣�ȵ��������쳣
				if (WSAECONNRESET == WSAGetLastError())
				{
					break;
				}
				//û�����ݵ��� ����ѭ����������
				else
				{
					Sleep(1);
					continue;
				}
			}
			//�յ�������ͻ��˷���������
			else if (0 < iRecvValue)
			{
				m_pTcpDataProcess->TcpDataProcess((unsigned char *)buffer, 
                                                   iRecvValue, recvSocket);
			}
			//�ͻ�������崻���ϵ�ȵ��������쳣
			else if (0 == iRecvValue )
			{
				break;
			}
		}
	}
	
	closesocket(recvSocket);

	//�����ӷ����̹߳�������ɾ���Լ�
	cout << "Thread will be deleted is:" << GetCurrentThreadId() << endl;
	m_pThreadManager->DeleteThread(GetCurrentThreadId());
}