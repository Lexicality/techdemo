#pragma once

struct InputData {
    bool Forwards;
    bool Backwards;
    bool Left;
    bool Right;
    /*
    bool Shoot;
    bool Use;
    bool InventoryPrev;
    bool InventoryNext;
    bool Slot1;
    bool Slot2;
    bool Slot3;
    bool Slot4;
    */
    bool Exit;
    float DeltaViewY;
    float DeltaViewX;

};
class InputManager {

public:
    const InputData GetInput();
private:
    int lastx, lasty;
    bool lostFocus;
};

