/**
 * Initializes components and manages core loop
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Window.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "Graphics/Model.h"

#include <memory>
#include <vector>

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

		void loadGameObjects();

		void cleanupGameObjects();

	private:


		std::unique_ptr<Window> m_window{};
		std::unique_ptr<Input> m_input{};
		std::unique_ptr<Graphics> m_graphics{};

		std::vector<std::unique_ptr<Model>> m_gameObjects{};
	};
}