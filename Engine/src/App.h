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
		 * Called by game script to start engine
		 */
		void run();

		/**
		 * TODO: load objects from scene file instead of manually adding them
		 */
		void loadGameObjects();


	private:


		std::unique_ptr<Window> m_window{};
		std::unique_ptr<Input> m_input{};
		std::unique_ptr<Graphics> m_graphics{};

		/**
		 * TODO: replace model with game object class
		 * Keep below graphics in the load order to ensure smart pointer are
		 * freed before Graphics, due to dependency on logical device pointer
		 */
		std::vector<std::unique_ptr<Model>> m_gameObjects{};
	};
}