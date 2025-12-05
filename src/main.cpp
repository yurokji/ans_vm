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

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Virtual Machine");


    sf::Texture texture;
    texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
    sf::Sprite sprite(texture);



    // Example 64-bit variable
    std::vector<Instr> bytecode;
    

    
    bytecode.push_back(makeRI(Op::SET, 0, 0));
    bytecode.push_back({Op::LOAD, 0, 0, 0, 0});        // r0 <- mem[r0]
    bytecode.push_back(makeRI(Op::SET, 2, 0));
    bytecode.push_back(makeRI(Op::SET, 3, 1));
    bytecode.push_back({Op::LOAD, 0, 2, 0, 0});        // r0 <- mem[r2]
    bytecode.push_back(makeINT(PRINT_CHAR));
    bytecode.push_back({Op::ADD, 2, 2, 3, 0});         // r2 += r3
    bytecode.push_back({Op::LOAD, 0, 2, 0, 0});

    int ascii_val;
    for (int k=0; k < 100; k++) {
        for (int i=0; i < 16; i++) {
            for (int j=0; j < 16; j++) {
                
                
                if(k % 2 == 0) {
                    ascii_val = i * 16 + j;
                    }
                else{
                    ascii_val = 255 - i * 16 + j;
                }
                bytecode.push_back(makeRI(Op::SET, 1, ascii_val));
                bytecode.push_back(makeRI(Op::SET, 2, j));
                bytecode.push_back(makeRI(Op::SET, 3, i));
                bytecode.push_back(makeINT(VGA_UPDATE));
                
            }

        }
        bytecode.push_back({Op::NOP, 0, 0, 0, 0});
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
