#include "frame.h"

Frame::Frame(int capacity) {
    this->capacity = capacity;
    this->isPinned = false;
    this->dirtyFlag = false;
    this->pinCount = 0;
    this->referenceBit = false;
}

Frame::~Frame() {}

void Frame::reset() {
    this->isPinned = false;
    this->dirtyFlag = false;
    this->pinCount = 0;
    this->referenceBit = false;
    this->data.clear();
}

void Frame::addRecord(std::string record) {
    this->data.push_back(record);
}

void Frame::pin() {
    this->isPinned = true;
}

void Frame::unpin() {
    this->isPinned = false;
}

void Frame::incrementPinCount() {
    this->pinCount++;
}       

void Frame::decrementPinCount() {
    if(this->pinCount > 0)
        this->pinCount--;
}

void Frame::setPinCount(char pinCount) {
    pinCount = pinCount - 'a' + 'A';
    this->countPin.push(pinCount);
}

bool Frame::getDirtyFlag() {
    return this->dirtyFlag;
}

int Frame::getPinCount() {
    return this->pinCount;
}

bool Frame::getIsPinned() {
    return this->isPinned;
}

bool Frame::getReferenceBit() {
    return this->referenceBit;
}

void Frame::freeCountPin() {
    if(!this->countPin.empty())
        this->countPin.pop();
}

//CLOCK
void Frame::setReferenceBit() {
    this->referenceBit = true;
}

void Frame::clearReferenceBit() {
    this->referenceBit = false;
}