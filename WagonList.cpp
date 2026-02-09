#include "WagonList.h"

#include <iostream>

WagonList::~WagonList() { clear(); }

void WagonList::clear()
{
    if (front == nullptr){
        return;
    }

    Wagon *current = front;
    Wagon *nextWagon = nullptr;

    while (current != nullptr)
    {
        nextWagon = current->getNext();
        delete current;
        current = nextWagon;
    }

    front = nullptr;
    rear = nullptr;
    totalWeight = 0;

    // TODO: Delete all Wagon objects in this list and reset pointers.
}

WagonList::WagonList(WagonList &&other) noexcept
{
    this->front = other.front;
    this->rear = other.rear;
    this->totalWeight = other.totalWeight;

    other.front = nullptr;
    other.rear = nullptr;
    other.totalWeight = 0;

    // TODO: Implement move constructor.
    // Transfer ownership of 'other' list’s nodes into this list
    // and leave 'other' in an empty but valid state.
}

WagonList &WagonList::operator=(WagonList &&other) noexcept
{
    if (this == &other) {
        return *this;
    }
    
    clear(); 

    this->front = other.front;
    this->rear = other.rear;
    this->totalWeight = other.totalWeight;
    
    other.front = nullptr;
    other.rear = nullptr;
    other.totalWeight = 0;

    return *this;

    // Operation version of the move constructor.
    // TODO: Implement it.
}

Wagon *WagonList::findById(int id)
{
    Wagon *current = front;
    while (current != nullptr)
    {
        if (current->getID() == id)
        {
            return current;
        }
        current = current->getNext();
    }

    // TODO: Find and return the Wagon with given ID.
    // Return nullptr if not found.
    return nullptr;
}

void WagonList::addWagonToRear(Wagon *w)
{
    w->setNext(nullptr);
    w->setPrev(nullptr);
    totalWeight += w->getWeight();
    if (front == nullptr)
    {
        front = w;
        rear = w;
        return;
    }
    else{
        rear->setNext(w);
        w->setPrev(rear);
        rear = w;
    }
    // TODO: Add a Wagon to the rear (end) of the list.
    // This function does not respect the weight order
    // it inserts to end regardless of the weight
}

int WagonList::getTotalWeight() const { return totalWeight; }

bool WagonList::isEmpty() const
{
    // TODO: Return true if the list has no wagons.
    if (front == nullptr)
    {
        return true;
    }
    
    return false;
}

void WagonList::insertSorted(Wagon *wagon)
{
    wagon->setNext(nullptr);
    wagon->setPrev(nullptr);
    totalWeight = totalWeight + wagon->getWeight();

    if (front==nullptr)
    {
        front = wagon;
        rear = wagon;
        return;
    }

    int frontWeight = front->getWeight();
    int wagonWeight = wagon->getWeight();
    if (wagonWeight>frontWeight)
    {
        wagon->setNext(front);
        front->setPrev(wagon);
        front = wagon;
        return;
    }

    Wagon *second = front->getNext();

    while (second!=nullptr && second->getWeight() > wagonWeight)
    {
        second = second->getNext();
    }
    if (second==nullptr)
    {
        wagon->setPrev(rear);
        rear->setNext(wagon);
        rear = wagon;
    }
    else{
        wagon->setNext(second);
        wagon->setPrev(second->getPrev());
        second->getPrev()->setNext(wagon);
        second->setPrev(wagon);
    }
    return;
    
    // TODO: Insert wagon into this list in descending order of weight.
}

void WagonList::appendList(WagonList &&other)
{
    if (other.front == nullptr)
    {
        return;
    }
    if (this->front == nullptr)
    {
        this->front = other.front;
        this->rear = other.rear;

        this->totalWeight = other.totalWeight;
    
        other.front = nullptr;
        other.rear = nullptr;
        other.totalWeight = 0;
    return;
    }
    else{
        this->rear->setNext(other.front); 
        other.front->setPrev(this->rear); 
        this->rear = other.rear;
        this->totalWeight += other.totalWeight;
        other.front = nullptr;
        other.rear = nullptr;
        other.totalWeight = 0;
    }
    
   // TODO: Append another WagonList to this one (merge them).
   // Use move semantics to avoid deep copies. (Double && at the parameter already makes it so)
   // 'other' should end up empty after this operation
   // At merge lists (blocks) will be protected 
   // But the one with heavier wagon at the front will be before the other list
}

Wagon *WagonList::detachById(int id)
{
    if (front == nullptr) {
        return nullptr;
    }
    Wagon *current = front;
    while (current != nullptr && current->getID()!=id)
    {
        current = current->getNext();
    }

    if (current == nullptr)
    {
        return nullptr;
    }
    
    if (current == front)
    {
        totalWeight -= current->getWeight();
        std::cout << "Wagon " << current->getID() << " detached from Wagon List. " << std::endl;
        front = current->getNext();
        if (front != nullptr)
        {
            front->setPrev(nullptr);
        }
        else
        {
            rear = nullptr;
        }
    }
    else if (current == rear)
    {
        totalWeight -= current->getWeight();
        std::cout << "Wagon " << current->getID() << " detached from Wagon List. " << std::endl;
        rear = current->getPrev();
        rear->setNext(nullptr);
    }
    else
    {
        totalWeight -= current->getWeight();
        std::cout << "Wagon " << current->getID() << " detached from Wagon List. " << std::endl;
        current->getPrev()->setNext(current->getNext());
        current->getNext()->setPrev(current->getPrev());
    }
    current->setNext(nullptr);
    current->setPrev(nullptr);
    return current;
    // TODO: Remove a specific wagon (by ID) from this list and return it.
    // Use: std::cout << "Wagon " << toRemove->id << " detached from Wagon List. " << std::endl;
    // Return nullptr if wagon not found.
}


WagonList WagonList::splitAtById(int id)
{
    WagonList newList; // return-by-value (will be moved automatically)

    Wagon *current = this->front;
    while (current != nullptr && current->getID()!=id)
    {
        current = current->getNext();
    }
    if (current == nullptr)
    {
        return newList;
    }

    newList.front = current;
    newList.rear = this->rear;

    int totalWeightOfNewList = 0 ;
    Wagon *temp = current;
    while (temp != nullptr)
    {
        totalWeightOfNewList += temp->getWeight();
        temp = temp->getNext();
    }
    newList.totalWeight=totalWeightOfNewList;
    this->totalWeight = this->totalWeight - totalWeightOfNewList;

    Wagon *tempRear = current->getPrev();
    if (tempRear == nullptr)
    {
        this->front = nullptr;
        this->rear = nullptr;
    }
    else{
        this->rear = tempRear;
        this->rear->setNext(nullptr);
    }
    current->setPrev(nullptr);
    
    // TODO: Split this list into two lists at the wagon with given ID.
    // The wagon with 'id' becomes the start of the new list.
    // Return the new WagonList (move return).
    // If 'id' not found, return an empty list.

    return newList; // moved, not copied
}

// Print is already implemented
void WagonList::print() const
{

    std::cout << *this << std::endl;
    return;
}

// << operator is already implemented
std::ostream &operator<<(std::ostream &os, const WagonList &list)
{
    if (list.isEmpty())
        return os;

    Wagon *current = list.front;

    while (current)
    {
        os << "W" << current->getID() << "(" << current->getWeight() << "ton)";
        if (current->getNext())
            os << " - ";
        current = current->getNext();
    }
    return os;
}
