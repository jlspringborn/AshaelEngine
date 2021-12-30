/**
 * Initializes components and manages core loop
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Window.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

#include <memory>

namespace ash
{
	class App
	{
	public:
		App();
		~App();
		
		/**
		 * @brief called by game script to start engine
		 */
		void run();

	private:


		std::unique_ptr<Window> m_window{};
		std::unique_ptr<Input> m_input{};
		std::unique_ptr<Graphics> m_graphics{};
	};
}