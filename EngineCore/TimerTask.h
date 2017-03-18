// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __TimerTask_h__
#define __TimerTask_h__


#ifdef _WIN32

// In Windows SDL_GetTicks causes the camera to "jump".
// Use QueryPerformanceCounter instead.
class CTimerTask : public CKernelTask, public TSingleton<CTimerTask>
{
protected:
	double m_dFactor;
	LARGE_INTEGER m_nGameStart, m_nFrameStart;

	// If set, locks the frame rate (for video capture)
	unsigned long m_nLockedRate;

	// Some clients will work with game time
	unsigned long m_nGameTicks;
	float m_fGameSeconds;

	// Some clients will work with frame time
	unsigned long m_nFrameTicks;
	float m_fFrameSeconds;

public:
	void LockFrameRate(unsigned long nTicks)	{ m_nLockedRate = nTicks; }
	void UnlockFrameRate()						{ m_nLockedRate = 0; }
	unsigned long GetGameTicks()	{ return m_nGameTicks; }
	float GetGameSeconds()			{ return m_fGameSeconds; }
	unsigned long GetFrameTicks()	{ return m_nFrameTicks; }
	float GetFrameSeconds()			{ return m_fFrameSeconds; }

	DEFAULT_TASK_CREATOR(CTimerTask);

	virtual bool Start()
	{
		m_nLockedRate = 0;
		LARGE_INTEGER nFrequency;
		::QueryPerformanceFrequency(&nFrequency);
		m_dFactor = 1.0 / nFrequency.QuadPart;

		::QueryPerformanceCounter(&m_nGameStart);
		m_nFrameStart.QuadPart = m_nGameStart.QuadPart;
		m_nGameTicks = m_nFrameTicks = 0;
		m_fGameSeconds = m_fFrameSeconds = 0.0f;
		return true;
	}

	virtual void Update()
	{
		PROFILE("CTimerTask::Update()", 1);
		if(m_nLockedRate)
		{
			m_nFrameTicks = m_nLockedRate;
			m_fFrameSeconds = m_nFrameTicks * 0.001f;
			m_nGameTicks += m_nLockedRate;
			m_fGameSeconds = m_nGameTicks * 0.001f;
			return;
		}

		LARGE_INTEGER nPreviousFrame;
		nPreviousFrame.QuadPart = m_nFrameStart.QuadPart;
		::QueryPerformanceCounter(&m_nFrameStart);
		m_fFrameSeconds = (float)((m_nFrameStart.QuadPart - nPreviousFrame.QuadPart) * m_dFactor);
		m_nFrameTicks = (unsigned long)(m_fFrameSeconds * 1000.0f + 0.5f);

		m_fGameSeconds = (float)((m_nFrameStart.QuadPart - m_nGameStart.QuadPart) * m_dFactor);
		m_nGameTicks = (unsigned long)(m_fGameSeconds * 1000.0f + 0.5f);
	}

	virtual void Stop()
	{
	}
};

#else

class CTimerTask : public CKernelTask, public TSingleton<CTimerTask>
{
protected:
	// Some clients will work with game time
	unsigned long m_nGameStart;
	unsigned long m_nGameTicks;
	float m_fGameSeconds;

	// Some clients will work with frame time
	unsigned long m_nFrameStart;
	unsigned long m_nFrameTicks;
	float m_fFrameSeconds;

public:
	unsigned long GetGameTicks()	{ return m_nGameTicks; }
	float GetGameSeconds()			{ return m_fGameSeconds; }
	unsigned long GetFrameTicks()	{ return m_nFrameTicks; }
	float GetFrameSeconds()			{ return m_fFrameSeconds; }

	DEFAULT_TASK_CREATOR(CTimerTask);

	virtual bool Start()
	{
		m_nGameStart = m_nFrameStart = SDL_GetTicks();
		m_nGameTicks = m_nFrameTicks = 0;
		m_fGameSeconds = m_fFrameSeconds = 0.0f;
		return true;
	}

	virtual void Update()
	{
		PROFILE("CTimerTask::Update()", 1);
		unsigned long nPreviousFrame = m_nFrameStart;
		m_nFrameStart = SDL_GetTicks();

		m_nFrameTicks = m_nFrameStart - nPreviousFrame;
		m_fFrameSeconds = m_nFrameTicks * 0.001f;

		m_nGameTicks = m_nFrameStart - m_nGameStart;
		m_fGameSeconds = m_nGameTicks * 0.001f;
	}

	virtual void Stop()
	{
	}
};

#endif

#endif // __TimerTask_h__
