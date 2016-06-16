#include "TcpDataProcessAlgo.h"

void TcpDataProcessAlgo::TcpDataProcess(const unsigned char *pTcpData, 
                                        const int &iLen, 
										const SOCKET &recvSocket)
{
	if (NULL==pTcpData || 0>=iLen)
	{
		return;
	}

	//ģ���㷨����
	Sleep(50);

	if (NULL!=recvSocket && INVALID_SOCKET!=recvSocket)
	{
		//��ͻ��˷��ش�����
		if (true == TcpServer::SendData((char *)pTcpData, iLen, recvSocket))
		{
			cout << "Data: " << pTcpData << " has been processed" << endl;
		}
	}
}