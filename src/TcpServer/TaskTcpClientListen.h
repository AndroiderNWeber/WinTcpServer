/**********************************************************

   *��    ����TCP�ͻ��˼��������� ������ITaskProcess
   *��    �ã�ΪTCP�ͻ��˼����߳��ṩ��������
   *ע�������

**********************************************************/

#ifndef _TASKTCPCLIENTLISTEN_H_
#define _TASKTCPCLIENTLISTEN_H_

#include "TcpSever.h"
#include "../FundationClass/Thread/ITaskProcess.h"

class TaskTcpClientListen : public ITaskProcess
{
public:
	TaskTcpClientListen(TcpServer *pTcpServer);

	//ʵ��TCP�ͻ��˼�������
	void TaskPorcess(void *pTask);

private:
	//TCP�����������
	TcpServer *m_pTcpServer;
};

#endif