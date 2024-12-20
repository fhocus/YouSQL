#include "BufferPool.h"

BufferPool::BufferPool(int size, int frameCapacity) {
    this->currIndex = 0;
    buffer.reserve(size);

    for(int i = 0; i < size; i++) {
        buffer.emplace_back(Frame(frameCapacity));
    }
}

BufferPool::~BufferPool() {}

int BufferPool::findUnpinnedFrame() {
    for(int i = 0; i < buffer.size(); i++) {
        if(buffer[i].getPinCount() && !buffer[i].getReferenceBit()) {
            return i;
        }
    }
    return -1;
}

void BufferPool::fillFrame(std::vector<std::string> data, int frameId) {
    Frame* frame = getFrame(frameId);

    if(frame) {
        for(int i = 0; i < data.size(); i++) {
            frame->addRecord(data[i]);
        }
    }
}

void BufferPool::pinFrame(int frameId) {
    Frame* frame = getFrame(frameId);
    if(frame) {
        frame->pin();
    }
}

void BufferPool::unpinFrame(int frameId) {
    Frame* frame = getFrame(frameId);
    if(frame) {
        frame->unpin();
    }
}

void BufferPool::incrementFrame(int frameId) {
    Frame* frame = getFrame(frameId);
    if(frame)
        frame->incrementPinCount();  
}

void BufferPool::decrementFrame(int frameId) {
    Frame* frame = getFrame(frameId);
    if(frame)
        frame->decrementPinCount();  
}

void BufferPool::resetFrame(int frameId) {
    Frame* frame = getFrame(frameId);
    if(frame)
        frame->reset();  
}

void BufferPool::dirtyFrame(int frameId) {
    Frame* frame = getFrame(frameId);
    if(frame)
        frame->dirty();  
}

void BufferPool::resetBuffer() {
    for(int i = 0; i < buffer.size(); i++) {
        buffer[i].reset();
    }
}

bool BufferPool::areAllFramesUsed() {
    for(int i = 0; i < buffer.size(); i++) {
        if(!buffer[i].getIsPinned()) {
            return false;
        }
    }
    return true;
}

void BufferPool::updateIndex() {
    currIndex = (currIndex + 1) % buffer.size();
}

int BufferPool::getcurrIndex() {
    return currIndex;
}

int BufferPool::getBufferSize() {
    return buffer.size();
}

Frame* BufferPool::getFrame(int frameId) {
    if(frameId >= 0 && frameId < buffer.size()) {
        return &buffer[frameId];
    }
    else {
        return nullptr;
    }
}

int BufferPool::CLOCK() {
    while(true) {
        Frame currFrame = buffer[currIndex];
        if(!currFrame.getReferenceBit() && currFrame.getPinCount() == 0) {
            return currIndex;
        } else {
            currFrame.clearReferenceBit();
        }
        updateIndex();
    }
}

