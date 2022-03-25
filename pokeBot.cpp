#define WINVER 0x0500
#define KEYEVENTF_KEYDOWN  0x0000
#include <iostream>
#include <windows.h>
#include <time.h>

using namespace std;


const float screenScaling = 1.25;

struct ColorValues {
    int R, G, B;

    ColorValues(int R, int G, int B)
        : R(R), G(G), B(B) {};
    
    bool operator== (ColorValues other)
    {
        return (*this).R == other.R && (*this).G == other.G && (*this).B == other.B;
    }

    void printColor()
    {
        cout << "RGB(" << R << ", " << G << ", " << B << ") " << endl;
    }

};

const ColorValues pokeColorHover(57, 130, 228);
const ColorValues pokeColor(35, 116, 225);



namespace input {

    INPUT createKeyboardInput(WORD key)
    {
        INPUT ip{};
        ip.type = INPUT_KEYBOARD;
        ip.ki.wScan = 0; 
        ip.ki.time = 0;
        ip.ki.dwExtraInfo = 0;
        ip.ki.wVk = key;

        return ip;
    }

    void sendInput(INPUT &input, UINT ACTION)
    {
        input.mi.dwFlags = ACTION;
        input.ki.dwFlags = ACTION;
        SendInput(1, &input, sizeof(INPUT));
    }

}

void pressKey(WORD key)
{
    INPUT input = input::createKeyboardInput(key);

    input::sendInput(input, KEYEVENTF_KEYDOWN);
    input::sendInput(input, KEYEVENTF_KEYUP);
}

void mouseClick(int x, int y)
{
    SetCursorPos(x/screenScaling, y/ screenScaling);

    INPUT input{};
    input.type = INPUT_MOUSE;

    input::sendInput(input, MOUSEEVENTF_LEFTDOWN);
    input::sendInput(input, MOUSEEVENTF_LEFTUP);
}



ColorValues getPixelColor(const int x, const int y)
{
    HDC hdcScreen(GetDC(NULL));
    COLORREF pixel = GetPixel(hdcScreen, x, y);
    ReleaseDC(NULL, hdcScreen);

    ColorValues pixelVal(GetRValue(pixel), GetGValue(pixel), GetBValue(pixel));
    return pixelVal;
}

POINT getCursorPos()
{
    POINT point;
    GetCursorPos(&point);
    point.x *= screenScaling;
    point.y *= screenScaling;
    return point;
}

POINT getCurrCursorPos()
{
    cout << "Press \"Insert\" key to get pixel cursor position \n";
    while (!GetAsyncKeyState(VK_INSERT)) {}
    POINT point = getCursorPos();

    Sleep(200);
    return point;
}


int milisecondsToDelay(bool noDelay)
{
    if (noDelay)
        return 2000; //delay needed for page to refresh

    srand(time(NULL));
    int minToDelay = rand() % 10; // how many minutes you want to delay 
    int secToDelay = rand() % 60; // how many seconds you want to delay 
    int milisecToDelay = secToDelay * 1000 + minToDelay * 60 * 1000;

    cout << "Total delay: " << minToDelay << "m " << secToDelay << "s "<< endl;
    return milisecToDelay;
}

bool clickOnPoke(ColorValues pokeColor, int x, int y)
{
    if (!(pokeColor == getPixelColor(x, y)))
        return false;

    mouseClick(x, y);
    pressKey(VK_F5);

    return true;
}

void invokePoking(int x, int y)
{
    int totalPokeCout = 0;
    while (!GetAsyncKeyState(VK_INSERT))
    {
        bool isPokeSuccessfull = /*clickOnPoke(getPixelColor(x,y), x, y);*// clickOnPoke(pokeColor, x, y) || clickOnPoke(pokeColorHover, x, y);
        if (isPokeSuccessfull)
            totalPokeCout++;

        system("CLS");
        cout << "Hold insert to stop the program." << endl;
        cout << "Total poke cout: " << totalPokeCout << endl;
        Sleep(milisecondsToDelay(true));
    }
}

void debug_showCurrPosAndColor()
{
    POINT point = getCurrCursorPos();

    cout << "(" << point.x << ", " << point.y << ")" << endl;
    getPixelColor(point.x, point.y).printColor();
    
    cout << endl << "Press any key to continue: " << endl;
    cin.get();
}

int main()
{
    POINT point = getCurrCursorPos();
    invokePoking(point.x, point.y);
}