/**	
 * Starts the Engine Application
 * 
 * Copyright (C) 2021, Jesse Springborn
 */
#include "App.h"

#include <stdexcept>	// for error handling
#include <iostream>		// for printing error messages
#include <cstdlib>		// for EXIT_FAILURE & EXIT_SUCCESS

#include <memory>

int main()
{
	std::unique_ptr<ash::App> app = std::make_unique<ash::App>();
	

	try
	{
		app->run();
	}
	catch (const std::exception& e)	// exception catch all
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}