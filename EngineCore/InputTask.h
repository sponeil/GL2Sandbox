// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __InputTask_h__
#define __InputTask_h__

interface IInputEventListener
{
public:
	virtual void OnKeyDown(int nKey, int nMod) {}
	virtual void OnKeyUp(int nKey, int nMod) {}
	virtual void OnMouseDown(int nButton) {}
	virtual void OnMouseUp(int nButton) {}
	virtual void OnActivate(bool bActive) {}
	virtual void OnQuit() {}
};

class CInputTask : public CKernelTask, public TSingleton<CInputTask>
{
protected:
	int m_nKeyCount;
	unsigned char *m_pKeys;

	int m_nMouseX, m_nMouseY;
	unsigned int m_nMouseButtons;

	std::list<IInputEventListener *> m_listeners;

	IInputEventListener *m_pConsole;				// The console is not like other listeners, as it overrides the others
	bool m_bConsoleActive;

public:
	DEFAULT_TASK_CREATOR(CInputTask);

	virtual bool Start()
	{
		m_pConsole = NULL;
		m_bConsoleActive = false;
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
		m_pKeys = SDL_GetKeyState(&m_nKeyCount);
		m_nMouseButtons = SDL_GetRelativeMouseState(&m_nMouseX, &m_nMouseY);
		SDL_PumpEvents();
		SDL_PumpEvents();
		return true;
	}

	virtual void Update()
	{
		PROFILE("CInputTask::Update()", 1);
		SDL_Event event;
		while(SDL_PollEvent(&event) > 0)
		{
			std::list<IInputEventListener *>::iterator it = m_listeners.begin();
			switch(event.type)
			{
				case SDL_ACTIVEEVENT:
					while(it != m_listeners.end())
						(*it++)->OnActivate(true);
					break;
				case SDL_QUIT:
					while(it != m_listeners.end())
						(*it++)->OnQuit();
					break;
				case SDL_KEYDOWN:
					if(m_pConsole != NULL && event.key.keysym.sym == SDLK_BACKQUOTE)
					{
						m_bConsoleActive = !m_bConsoleActive;
						break;
					}
					if(m_bConsoleActive)
						m_pConsole->OnKeyDown(event.key.keysym.sym, event.key.keysym.mod);
					else while(it != m_listeners.end())
						(*it++)->OnKeyDown(event.key.keysym.sym, event.key.keysym.mod);
					break;
				case SDL_KEYUP:
					if(m_bConsoleActive)
						m_pConsole->OnKeyUp(event.key.keysym.sym, event.key.keysym.mod);
					else while(it != m_listeners.end())
						(*it++)->OnKeyUp(event.key.keysym.sym, event.key.keysym.mod);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if(m_bConsoleActive)
						m_pConsole->OnMouseDown(event.button.button);
					else while(it != m_listeners.end())
						(*it++)->OnMouseDown(event.button.button);
					break;
				case SDL_MOUSEBUTTONUP:
					if(m_bConsoleActive)
						m_pConsole->OnMouseUp(event.button.button);
					else while(it != m_listeners.end())
						(*it++)->OnMouseUp(event.button.button);
					break;
			}
		}

		//SDL_PumpEvents();
		m_pKeys = SDL_GetKeyState(&m_nKeyCount);
		m_nMouseButtons = SDL_GetRelativeMouseState(&m_nMouseX, &m_nMouseY);
	}

	virtual void Stop()				{}

	bool IsKeyDown(int n)			{ return m_bConsoleActive ? false : m_pKeys[n] != 0; }
	bool IsMouseButtonDown(int n)	{ return m_bConsoleActive ? false : (m_nMouseButtons & SDL_BUTTON(n)) != 0; }
	int GetMouseX()					{ return m_bConsoleActive ? 0 : m_nMouseX; }
	int GetMouseY()					{ return m_bConsoleActive ? 0 : m_nMouseY; }

	void SetConsole(IInputEventListener *pConsole)				{ m_pConsole = pConsole; }
	bool IsConsoleActive()										{ return m_bConsoleActive; }
	void AddInputEventListener(IInputEventListener *pListener)	{ m_listeners.push_back(pListener); }
	void RemoveInputEventListener(IInputEventListener *pListener)
	{
		std::list<IInputEventListener *>::iterator it = m_listeners.begin();
		while(it != m_listeners.end())
		{
			if(*it == pListener)
				it = m_listeners.erase(it);
			else
				it++;
		}
	}
};

#endif // __InputTask_h__
