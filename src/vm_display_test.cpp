#include "vm.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdint>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

const int mem_size = 0x100000;

VirtualMachine vm(mem_size);
const unsigned int SCREEN_WIDTH = 512;
const unsigned int SCREEN_HEIGHT = 512;


int main() {

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Virtual VGA Display");


    sf::Texture texture;
    texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
    sf::Sprite sprite(texture);



    int main() {
    // Example 64-bit variable
    

    


    



    // std::vector<std::pair<ByteCode, std::uint64_t>> bytecode;
    
    uint64_t variable = 0x123456789ABCDEF0;
    Instruction instruction(variable);

    
    bytecode.push_back({SETH, vm.encodeRegisterAndImmediate(0, 0)});
    bytecode.push_back({SETL, vm.encodeRegisterAndImmediate(0, 0)});
    bytecode.push_back({LOAD, 0});        // Load the interrupt value from memory
    bytecode.push_back({SETL, vm.encodeRegisterAndImmediate(2, 0)});
    bytecode.push_back({SETL, vm.encodeRegisterAndImmediate(3, 1)});
    bytecode.push_back({LOAD, vm.encodeTwoRegisters(0, 2)});
    bytecode.push_back({INT, PRINT_CHAR});
    bytecode.push_back({ADD, vm.encodeThreeRegisters(2, 2, 3)});
    bytecode.push_back({LOAD, vm.encodeTwoRegisters(0, 2)});

    for (int k=0; k < 100; k++) {
        for (int i=0; i < 16; i++) {
            for (int j=0; j < 16; j++) {
                
                int ascii_val;
                if(k % 2 == 0) {
                    ascii_val = i * 16 + j;
                    }
                else{
                    ascii_val = 255 - i * 16 + j;
                }
                bytecode.push_back({SETL, vm.encodeRegisterAndImmediate(1, ascii_val)});
                bytecode.push_back({SETL, vm.encodeRegisterAndImmediate(2, j)});
                bytecode.push_back({SETL, vm.encodeRegisterAndImmediate(3, i)});
                bytecode.push_back({INT, VGA_UPDATE});
                
            }

        }
        bytecode.push_back({NOP, 0});
    }


    vm.load(bytecode);

    sf::Mutex mutex; // Mutex for synchronization

    // Function to be executed in a separate thread
    auto threadFunction = [&]() {
        while (true) {
            sf::Lock lock(mutex); // Lock the mutex to ensure safe access to vm.proceed()

            vm.proceed();
            // display로부터 pixelData를 가져옴


            // sf::sleep(sf::milliseconds(1)); // Adjust sleep duration as needed
        }
    };

    // Launch a separate thread
    sf::Thread thread(threadFunction);
    thread.launch();

















    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        {
            sf::Lock lock(mutex); // Lock the mutex to ensure safe access to window.draw()
            if (vm.getRedrawFlag()) {
                const std::vector<sf::Color>& pixelData = vm.getDisplay().getPixelData();
                const sf::Uint8* pixelDataPtr = reinterpret_cast<const sf::Uint8*>(pixelData.data());

                // 비디오 메모리의 내용을 텍스처로 업데이트
                texture.update(pixelDataPtr);
                vm.setRedrawFlag(false);
            }
            window.clear();
            window.draw(sprite);
            window.display();
        }
    }

    return 0;
}