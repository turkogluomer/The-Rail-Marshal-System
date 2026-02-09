#include "ClassificationYard.h"
#include <iostream>

ClassificationYard::ClassificationYard() {}
ClassificationYard::~ClassificationYard() { clear(); }

WagonList &ClassificationYard::getBlockTrain(int destination, int cargoType)
{
    return blockTrains[destination][cargoType];
}

WagonList *ClassificationYard::getBlocksFor(Destination dest)
{
    return blockTrains[static_cast<int>(dest)];
}

// Inserts vagon to the corract place at the yard
void ClassificationYard::insertWagon(Wagon *w)
{
    if (!w)
        return;
    int dest = static_cast<int>(w->getDestination());
    int cargo = static_cast<int>(w->getCargoType());
    blockTrains[dest][cargo].insertSorted(w);
}

// Merges multiple blocks into a train while keeping blocks grouped
struct WagonListInfo {
    WagonList *list;
    int heaviestWagonWeight;
    int cargoTypeIndex;
};
Train *ClassificationYard::assembleTrain(Destination dest, const std::string &trainName)
{
    WagonListInfo WagonListToMerge[NUM_CARGOTYPES_INT];
    int blockCount = 0;
    WagonList *hazardousWagons = nullptr;
    int destIndex = static_cast<int>(dest);

    for (int i = 0; i < NUM_CARGOTYPES_INT; ++i)
    {
        WagonList &currentList = blockTrains[destIndex][i];

        if (currentList.isEmpty()) {
            continue;
        }
        
        CargoType currentCargo = currentList.getFront()->getCargoType();

        if (currentCargo == CargoType::HAZARDOUS)
        {
            if (hazardousWagons == nullptr)
            {
                hazardousWagons = &currentList;
            }
        }
        else
        {
            WagonListToMerge[blockCount].list = &currentList;
            WagonListToMerge[blockCount].heaviestWagonWeight = currentList.getFront()->getWeight();
            WagonListToMerge[blockCount].cargoTypeIndex = i;
            blockCount++;
        }
    }

    if (blockCount == 0 && hazardousWagons == nullptr)
    {
        //std::cout << "No wagons to assemble for " << destinationToString(dest) << std::endl;
        return nullptr;
    }

    for (int i = 1; i < blockCount; ++i)
    {
        WagonListInfo compareElement = WagonListToMerge[i];
        int j = i - 1;
        
        while (j >= 0 && WagonListToMerge[j].heaviestWagonWeight < compareElement.heaviestWagonWeight)
        {
            WagonListToMerge[j + 1] = WagonListToMerge[j];
            j = j - 1;
        }
        WagonListToMerge[j + 1] = compareElement;
    }

    Train *newTrain = new Train(trainName, dest);
    for (int i = 0; i < blockCount; ++i)
    {
        newTrain->appendWagonList(*WagonListToMerge[i].list);
    }
    
    if (hazardousWagons != nullptr)
    {
        newTrain->appendWagonList(*hazardousWagons);
    }

    std::cout << "Train " << newTrain->getName() << " assembled with " << newTrain->getWagons() << " wagons." << std::endl;

    return newTrain;
    // TODO: Collect wagons of the same destination and assemble them into a single Train.

    /**
     * - Blocks of the same cargo type must remain grouped together.
     * - These groups must be appended to the train in descending order
     *   based on their heaviest wagon.
     * - Hazardous cargo (e.g., OIL) must always be placed at the very end of the train,
     *   and only one hazardous block can be included per train.*/
}

bool ClassificationYard::isEmpty() const
{
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i)
    {
        for (int j = 0; j < NUM_CARGOTYPES_INT; ++j)
        {
            if (!blockTrains[i][j].isEmpty())
            {
                return false; 
            }
        }
    }
    /** TODO: Check if the entire classification yard is empty.
     *
     * The yard is empty if every blockTrain list for all destination-cargo pairs is empty.
     */
    return true;
}

void ClassificationYard::clear()
{
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i){
        for (int j = 0; j < NUM_CARGOTYPES_INT; ++j)
        {
            blockTrains[i][j].clear();
        }
    }
    /** TODO: Clear all wagons from the classification yard.
     *
     * Used when resetting or ending the simulation.
     */
}

// Print function is already implemented to keep output uniform
void ClassificationYard::print() const
{
    for (int i = 0; i < static_cast<int>(Destination::NUM_DESTINATIONS); ++i)
    {
        auto dest = destinationToString(static_cast<Destination>(i));
        std::cout << "Destination " << dest << ":\n";
        for (int j = 0; j < static_cast<int>(CargoType::NUM_CARGOTYPES); ++j)
        {
            if (!blockTrains[i][j].isEmpty())
            {
                auto type = cargoTypeToString(static_cast<CargoType>(j));
                std::cout << "  CargoType " << type << ": ";
                blockTrains[i][j].print();
            }
        }
    }
}