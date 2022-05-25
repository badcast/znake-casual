#pragma once

#include "framework.h"

namespace RoninEngine
{
	enum MouseState
	{
		MouseLeft,
		MouseRight,
		MouseMiddle,
		MouseWheel
	};

	enum KeyboardState
	{
		//The CAPS LOCK light is on.
		CapsLock_On,
		//The key is enhanced.
		Enchanced_Key,
		//The left ALT key is pressed.
		LeftAlt_Pressed,
		//The left CTRL key is pressed.
		LeftCtrl_Pressed,
		//The NUM LOCK light is on.
		NumLock_On,
		//The right ALT key is pressed.
		RightAlt_Pressed,
		//The right CTRL key is pressed.
		RightCtrl_Pressed,
		//The SCROLL LOCK light is on.
		ScrollLock_On,
		//The SHIFT key is pressed.
		Shift_Pressed,
	};
	class input
	{
	private:
		friend class Application;
		static point_t m_mousePoint;
		static char mouseState;
		static char lastMouseState;
		static char mouseWheels;
		static Vec2 m_axis;


		static void Reset()
		{
			mouseState = 0;
			mouseWheels = 0;
		}

	public:
		static SDL_Event* event;
		static void Init_Input()
		{
		}

		static void Update_Input(SDL_Event* e) {

			event = e;
			switch (e->type)
			{
			case SDL_EventType::SDL_MOUSEMOTION:

				m_mousePoint.x = e->motion.x;
				m_mousePoint.y = e->motion.y;
				break;
			case SDL_EventType::SDL_KEYDOWN:
			{
				if (false || e->key.repeat != 0)
				{
					switch (e->key.keysym.sym)
					{
					case SDL_KeyCode::SDLK_a:
					case SDL_KeyCode::SDLK_LEFT:
						m_axis.x = 1;
						break;
					case SDL_KeyCode::SDLK_d:
					case SDL_KeyCode::SDLK_RIGHT:
						m_axis.x = -1;
						break;
					case SDL_KeyCode::SDLK_w:
					case SDL_KeyCode::SDLK_UP:
						m_axis.y = 1;
						break;
					case SDL_KeyCode::SDLK_s:
					case SDL_KeyCode::SDLK_DOWN:
						m_axis.y = -1;
						break;
					}
				}
			}
			case SDL_EventType::SDL_KEYUP:
			{
				if (false && e->key.repeat == 0)
				{

					switch (e->key.keysym.sym)
					{
					case SDL_KeyCode::SDLK_a:
					case SDL_KeyCode::SDLK_LEFT:
					case SDL_KeyCode::SDLK_d:
					case SDL_KeyCode::SDLK_RIGHT:
						m_axis.x = 0;
						break;
					case SDL_KeyCode::SDLK_w:
					case SDL_KeyCode::SDLK_UP:
					case SDL_KeyCode::SDLK_s:
					case SDL_KeyCode::SDLK_DOWN:
						m_axis.y = 0;
						break;
					}
				}
			}
			}


		}

		static void Late_Update()
		{
			const uint8_t* s = SDL_GetKeyboardState(nullptr);
			if (s[SDL_SCANCODE_D] || s[SDL_SCANCODE_RIGHT])
				m_axis.x = 1;
			else if (s[SDL_SCANCODE_A] || s[SDL_SCANCODE_LEFT])
				m_axis.x = -1;
			else
				m_axis.x = 0;

			if (s[SDL_SCANCODE_W] || s[SDL_SCANCODE_UP])
				m_axis.y = 1;
			else if (s[SDL_SCANCODE_S] || s[SDL_SCANCODE_DOWN])
				m_axis.y = -1;
			else
				m_axis.y = 0;

		}

		inline static const bool isMouseUp(int button = 1) {
			return event->type == SDL_MOUSEBUTTONUP && button == event->button.button;
		}
		inline static const bool isMouseDown(int button = 1) {
			return event->type == SDL_MOUSEBUTTONDOWN && button == event->button.button;
		}
		inline static const bool isCapsLock() { return get_key(SDL_SCANCODE_CAPSLOCK); }

		inline static const char wheelRadix() { return mouseWheels; }

		inline static const point_t getMousePoint()
		{
			return m_mousePoint;
		}

		inline static const Vec2 getMousePointF()
		{
			return { static_cast<float>(m_mousePoint.x), static_cast<float>(m_mousePoint.y) };
		}

		inline static const Vec2 get_axis()
		{
			return m_axis;
		}

		inline static const bool get_key(SDL_Scancode code) {
			return static_cast<bool>(SDL_GetKeyboardState(nullptr)[code]);
		}

		inline static const bool get_key_down(SDL_Scancode code) {
			static uint32_t _last_stamp = 0;
			if (event->key.type == SDL_KEYDOWN && event->key.keysym.scancode == code && !event->key.repeat && _last_stamp != event->key.timestamp)
			{
				_last_stamp = event->key.timestamp;
				return true;
			}
			return false;
		}

		inline static const bool get_key_up(SDL_Scancode code) {
			static uint32_t _last_stamp = 0;
			if (event->key.type == SDL_KEYUP && event->key.keysym.scancode == code && !event->key.repeat && _last_stamp != event->key.timestamp)
			{
				_last_stamp = event->key.timestamp;
				return true;
			}
			return false;
		}
	};

}
