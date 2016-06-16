#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "TcpServer/TaskTcpClientListen.h"
#include "TcpServer/TcpDataProcessAlgo.h"
#include "TcpServer/TcpSever.h"

//�߳��˳��ȴ�����
void WaitForThreadExit(const HANDLE &hThreadHandle);

int main()
{
	//��ʼ�ڴ�״̬��¼
	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);

	//�����TCP���ݴ������
	ITcpDataProcess *pTcpDataProcess = new TcpDataProcessAlgo;

	//��Զ�ͻ��˵�TCP������
	TcpServer *pTcpServer = new TcpServer(pTcpDataProcess);

	//�趨ͨ�Ŷ˿ڲ�����TCP�ͻ��˼�������
	int iPort = 32888;
	ITaskProcess *pTaskClientListen = new TaskTcpClientListen(pTcpServer);

	//����TCP�ͻ��˼������߳�
	IThread *pClientListenThread = new Thread(pTaskClientListen, (void *)iPort);
	pClientListenThread->Start();
	
	getchar();

 	pTcpServer->StopListen();

	//�ȴ�TCP�ͻ��˼����߳��˳�
	WaitForThreadExit(pClientListenThread->GetThreadHandle());
	//����TCP�ͻ��˼����߳�
	delete pClientListenThread;

	delete pTaskClientListen;
	delete pTcpServer;
	delete pTcpDataProcess;
	
	//�����ڴ�״̬��¼
	_CrtMemCheckpoint(&s2);
	//�ڴ�й¶���
	if (0 != _CrtMemDifference(&s3, &s1, &s2))
	{
		_CrtMemDumpStatistics(&s3);
	}

	return 0;
}

void WaitForThreadExit(const HANDLE &hThreadHandle)
{
	DWORD dwThreadExitCode;
	while (true)
	{
		//�����߳��˳����ж��߳��Ƿ����˳�
		if (TRUE == GetExitCodeThread(hThreadHandle, &dwThreadExitCode))
		{
			if (STILL_ACTIVE == dwThreadExitCode)
			{
				Sleep(1);
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			Sleep(1);
			continue;
		}
	}
}