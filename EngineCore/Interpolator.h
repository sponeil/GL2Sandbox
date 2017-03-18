// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __Interpolator_h__
#define __Interpolator_h__


class CInterpolator : public CAutoRefCounter
{
protected:
	float *m_pTarget;	// Points to the value that needs to be updated
	bool m_bPaused;

public:
	typedef TReference<CInterpolator> Ref;
	CInterpolator(float *pTarget=NULL)
	{
		m_pTarget = pTarget;
		m_bPaused = false;
	}
	void Pause()				{ m_bPaused = true; }
	void Resume()				{ m_bPaused = false; }
	virtual bool Update(float dt) = 0;
};

class CInterpolatorTask : public CKernelTask, public TSingleton<CInterpolatorTask>
{
protected:
	std::list<CInterpolator::Ref> m_list;

public:
	DEFAULT_TASK_CREATOR(CInterpolatorTask);

	virtual bool Start()
	{
		return true;
	}

	virtual void Update()
	{
		PROFILE("CInterpolatorTask::Update()", 1);
		float fSeconds = CTimerTask::GetPtr()->GetFrameSeconds();
		for(std::list<CInterpolator::Ref>::iterator i = m_list.begin(); i != m_list.end();)
		{
			if((*i)->Update(fSeconds))
				i++;
			else
				i = m_list.erase(i);
		}
	}

	virtual void Stop()
	{
		m_list.clear();
	}

	void Add(CInterpolator::Ref ref)
	{
		m_list.push_back(ref);
	}

	void Kill(CInterpolator::Ref ref)
	{
		std::list<CInterpolator::Ref>::iterator i = std::find(m_list.begin(), m_list.end(), ref);
		if(i != m_list.end())
			m_list.erase(i);
	}
};

template <class T>
class TTimeInterpolator : public CInterpolator
{
protected:
	float m_fStart, m_fEnd;
	float m_fElapsedTime, m_fTotalTime;
	virtual void Calculate() = 0;

public:
	void Reset()
	{
		m_fElapsedTime = 0;
	}
	virtual bool Update(float dt)
	{
		if(!m_bPaused)
		{
			m_fElapsedTime += dt;
			Calculate();
			if(m_fElapsedTime >= m_fTotalTime)
				return false;
		}
		return true;
	}

	static CInterpolator *Create(float *pTarget, float fTime, float fStart, float fEnd)
	{
		TTimeInterpolator *p = new T();
		p->m_pTarget = pTarget;
		p->m_fElapsedTime = 0;
		p->m_fTotalTime = fTime;
		p->m_fStart = fStart;
		p->m_fEnd = fEnd;
		CInterpolatorTask::GetPtr()->Add(p);
		return p;
	}
};

class CLinearInterpolator : public TTimeInterpolator<CLinearInterpolator>
{
protected:
	virtual void Calculate()
	{
		float fRatio = CMath::Clamp(0.0f, 1.0f, m_fElapsedTime / m_fTotalTime);
		*m_pTarget = m_fStart * (1-fRatio) + m_fEnd * fRatio;
	}
};

class CSqrInterpolator : public TTimeInterpolator<CSqrInterpolator>
{
protected:
	virtual void Calculate()
	{
		float fRatio = CMath::Clamp(0.0f, 1.0f, m_fElapsedTime / m_fTotalTime);
		fRatio *= fRatio;
		*m_pTarget = m_fStart * (1-fRatio) + m_fEnd * fRatio;
	}
};

class CSqrtInterpolator : public TTimeInterpolator<CSqrtInterpolator>
{
protected:
	virtual void Calculate()
	{
		float fRatio = CMath::Clamp(0.0f, 1.0f, m_fElapsedTime / m_fTotalTime);
		fRatio = sqrtf(fRatio);
		*m_pTarget = m_fStart * (1-fRatio) + m_fEnd * fRatio;
	}
};

class CSmoothInterpolator : public TTimeInterpolator<CSqrtInterpolator>
{
protected:
	virtual void Calculate()
	{
		float fRatio = CMath::Clamp(0.0f, 1.0f, m_fElapsedTime / m_fTotalTime);
		fRatio = CMath::Smoothstep(0.0f, 1.0f, fRatio);
		*m_pTarget = m_fStart * (1-fRatio) + m_fEnd * fRatio;
	}
};

#endif // __Interpolator_h__
