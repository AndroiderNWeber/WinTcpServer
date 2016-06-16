/**********************************************************

   *��    �����̹߳�����
   *��    �ã�ʵ�ֶ�TCP���ӷ����̵߳�ͳһ����
   *ע�������������ͨ������ģʽʵ��

**********************************************************/

#ifndef _THREADMANAGER_H_
#define _THREADMANAGER_H_

#include <windows.h>
#include <queue>
#include <vector>
#include <iostream>
#include <assert.h>
#include "../FundationClass/Thread/Thread.h"

using namespace std;

class ThreadManager
{
public:
	//�����̹߳�������Ψһ����
	static ThreadManager *GetInstance();

	~ThreadManager();

	//���߳�����������ӷ����߳�
	void AddThread(IThread *pThread);

	//�˳����ӷ����߳�
	void ExitThread(); 

	//�Ƿ��˳����ӷ����߳�
	bool IsExitThread() const;

	//���߳�������ɾ�����ӷ����߳�
	void DeleteThread(const DWORD &dwThreadID);

	//��ȡ���ӷ����߳�����
	size_t GetThreadCount();

private:
	ThreadManager();
	
	//�߳��˳��ȴ�����
	void WaitForThreadExit(const HANDLE &hThreadHandle);

	//���ӷ����̹߳�����
	static void ThreadManageFunc(void *pParam = NULL);

private:
	//���ӷ����̹߳���������
	static ThreadManager *ms_pInstance;

	//���ӷ����߳��˳���ʶ
	bool m_bIsExitThread;

	//�����߳� ����ɾ�����ӷ����߳�
	IThread *m_pManageThread;

	//ɾ�����ӷ����߳��ź���
	HANDLE m_semDeleteThread; 

	//�˳������߳��ź���
	HANDLE m_semExitManageThread;

	//���ӷ����߳����������ٽ���
	vector<IThread *> m_ThreadVector;
	CRITICAL_SECTION m_csThreadVector;

	//���ӷ����߳�ID���м����ٽ���
	queue<DWORD> m_ThreadIDQueue;
	CRITICAL_SECTION m_csThreadIDQueue;
};

#endif