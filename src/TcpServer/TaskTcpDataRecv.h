/**********************************************************

   *��    ����TCP���ݻ�ȡ������ ������ITaskProcess
   *��    �ã�ʵ��TCP���ݻ�ȡ����
   *ע�����TCP���ݻ���������ȡ����Ϊ1024�ֽ�

**********************************************************/

#ifndef _TASKTCPDATARECV_H_
#define _TASKTCPDATARECV_H_

#include <WinSock2.h>
#include <mstcpip.h>
#include <windows.h>
#include <iostream>
#include "ITcpDataProcess.h"
#include "ThreadManager.h"
#include "../FundationClass/Thread/ITaskProcess.h"

using namespace std;

class TaskTcpDataRecv : public ITaskProcess
{
public:
	TaskTcpDataRecv(ITcpDataProcess *pTcpDataProcess);

	//ʵ��TCP���ݻ�ȡ����
	virtual void TaskPorcess(void *pTask);

private:
	//TCP���ݴ������
	ITcpDataProcess *m_pTcpDataProcess;

	//���ӷ����̹߳���������
	ThreadManager *m_pThreadManager;

	//TCP���ݻ���������ȡ����
	static const int MAX_BUFFER_READ_LENGTH = 1024;
};

#endif