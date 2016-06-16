#include "ThreadManager.h"

ThreadManager *ThreadManager::ms_pInstance = NULL;

ThreadManager *ThreadManager::GetInstance()
{
	if (NULL == ms_pInstance)
	{
		ms_pInstance = new ThreadManager;
	}

	return ms_pInstance;
}

ThreadManager::ThreadManager()
{
	/*
	  * ��������InitializeCriticalSectionAndSpinCount
	  * ���ܣ�  �����ת����ʼ���ٽ���
	  * ������  lpCriticalSection    �ٽ�����ַ
	  *         dwSpinCount          CPUʱ�����ڵȴ�����   һ��Ϊ4000
	  *         lMaximumCount        ���������Դ����
	  *         lpName               �ں˶�������          ��ΪNULL
	  * ����ֵ�����Ƿ���һ������ֵ
	*/

	InitializeCriticalSectionAndSpinCount(&m_csThreadVector, 4000);
	InitializeCriticalSectionAndSpinCount(&m_csThreadIDQueue, 4000);

	/*
	  * ��������CreateSemaphore
	  * ���ܣ�  ��������ʼ���ź����ں˶���
	  * ������  lpSemaphoreAttributes   ��ȫ����ָ��       һ��ΪNULL
	  *         lInitialCount           ��ʼ��Դ����
	  *         lMaximumCount           ���������Դ����
	  *         lpName                  �ں˶�������       ��ΪNULL
	  * ����ֵ���ɹ�ʱ�����ź����ں˶����� ʧ��ʱ����NULL
	*/
	m_semDeleteThread = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
	assert(NULL != m_semDeleteThread);

	m_semExitManageThread = CreateSemaphore(NULL, 0, 1, NULL);
	assert(NULL != m_semExitManageThread);

	//��ʼ�����ӷ����߳��˳���ʶ
	m_bIsExitThread = false;

	//��������ɾ�����ӷ����̵߳Ĺ����߳�
	m_pManageThread = new Thread(ThreadManageFunc, this);
	m_pManageThread->Start();
}

ThreadManager::~ThreadManager()
{
	//�ȴ��������ӷ����߳�ɾ�����
	while (0 != GetThreadCount())
	{
		Sleep(1);
	}

	//�ͷ��˳������߳��ź����ź�
	ReleaseSemaphore(m_semExitManageThread, 1, NULL);
	//�ȴ������߳��˳�
	WaitForThreadExit(m_pManageThread->GetThreadHandle());
	//���������߳�
	delete m_pManageThread;

	//�ͷ��ں˶��� ��ֹ�ں�й¶
	CloseHandle(m_semDeleteThread);
	m_semDeleteThread = NULL;

	CloseHandle(m_semExitManageThread);
	m_semExitManageThread = NULL;
	
	DeleteCriticalSection(&m_csThreadVector);
	DeleteCriticalSection(&m_csThreadIDQueue);
}

void ThreadManager::AddThread(IThread *pThread)
{
	if (NULL == pThread)
	{
		return;
	}

	EnterCriticalSection(&m_csThreadVector);
	m_ThreadVector.push_back(pThread);
	cout << "Thread count is: " << m_ThreadVector.size() << endl;
	LeaveCriticalSection(&m_csThreadVector);
}

void ThreadManager::ExitThread()
{
	m_bIsExitThread = true;
}

bool ThreadManager::IsExitThread() const
{
	return m_bIsExitThread;
}

void ThreadManager::DeleteThread(const DWORD &dwThreadID)
{
	EnterCriticalSection(&m_csThreadIDQueue);
	m_ThreadIDQueue.push(dwThreadID);
	LeaveCriticalSection(&m_csThreadIDQueue);

	//�ͷ�ɾ�����ӷ����߳��ź����ź�
	ReleaseSemaphore(m_semDeleteThread, 1, NULL);
}

size_t ThreadManager::GetThreadCount()
{
	EnterCriticalSection(&m_csThreadVector);
	size_t zThreadCount = m_ThreadVector.size();
	LeaveCriticalSection(&m_csThreadVector);

	return zThreadCount;
}

void ThreadManager::WaitForThreadExit(const HANDLE &hThreadHandle)
{
	DWORD dwThreadExitCode = 0;
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

void ThreadManager::ThreadManageFunc(void *pParam)
{
	if (NULL == pParam)
	{
		return;
	}

	ThreadManager *pThisManager = static_cast<ThreadManager *>(pParam);

	//�źŵȴ�����
	HANDLE arrSingalWait[2] = { pThisManager->m_semDeleteThread, 
						        pThisManager->m_semExitManageThread };
	DWORD dwSingalValue = 0;
	DWORD dwThreadID = 0;
	bool bHasThreadID = false;
	bool bHasThread = false;
	while (true)
	{
		//�ȴ�arrSingalWait�е��ź�
		dwSingalValue = WaitForMultipleObjects(2, arrSingalWait, 
                                               false, INFINITE);
		//�յ�ɾ�������߳��ź����ź�
		if (WAIT_OBJECT_0 == dwSingalValue)  
		{
			//�����ӷ����߳�ID����ȡ����ɾ���̵߳�ID
			EnterCriticalSection(&pThisManager->m_csThreadIDQueue);
			bHasThreadID = !pThisManager->m_ThreadIDQueue.empty();
			if (true == bHasThreadID)
			{
				dwThreadID = pThisManager->m_ThreadIDQueue.front();
				pThisManager->m_ThreadIDQueue.pop();
			}
			LeaveCriticalSection(&pThisManager->m_csThreadIDQueue);

			//���߳�����Ѱ�Ҳ�ɾ�����ӷ����߳�
			EnterCriticalSection(&pThisManager->m_csThreadVector);
			bHasThread = !pThisManager->m_ThreadVector.empty();
			if (true==bHasThreadID && true==bHasThread)
			{
				typedef vector<IThread *>::iterator ThreadIter;
				ThreadIter iter = pThisManager->m_ThreadVector.begin();
				for (; iter != pThisManager->m_ThreadVector.end(); ++iter)
				{
					if (dwThreadID == (*iter)->GetThreadID())
					{
						//�ȴ����߳��˳�
						pThisManager->WaitForThreadExit((*iter)->GetThreadHandle());
						//�������߳�
						delete static_cast<IThread *>(*iter);
						//���̵߳�Ԫ�����������߳�
						pThisManager->m_ThreadVector.erase(iter);
						break;
					}
				}
			}
			LeaveCriticalSection(&pThisManager->m_csThreadVector);
		}
		//�յ��˳������߳��ź����ź�
		else
		{
			break;
		}
	}
}