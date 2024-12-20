#pragma once
#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <vector>       
#include <queue>

class Frame {
public:
    Frame(int capacity);
    ~Frame();

    void reset();
    void addRecord(std::string record);
    void pin();
    void unpin();
    void incrementPinCount();
    void decrementPinCount();
    void dirty();

    void setPinCount(char pinCount);
    bool getDirtyFlag();
    int getPinCount();
    bool getIsPinned();
    bool getReferenceBit();
    void freeCountPin();

    //CLOCK
    void setReferenceBit();
    void clearReferenceBit();

private:
    int capacity;
    std::vector<std::string> data;
    bool isPinned;
    bool dirtyFlag;
    int pinCount;
    std::queue<int> countPin;
    bool referenceBit;  //CLOCK
};

#endif