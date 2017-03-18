// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __Kernel_h__
#define __Kernel_h__


class CKernelTask : public CAutoRefCounter
{
protected:
	unsigned short m_nPriority;
	bool m_bKillFlag;

	CKernelTask()
	{
		m_nPriority = 0;
		m_bKillFlag = false;
	}

public:
	typedef TReference<CKernelTask> Ref;
	void SetKillFlag()				{ m_bKillFlag = true; }
	bool GetKillFlag()				{ return m_bKillFlag; }
	unsigned short GetPriority()	{ return m_nPriority; }

	virtual bool Start()=0;
	virtual void Stop()=0;
	virtual void Update()=0;
	virtual void OnSuspend() {}
	virtual void OnResume() {}
};

#define DEFAULT_TASK_CREATOR(class_name)\
public:\
	typedef TReference<class_name> Ref;\
	static class_name *Create(int nPriority)\
	{\
		LogDebug("Creating kernel task %s", #class_name);\
		class_name *pTask = new class_name();\
		pTask->m_nPriority = nPriority;\
		return pTask;\
	}


class CKernel : public TSingleton<CKernel>
{
protected:
	std::list<CKernelTask::Ref> m_lTasks;
	std::list<CKernelTask::Ref> m_lPaused;

	void Insert(CKernelTask::Ref task)
	{
		// Insert the new task in priority order
		std::list<CKernelTask::Ref>::iterator it;
		for(it=m_lTasks.begin(); it!=m_lTasks.end(); it++)
		{
			if((*it)->GetPriority() > task->GetPriority())
				break;
		}
		m_lTasks.insert(it, task);
	}

	CKernel()
	{
		SDL_Init(0);
	}
	~CKernel()
	{
		SDL_Quit();
	}

public:
	static CKernel *Create()
	{
		return new CKernel();
	}
	static void Destroy()
	{
		delete CKernel::GetPtr();
	}

	bool IsTaskRunning(CKernelTask::Ref task)
	{
		for(std::list<CKernelTask::Ref>::iterator i=m_lTasks.begin(); i!=m_lTasks.end(); i++)
		{
			if(*i == task)
				return true;
		}
		return false;
	}

	bool AddTask(CKernelTask::Ref task)
	{
		if(!task->Start())
			return false;
		Insert(task);
		return true;
	}

	void RemoveTask(CKernelTask::Ref task)
	{
		for(std::list<CKernelTask::Ref>::iterator i=m_lTasks.begin(); i!=m_lTasks.end(); i++)
		{
			if(*i == task)
				task->SetKillFlag();
		}
	}

	void SuspendTask(CKernelTask::Ref task)
	{
		std::list<CKernelTask::Ref>::iterator i = std::find(m_lTasks.begin(), m_lTasks.end(), task);
		if(i != m_lTasks.end())
		{
			m_lTasks.erase(i);
			m_lPaused.push_back(task);
		}
	}

	void ResumeTask(CKernelTask::Ref task)
	{
		std::list<CKernelTask::Ref>::iterator i = std::find(m_lPaused.begin(), m_lPaused.end(), task);
		if(i != m_lPaused.end())
		{
			m_lPaused.erase(i);
			Insert(task);
		}
	}

	void KillAllTasks()
	{
		for(std::list<CKernelTask::Ref>::iterator i=m_lTasks.begin(); i!=m_lTasks.end(); i++)
			(*i)->SetKillFlag();
	}

	int Execute()
	{
		while(!m_lTasks.empty())
		{
			// Loop to execute tasks, removing any dead ones
			PROFILE("Kernel task loop", 0);
			std::list<CKernelTask::Ref>::iterator i = m_lTasks.begin();
			while(i != m_lTasks.end())
			{
				if((*i)->GetKillFlag())
				{
					(*i)->Stop();
					i = m_lTasks.erase(i);
				}
				else
				{
					(*i)->Update();
					i++;
				}
			}
		}
		return 0;
	}
};

#endif // __Kernel_h__
