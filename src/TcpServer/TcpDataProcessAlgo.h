/**********************************************************

   *��    ����TCP���ݴ���ʵ���� ������ITcpDataProcess
   *��    �ã�ʵ�־����TCP���ݴ���
   *ע����������ع��캯���Դ������ݴ�������Ķ�������

**********************************************************/

#ifndef _TCPDATAPROCESSALGO_H_
#define _TCPDATAPROCESSALGO_H_

#include <iostream>
#include "TcpSever.h"
#include "ITcpDataProcess.h"

using namespace std;

class TcpDataProcessAlgo : public ITcpDataProcess
{
public:
	//ʵ�־����TCP���ݴ���
	virtual void TcpDataProcess(const unsigned char *pTcpData, 
                                const int &iLen, 
                                const SOCKET &recvSocket = NULL);
};

#endif