/**********************************************************

   *��    ����TCP���ݴ��������
   *��    �ã��ṩTCP���ݴ���ӿ�
   *ע�������Ҫ��ͻ��˷��ش�����ʱ�����ӦrecvSocket

**********************************************************/

#ifndef _ITCPDATAPROCESS_H_
#define _ITCPDATAPROCESS_H_

#include <WinSock2.h>
#include <windows.h>

class ITcpDataProcess
{
public:
	virtual ~ITcpDataProcess(){}

	//TCP���ݴ���ӿ�
	virtual void TcpDataProcess(const unsigned char *pTcpData, 
                                const int &iLen, 
                                const SOCKET &recvSocket = NULL) = 0;
};

#endif