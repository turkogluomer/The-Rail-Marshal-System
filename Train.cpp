#include "Train.h"
#include <iostream>

Train::Train() : name(""), destination(Destination::OTHERS), totalWeight(0), nextLocomotive(nullptr) {}
Train::Train(const std::string &_name, Destination _dest) : name(_name), destination(_dest), totalWeight(0), nextLocomotive(nullptr) {}
Train::~Train() { clear(); }

// This function is given to you ready
void Train::appendWagonList(WagonList &wl)
{
    // Makes appendList use move semantics
    wagons.appendList(std::move(wl));
    totalWeight = wagons.getTotalWeight();
}

// This function is given to you ready
void Train::addWagonToRear(Wagon *w)
{
    wagons.addWagonToRear(w);
    totalWeight = wagons.getTotalWeight();
}

void Train::clear()
{ 
    wagons.clear();
    totalWeight = 0;
    //TODO: Do the cleaning as necesssary
}

// This function is given to you ready
void Train::print() const
{
    std::cout << "Train " << name << " (" << totalWeight << " tons): ";
    std::cout << wagons << std::endl;
}

Train *Train::verifyCouplersAndSplit(int splitCounter)
{
    int currenWeight = 0;
    Wagon *overLoadedWagon = nullptr;
    Wagon *current = wagons.getRear();
    while (current != nullptr)
    {
        if (currenWeight > current->getMaxCouplerLoad())
        {
            overLoadedWagon = current;
            break;
        }
        currenWeight += current->getWeight();
        current = current->getPrev();
    }

    if (overLoadedWagon == nullptr)
    {
        return nullptr;
    }
    
    Wagon *frontOfNewTrain = overLoadedWagon->getNext();
    if (frontOfNewTrain == nullptr) {
        return nullptr; 
    }
    int frontId = frontOfNewTrain->getID();
    WagonList newWagonList = wagons.splitAtById(frontId);
    if (newWagonList.isEmpty()) {
        return nullptr; 
    }
    std::string newTrainName = this->name + "_split_" + std::to_string(splitCounter);
    Train *newTrain = new Train(newTrainName, this->destination);
    newTrain->appendWagonList(newWagonList);
    std::cout << "Train " << this->name << " split due to coupler overload before Wagon " << frontId << std::endl;
    std::cout << newTrain->getWagons() << std::endl;
    return newTrain;
    
    // TODO: Verify whether any wagon’s coupler is overloaded.
    // You must traverse from the rear (backmost wagon) toward the front.
    //
    // Splitting rule:
    // Split the train AFTER the overloaded wagon (the overloaded one stays).
    // Use WagonList::splitAtById to detach the overloaded segment into a new WagonList.
    //
    // If no overload occurs, return nullptr (no split needed).
    //
    // If a valid split occurs:
    // new Train should be named "<oldName>_split_<splitCounter>".

    // print message
    // std::cout << "Train " << name << " split due to coupler overload before Wagon " << splitId << std::endl;
    // std::cout << newTrain->wagons << std::endl;
}