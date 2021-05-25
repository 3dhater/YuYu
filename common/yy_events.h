#ifndef _YUYU_EVENT_H_
#define _YUYU_EVENT_H_

enum class yyEventType 
{
	Engine,
	Window,
	System,
	User
};

struct yyEvent_User
{
	void * m_data;
	s32 m_id;

	bool is_equal(const yyEvent_User& other) {
		return m_id == other.m_id;
	}
};

struct yyEvent_Engine
{
	s32 i;

	bool is_equal(const yyEvent_Engine& other) {
		return false;
	}
};

struct yyEvent_Window
{
	enum _event {
		size_changed
	}m_event;

	bool is_equal(const yyEvent_Window& other) {
		if (m_event != other.m_event) return false;
		return true;
	}
};

struct yyEvent_System
{
	s32 i;

	bool is_equal(const yyEvent_System& other) {
		return false;
	}
};

struct yyEvent
{
	yyEvent() {
		m_type = yyEventType::Engine;
	}
	yyEventType m_type;

	union {
		yyEvent_Engine m_event_engine;
		yyEvent_Window m_event_window;
		yyEvent_System m_event_system;
		yyEvent_User   m_event_user;
	};

	bool is_equal(const yyEvent& event) {
		if (m_type != event.m_type) return false;
		switch (m_type)
		{
		case yyEventType::Engine:
			return m_event_engine.is_equal(event.m_event_engine);
		case yyEventType::Window:
			return m_event_window.is_equal(event.m_event_window);
		case yyEventType::System:
			return m_event_system.is_equal(event.m_event_system);
		case yyEventType::User:
			return m_event_user.is_equal(event.m_event_user);
		default:
			yyLogWriteWarning("Need to implement!\n");
			YY_PRINT_FAILED;
			break;
		}
		return true;
	}
};

#endif