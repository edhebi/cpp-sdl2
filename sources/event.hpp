#pragma once

#include <SDL2/SDL_events.h>

#include <vector>
#include <functional>
#include <map>

#include "exception.hpp"

#include <SDL2/begin_code.h> // use SDL2 packing

namespace sdl
{

union Event
{
	// this is copy-pasted from the definition of SDL_Event in <SDL2/SDL_events.h>
public:
	Uint32 type;                    /**< Event type, shared with all events */
	SDL_CommonEvent common;         /**< Common event data */
	SDL_WindowEvent window;         /**< Window event data */
	SDL_KeyboardEvent key;          /**< Keyboard event data */
	SDL_TextEditingEvent edit;      /**< Text editing event data */
	SDL_TextInputEvent text;        /**< Text input event data */
	SDL_MouseMotionEvent motion;    /**< Mouse motion event data */
	SDL_MouseButtonEvent button;    /**< Mouse button event data */
	SDL_MouseWheelEvent wheel;      /**< Mouse wheel event data */
	SDL_JoyAxisEvent jaxis;         /**< Joystick axis event data */
	SDL_JoyBallEvent jball;         /**< Joystick ball event data */
	SDL_JoyHatEvent jhat;           /**< Joystick hat event data */
	SDL_JoyButtonEvent jbutton;     /**< Joystick button event data */
	SDL_JoyDeviceEvent jdevice;     /**< Joystick device change event data */
	SDL_ControllerAxisEvent caxis;      /**< Game Controller axis event data */
	SDL_ControllerButtonEvent cbutton;  /**< Game Controller button event data */
	SDL_ControllerDeviceEvent cdevice;  /**< Game Controller device event data */
	SDL_AudioDeviceEvent adevice;   /**< Audio device event data */
	SDL_QuitEvent quit;             /**< Quit request event data */
	SDL_UserEvent user;             /**< Custom event data */
	SDL_SysWMEvent syswm;           /**< System dependent window event data */
	SDL_TouchFingerEvent tfinger;   /**< Touch finger event data */
	SDL_MultiGestureEvent mgesture; /**< Gesture event data */
	SDL_DollarGestureEvent dgesture; /**< Gesture event data */
	SDL_DropEvent drop;             /**< Drag and drop event data */

	/* This is necessary for ABI compatibility between Visual C++ and GCC
	Visual C++ will respect the push pack pragma and use 52 bytes for
	this structure, and GCC will use the alignment of the largest datatype
	within the union, which is 8 bytes.

	So... we'll add padding to force the size to be 56 bytes for both.
	*/
	Uint8 padding[56];

	/////////////////////////
	// begining of c++ API //
	/////////////////////////

	Event() = default;

	///////////////////////////////
	// conversion from SDL_Event //
	///////////////////////////////

	Event(SDL_Event const& e)
		: Event{ *reinterpret_cast<Event const*>(&e) }
	{
	}

	static Event const& ref_from(SDL_Event const& e)
	{
		return *reinterpret_cast<Event const*>(&e);
	}

	static Event& ref_from(SDL_Event& e)
	{
		return *reinterpret_cast<Event*>(&e);
	}

	static Event const& ref_from(SDL_Event const* e)
	{
		return *reinterpret_cast<Event const*>(e);
	}

	static Event& ref_from(SDL_Event* e)
	{
		return *reinterpret_cast<Event*>(e);
	}

	/////////////////////////////
	// conversion to SDL_Event //
	/////////////////////////////

	operator SDL_Event() const
	{
		return *reinterpret_cast<SDL_Event const*>(this);
	}

	SDL_Event const* native_ptr() const
	{
		return reinterpret_cast<SDL_Event const*>(this);
	}

	SDL_Event* native_ptr()
	{
		return reinterpret_cast<SDL_Event*>(this);
	}

	/////////////////////////////
	// SDL2 functions wrapping //
	/////////////////////////////

	// some type safety
	enum class State : int
	{
		Query  = SDL_QUERY,
		Ignore = SDL_IGNORE,
		Enable = SDL_ENABLE,
	};


	bool poll() { return SDL_PollEvent(native_ptr()); }

	void wait()
	{
		if (!SDL_WaitEvent(native_ptr())) throw Exception{ "SDL_WaitEvent" };
	}

	void wait(int timeout)
	{
		if (!SDL_WaitEventTimeout(native_ptr(), timeout))
		{
			throw Exception{ "SDL_WaitEventTimeout" };
		}
	}

	void push() const
	{
		// SDL_PushEvent won't modify it's argument
		if (!SDL_PushEvent(const_cast<SDL_Event*>(native_ptr())))
		{
			throw Exception{ "SDL_PushEvent" };
		}
	}

	void peek()
	{
		if (SDL_PeepEvents(native_ptr(), 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) < 0)
		{
			throw Exception{ "SDL_PeepEvents" };
		}
	}
};

////////////////////////////////////////
// Internal events queue manipulation //
////////////////////////////////////////

inline bool has_events()
{
	return SDL_HasEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

inline bool has_events(Uint32 type)
{
	return SDL_HasEvent(type);
}

inline bool has_events(Uint32 minType, Uint32 maxType)
{
	return SDL_HasEvents(minType, maxType);
}

inline void pump_events()
{
	SDL_PumpEvents();
}

inline void flush_events(Uint32 minType, Uint32 maxType)
{
	SDL_FlushEvents(minType, maxType);
}

inline void flush_events() { flush_events(SDL_FIRSTEVENT, SDL_LASTEVENT); }

inline void flush_events(Uint32 type) { flush_events(type, type); }

inline void add_events(std::vector<Event> const& events, Uint32 minType, Uint32 maxType)
{
	// This use of SDL_PeepEvents don't modify the events
	auto array = const_cast<SDL_Event*>(reinterpret_cast<SDL_Event const*>(&events[0]));
	if (SDL_PeepEvents(array, int(events.size()), SDL_ADDEVENT, minType, maxType) < 0)
	{
		throw Exception{ "SDL_PeepEvents" };
	}
}

inline void add_events(std::vector<Event> const& events)
{
	add_events(events, SDL_FIRSTEVENT, SDL_LASTEVENT);
}

inline void add_events(std::vector<Event> const& events, Uint32 type)
{
	add_events(events, type, type);
}

inline std::vector<Event> peek_events(size_t maxEvents, Uint32 minType, Uint32 maxType)
{
	auto res = std::vector<Event>(maxEvents);
	auto array = reinterpret_cast<SDL_Event*>(&res[0]);
	if (SDL_PeepEvents(array, int(maxEvents), SDL_PEEKEVENT, minType, maxType) < 0)
	{
		throw Exception{ "SDL_PeepEvents" };
	}
	return res;
}

inline std::vector<Event> peek_events(size_t maxEvents)
{
	return peek_events(maxEvents, SDL_FIRSTEVENT, SDL_LASTEVENT);
}

inline std::vector<Event> peek_events(size_t maxEvents, Uint32 type)
{
	return peek_events(maxEvents, type, type);
}

inline std::vector<Event> get_events(size_t maxEvents, Uint32 minType, Uint32 maxType)
{
	auto res = std::vector<Event>(maxEvents);
	auto array = reinterpret_cast<SDL_Event*>(&res[0]);
	if (SDL_PeepEvents(array, int(maxEvents), SDL_GETEVENT, minType, maxType) < 0)
	{
		throw Exception{ "SDL_PeepEvents" };
	}
	return res;
}

inline std::vector<Event> get_events(size_t maxEvents)
{
	get_events(maxEvents, SDL_FIRSTEVENT, SDL_LASTEVENT);
}

inline std::vector<Event> get_events(size_t maxEvents, Uint32 type)
{
	get_events(maxEvents, type, type);
}

//////////////////////
// Events filtering //
//////////////////////

struct EventFilter
{
	using func_type = bool(*)(void*, Event&);

	void* userdata   = nullptr;
	func_type filter = nullptr;
	bool is_watcher  = false;

	EventFilter(func_type filter, void* userdata)
		: filter{ filter }, userdata{ userdata }
	{
	}

	EventFilter(func_type filter) : filter{ filter } {}

	~EventFilter() { if (is_watcher) del_watcher(); }

	static int call_filter(void* data, SDL_Event* event)
	{
		auto filter = static_cast<EventFilter*>(data);
		return filter->filter(filter->userdata, sdl::Event::ref_from(event));
	}

	void filter_queue() { SDL_FilterEvents(&call_filter, this); }

	void set() { SDL_SetEventFilter(&call_filter, userdata); }

	static void unset() { SDL_FilterEvents(nullptr, nullptr); }

	void add_watcher()
	{
		SDL_AddEventWatch(&call_filter, this);
		is_watcher = true;
	}

	void del_watcher()
	{
		SDL_DelEventWatch(&call_filter, this);
		is_watcher = false;
	}
};

inline Event::State event_state(Uint32 type)
{
	return static_cast<Event::State>(SDL_GetEventState(type));
}

inline void set_event_state(Uint32 type, Event::State state)
{
	SDL_EventState(type, int(state));
}


} // namespace sdl

#include <SDL2/close_code.h>
