#include "simple_game.hpp"

//std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
    std::cout << "Starting Vulkan Engine..." << std::endl;
    std::cout.flush();
    
    try {
        std::cout << "Creating SimpleGame object..." << std::endl;
        std::cout.flush();
        lve::SimpleGame app{};
        
        std::cout << "Starting app.run()..." << std::endl;
        app.run();  
        
        std::cout << "App finished successfully!" << std::endl;
    } catch(const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch(...) {
        std::cerr << "Unknown exception caught!" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Main function completed successfully!" << std::endl;
    return EXIT_SUCCESS; 
}