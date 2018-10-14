#pragma once

#include <SDL_events.h>

#include <functional>
#include <map>
#include <vector>

#include "exception.hpp"

#include <begin_code.h> // use SDL2 packing

namespace sdl
{
///\brief Object that reprent an event captured by SDL
///
///This union has the exact same memory layout as the SDL_Event structure.
///An SDL_Event and a sdl::Event object are "the same bits" in memory if they hold infos about the same event.
///cpp-sdl2 convert the raw SDL_Event into an sdl::Event object to add an object-oriented API around them
union Event
{
	// this is copy-pasted from the definition of SDL_Event in
	// <SDL2/SDL_events.h>
public:
	Uint32					  type;   /**< Event type, shared with all events */
	SDL_CommonEvent			  common; /**< Common event data */
	SDL_WindowEvent			  window; /**< Window event data */
	SDL_KeyboardEvent		  key;	/**< Keyboard event data */
	SDL_TextEditingEvent	  edit;   /**< Text editing event data */
	SDL_TextInputEvent		  text;   /**< Text input event data */
	SDL_MouseMotionEvent	  motion; /**< Mouse motion event data */
	SDL_MouseButtonEvent	  button; /**< Mouse button event data */
	SDL_MouseWheelEvent		  wheel;  /**< Mouse wheel event data */
	SDL_JoyAxisEvent		  jaxis;  /**< Joystick axis event data */
	SDL_JoyBallEvent		  jball;  /**< Joystick ball event data */
	SDL_JoyHatEvent			  jhat;   /**< Joystick hat event data */
	SDL_JoyButtonEvent		  jbutton; /**< Joystick button event data */
	SDL_JoyDeviceEvent		  jdevice; /**< Joystick device change event data */
	SDL_ControllerAxisEvent   caxis;   /**< Game Controller axis event data */
	SDL_ControllerButtonEvent cbutton; /**< Game Controller button event data */
	SDL_ControllerDeviceEvent cdevice; /**< Game Controller device event data */
	SDL_AudioDeviceEvent	  adevice; /**< Audio device event data */
	SDL_QuitEvent			  quit;	/**< Quit request event data */
	SDL_UserEvent			  user;	/**< Custom event data */
	SDL_SysWMEvent			  syswm; /**< System dependent window event data */
	SDL_TouchFingerEvent	  tfinger;  /**< Touch finger event data */
	SDL_MultiGestureEvent	 mgesture; /**< Gesture event data */
	SDL_DollarGestureEvent	dgesture; /**< Gesture event data */
	SDL_DropEvent			  drop;		/**< Drag and drop event data */

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
	///Default construct an event
	Event() = default;

	///////////////////////////////
	// conversion from SDL_Event //
	///////////////////////////////

	///converting constructor to create an sdl::Event from an SDL_Event struct
	Event(SDL_Event const& e) : Event{*reinterpret_cast<Event const*>(&e)} {}

	///Get a const reference to an sdl::Event from an SDL_Event
	static Event const& ref_from(SDL_Event const& e)
	{
		return *reinterpret_cast<Event const*>(&e);
	}

	///Get an non-const reference to an sdl::Event from an SDL_Event
	static Event& ref_from(SDL_Event& e)
	{
		return *reinterpret_cast<Event*>(&e);
	}

	/// \copydoc Event const& ref_from(SDL_Event const* e)
	static Event const& ref_from(SDL_Event const* e)
	{
		return *reinterpret_cast<Event const*>(e);
	}

	/// \copydoc Event& ref_from(SDL_Event& e)
	static Event& ref_from(SDL_Event* e)
	{
		return *reinterpret_cast<Event*>(e);
	}

	/////////////////////////////
	// conversion to SDL_Event //
	/////////////////////////////

	/// Implicit convertion to SDL_Event()
	operator SDL_Event() const
	{
		return *reinterpret_cast<SDL_Event const*>(this);
	}

	/// Get a pointer to an SDL_Event
	SDL_Event const* native_ptr() const
	{
		return reinterpret_cast<SDL_Event const*>(this);
	}

	/// Get a pointer to an SDL_Event
	SDL_Event* native_ptr() { return reinterpret_cast<SDL_Event*>(this); }

	/////////////////////////////
	// SDL2 functions wrapping //
	/////////////////////////////

	///For type safety, we will use these scoped enum values instead of raw numbers like the C api
	enum class State : int
	{
		Query  = SDL_QUERY,
		Ignore = SDL_IGNORE,
		Enable = SDL_ENABLE,
	};

	///Pool for events, return false when there are no more events to poll
	bool poll() { return SDL_PollEvent(native_ptr()); }

	///Wait until next event occur. This will stop the execution of your code until *something* happens
	void wait()
	{
		if (!SDL_WaitEvent(native_ptr())) throw Exception{"SDL_WaitEvent"};
	}

	///Wait until next event occur, or until the given duration expired
	/// \param timeout max duration to wait for in milliseconds
	void wait(int timeout)
	{
		if (!SDL_WaitEventTimeout(native_ptr(), timeout))
		{
			throw Exception{"SDL_WaitEventTimeout"};
		}
	}

	///Push the current event to the list of event to process
	void push() const
	{
		// SDL_PushEvent won't modify it's argument
		if (!SDL_PushEvent(const_cast<SDL_Event*>(native_ptr())))
		{
			throw Exception{"SDL_PushEvent"};
		}
	}

	/// Peek ot next event in the list
	void peek()
	{
		if (SDL_PeepEvents(
				native_ptr(), 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)
			< 0)
		{
			throw Exception{"SDL_PeepEvents"};
		}
	}
};

////////////////////////////////////////
// Internal events queue manipulation //
////////////////////////////////////////

///Return true if there are events in the queue
inline bool has_events()
{
	return SDL_HasEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

///Return true if there are events of a specific type in the queue
///\param type the type of the events you want to check for
inline bool has_events(Uint32 type)
{
	return SDL_HasEvent(type);
}

///Return true if there are events of a specific range of types in the queue
///\param minType lower type boundary of the range
///\param maxType upper type boundary of the range
inline bool has_events(Uint32 minType, Uint32 maxType)
{
	return SDL_HasEvents(minType, maxType);
}

///Pump the event loop from the OS enent system. only call this from the main thread (or the thread taht initialized the video/window systems)
///This is only usefull if you aren't polling or waiting for events
inline void pump_events()
{
	SDL_PumpEvents();
}

///Clear events of a range of types from the event queue
///\param minType lower type boundary of the range
///\param maxType upper type boundary of the range
inline void flush_events(Uint32 minType, Uint32 maxType)
{
	SDL_FlushEvents(minType, maxType);
}

///Clear all events from the event queue
inline void flush_events()
{
	flush_events(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

///Clear events from a specific type from the event queue
inline void flush_events(Uint32 type)
{
	flush_events(type, type);
}

///
inline void add_events(
	std::vector<Event> const& events, Uint32 minType, Uint32 maxType)
{
	// This use of SDL_PeepEvents don't modify the events
	auto array =
		const_cast<SDL_Event*>(reinterpret_cast<SDL_Event const*>(&events[0]));
	if (SDL_PeepEvents(
			array, int(events.size()), SDL_ADDEVENT, minType, maxType)
		< 0)
	{
		throw Exception{"SDL_PeepEvents"};
	}
}

///Add events to the queue
inline void add_events(std::vector<Event> const& events)
{
	add_events(events, SDL_FIRSTEVENT, SDL_LASTEVENT);
}

///Add events of a specific type to the queue
inline void add_events(std::vector<Event> const& events, Uint32 type)
{
	add_events(events, type, type);
}

///Peek at multiple future events
///\param maxEvents max number of events to get
///\param minType lower bound of event type range
///\param maxType upper bound of event type range
inline std::vector<Event> peek_events(
	size_t maxEvents, Uint32 minType, Uint32 maxType)
{
	auto res   = std::vector<Event>(maxEvents);
	auto array = reinterpret_cast<SDL_Event*>(&res[0]);
	if (SDL_PeepEvents(array, int(maxEvents), SDL_PEEKEVENT, minType, maxType)
		< 0)
	{
		throw Exception{"SDL_PeepEvents"};
	}
	return res;
}

///Peek at future events
inline std::vector<Event> peek_events(size_t maxEvents)
{
	return peek_events(maxEvents, SDL_FIRSTEVENT, SDL_LASTEVENT);
}

///Peek events from a specific type
///\param type The type of events to look for
inline std::vector<Event> peek_events(size_t maxEvents, Uint32 type)
{
	return peek_events(maxEvents, type, type);
}

///Get events from the queue
///\prarm maxEvents max number of events to get
///\param minType lower bound of type range
///\param maxType upper bound of type range
inline std::vector<Event> get_events(
	size_t maxEvents, Uint32 minType, Uint32 maxType)
{
	auto res   = std::vector<Event>(maxEvents);
	auto array = reinterpret_cast<SDL_Event*>(&res[0]);
	if (SDL_PeepEvents(array, int(maxEvents), SDL_GETEVENT, minType, maxType)
		< 0)
	{
		throw Exception{"SDL_PeepEvents"};
	}
	return res;
}

///Get events from the queue
///\param type The type of events to look for
///\param maxEvents max number of events to get
inline std::vector<Event> get_events(size_t maxEvents)
{
	return get_events(maxEvents, SDL_FIRSTEVENT, SDL_LASTEVENT);
}

///Get events from a specific type
///\param type The type of events to look for
///\param maxEvents max number of events to get
inline std::vector<Event> get_events(size_t maxEvents, Uint32 type)
{
	return get_events(maxEvents, type, type);
}

//////////////////////
// Events filtering //
//////////////////////

///Event filter object
struct EventFilter
{
	using func_type = bool (*)(void*, Event&);

	void*	 userdata   = nullptr;
	func_type filter	 = nullptr;
	bool	  is_watcher = false;

	EventFilter(func_type filter, void* userdata)
		: filter{filter}, userdata{userdata}
	{
	}

	EventFilter(func_type filter) : filter{filter} {}

	~EventFilter()
	{
		if (is_watcher) del_watcher();
	}

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

#include <close_code.h>
