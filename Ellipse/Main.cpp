#include <iostream>

#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, 
    HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    int nCmdShow)
{ 
    std::cout << "WTF" << std::endl;
    system("pause");
    return 0;
}