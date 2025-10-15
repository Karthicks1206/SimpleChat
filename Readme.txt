SimpleChat – Programming Assignment 1

Introduction This is my solution for PA1. The goal was to build a small chat program that runs multiple instances locally and connects them in a ring network. Each instance listens on its own port, forwards messages to the right neighbor, and passes them around until they reach the destination. I used C++17 and Qt6 for the GUI and networking.

What the program does

Opens a chat window with a log area, a destination box, and a message input.
Each process has a unique ID (I usually just use the port number).
Messages are JSON objects with origin, destination, sequence number, and text.
Messages travel clockwise in the ring. Only the destination shows them.
The sequence numbers are enforced so that if I send out-of-order, they still show up in the right order on the receiving side.
I also added a small command-line injector (SimpleInjector) that lets me test propagation and ordering without typing in the GUI.
How to build I tested on macOS and it should also work on Linux.

Install Qt6 and CMake. On Ubuntu for example: sudo apt-get install qt6-base-dev cmake build-essential

From the project folder: mkdir build cd build cmake .. cmake --build . -j

That will give two executables:

SimpleChat (the GUI chat window)
SimpleInjector (headless tester)
How to run Open four terminals and start each node like this:

./SimpleChat --id 9001 --port 9001 --right 9002 & ./SimpleChat --id 9002 --port 9002 --right 9003 & ./SimpleChat --id 9003 --port 9003 --right 9004 & ./SimpleChat --id 9004 --port 9004 --right 9001 &

Now the ring is complete: 9001 → 9002 → 9003 → 9004 → back to 9001.

Testing Propagation: If I send from 9001 with destination 9003, the message goes through 9002 and shows up at 9003. Ordering: If I send two quick messages from 9001 to 9003, they always arrive in order (seq 1 then seq 2). Loopback: If destination = self, the message goes around the whole ring and comes back.

Automated demo: ./SimpleInjector --demo

This does two things:

Sends seq 2 then seq 1 from 9001 to 9003. On 9003 I see them show up in order (seq 1 first, then seq 2).
Sends a message from 9002 to 9004. On 9004 I see “Propagation check…”
Notes I didn’t include the build/ folder in git since it’s machine-specific. If ports are already in use, change them or kill previous processes. If Qt isn’t found automatically, you might need to set CMAKE_PREFIX_PATH depending on your Qt install.

Files in repo src/ (all source code) CMakeLists.txt SimpleInjector.cpp (injector tool) README.txt (this file)

I also took screenshots of multiple nodes running, propagation, and ordering to show that everything works as required.

Author Karthick S Illinois Institute of Technology GitHub: https://github.com/Karthicks1206/SimpleChat