#include "BufferManager.h"


BufferManager::BufferManager(int BufferSize, int capacity) : bufferPool(BufferSize, capacity) {}

BufferManager::~BufferManager() {}

void BufferManager::requestPage(int pageID, char operation, std::vector<std::string> Data) {
    bool isPageMapped = pageTable.find(pageID) != pageTable.end() ? true : false;
    operation = std::toupper(operation);

    //bufferPool lleno
    if (pageTable.size() == bufferPool.getBufferSize() && !isPageMapped) {
        int aux = bufferPool.CLOCK();
        for (const auto& entry : pageTable) {
            if(entry.second == aux)
                aux = entry.first;
        }
        releasePage(aux);
    }

    if (operation == 'R') {
        if (!isPageMapped) {
            int frameID = bufferPool.findUnpinnedFrame();
            pageTable[pageID] = frameID;
            bufferPool.incrementFrame(frameID);
            
            
            for (const auto& entry : pageTable) {
                if (pageID != entry.first){
                    Frame* frame = bufferPool.getFrame(entry.second);
                    
                    frame->setReferenceBit();
                }
                else {
                    Frame* frame = bufferPool.getFrame(entry.second);
                    
                    frame->setReferenceBit();
                    bufferPool.updateIndex();
                    frame->setPinCount(operation);
                    bufferPool.fillFrame(Data, entry.second);
                }
            }
        } else {
            int frameID = pageTable[pageID];
            bufferPool.incrementFrame(frameID);
            
            for (const auto& entry : pageTable) {
                if (pageID != entry.first){
                    Frame* frame = bufferPool.getFrame(entry.second);
                    
                    frame->setReferenceBit();
                }
                else {
                    Frame* frame = bufferPool.getFrame(entry.second);
                    
                    frame->setReferenceBit();
                    bufferPool.updateIndex();
                    frame->setPinCount(operation);
                    bufferPool.fillFrame(Data, entry.second);
                }
            }
        }
    }
    else if (operation == 'W'){
        if (!isPageMapped) {
            int frameID = bufferPool.findUnpinnedFrame();
            pageTable[pageID] = frameID;
            bufferPool.incrementFrame(frameID);
            bufferPool.dirtyFrame(frameID);   
            for (const auto& entry : pageTable) {
                if (pageID != entry.first){
                    Frame* frame = bufferPool.getFrame(entry.second);
                    
                    frame->setReferenceBit();
                }
                else {
                    Frame* frame = bufferPool.getFrame(entry.second);
                    
                    frame->setReferenceBit();
                    bufferPool.updateIndex();
                    frame->setPinCount(operation);
                    bufferPool.fillFrame(Data, entry.second);
                }
            }
        } 
        else {
            // La página ya está mapeada, actualizar el dato en el buffer pool
            int frameID = pageTable[pageID];
            bufferPool.incrementFrame(frameID);
            bufferPool.dirtyFrame(frameID);
            for (const auto& entry : pageTable) {
                if (pageID != entry.first){
                    Frame* frame = bufferPool.getFrame(entry.second);
                    
                    frame->setReferenceBit();
                }
                else {
                    Frame* frame = bufferPool.getFrame(entry.second);
                    
                    frame->setReferenceBit();
                    bufferPool.updateIndex();
                    frame->setPinCount(operation);
                    bufferPool.fillFrame(Data, entry.second);
                }
            }
        }
    }
}


void BufferManager::releasePage(int pageID) {
    int frameID = pageTable[pageID];
    Frame* frame = bufferPool.getFrame(frameID);
    
    if (frame->getPinCount() == 0) {
        if (frame->getDirtyFlag() ) {
            savePagetoDisk(pageID);
        }

        frame->reset();
        bufferPool.resetFrame(frameID);
        pageTable.erase(pageID);
        bufferPool.updateIndex(); // Opcionalmente actualizar el índice para la próxima política CLOCK

        std::cout << "La page " << pageID << " ha sido liberada del frame " << frameID << "." << std::endl;
    }
    else{
        Frame* frame = bufferPool.getFrame(frameID);
        frame->decrementPinCount();
        frame->freeCountPin();
    }
}

void BufferManager::pinPage(int pageID) {
    auto it = pageTable.find(pageID);
    if (it != pageTable.end()) {
        int frameID = it->second;
        Frame* frame = bufferPool.getFrame(frameID);
        frame->pin();
    } else {
        std::cout << "La pagina no se encuentra" << std::endl;
    }
}