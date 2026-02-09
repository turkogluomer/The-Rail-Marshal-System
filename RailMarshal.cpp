#include "RailMarshal.h"
#include <iostream>
#include <sstream>
#include <algorithm>

RailMarshal::RailMarshal()
{
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i)
    {
        departureYard[i] = TrainTrack(static_cast<Destination>(i));
    }
    // TODO: Initialize each track in the departure yard.
    // Each TrainTrack corresponds to one Destination.
}

RailMarshal::~RailMarshal()
{
    classificationYard.clear();
    // TODO: Cleanup remaining trains, prevent memory leaks
}

// Getter (ready)
ClassificationYard &RailMarshal::getClassificationYard()
{
    return classificationYard;
}

// Getter (ready)
TrainTrack &RailMarshal::getDepartureYard(Destination dest)
{
    int idx = static_cast<int>(dest);
    return departureYard[idx];
}

void RailMarshal::processCommand(const std::string &line)
{
    // TODO: Parse user commands from input lines.
    std::stringstream ss(line);
    std::string command;
    ss >> command;
    if (command.empty()) {
        return;
    }
    std::string s1, s2;
    int id, weight, maxCouplerLoad;
    std::string destinationString, cargoTypeString;

    // if ADD_WAGON
    // Use: std::cout << "Error: Invalid ADD_WAGON parameters.\n";
    // Use: std::cout << "Wagon " << *w << " added to yard." << std::endl;
    if (command == "ADD_WAGON")
    {
        if (!(ss >> id >> cargoTypeString >> destinationString >> weight >> maxCouplerLoad)) {
            std::cout << "Error: Invalid ADD_WAGON parameters.\n";
            return;
        }

        CargoType cargoType = parseCargo(cargoTypeString);
        Destination destination = parseDestination(destinationString);
        Wagon *newWagon = new Wagon(id, cargoType, destination, weight, maxCouplerLoad);
        classificationYard.insertWagon(newWagon);
        std::cout << "Wagon " << *newWagon << " added to yard." << std::endl;
    }

    // if REMOVE_WAGON
    // Use: std::cout << "Error: Invalid REMOVE_WAGON parameters.\n";
    // Use: std::cout << "Wagon " << id << " removed." << std::endl;
    // Use: std::cout << "Error: Wagon " << id << " not found." << std::endl;
    else if (command == "REMOVE_WAGON")
    {
        if (!(ss >> id)) {
             std::cout << "Error: Invalid REMOVE_WAGON parameters.\n";
             return;
        }
        
        Wagon *removedWagon = nullptr;
        for (int i = 0; i < NUM_DESTINATIONS_INT; ++i) {
            for (int j = 0; j < NUM_CARGOTYPES_INT; ++j) {
                removedWagon = classificationYard.getBlockTrain(i, j).detachById(id);
                if (removedWagon != nullptr) {
                    break; 
                }
            }
            if (removedWagon != nullptr) {
                break;
            }
        }
        
        if (removedWagon == nullptr) {
            std::cout << "Error: Wagon " << id << " not found." << std::endl;
        }else {
            delete removedWagon;
            std::cout << "Wagon " << id << " removed." << std::endl; 
        }
    }

    // if ASSEMBLE_TRAIN
    //  Use: std::cout << "Error: Invalid ASSEMBLE_TRAIN parameters.\n";
    //  Use: std::cout << "No wagons to assemble for " << destStr << std::endl;
    //  verify couplers and possibly split (deterministic)
    //  Keep splitting the *front* train until no more overloaded couplers found
    //  create new train with same destination and name suffix
    //  use std::cout << "Train " << newTrain->getName() << " assembled after split with "
    //  << newTrain->getWagons()<< " wagons." << std::endl;
    // use std::cout << "Train " << t->getName() << " assembled with " << t->getWagons() << " wagons." << std::endl;
    else if (command == "ASSEMBLE_TRAIN")
    {
        if (!(ss >> destinationString)) {
            std::cout << "Error: Invalid ASSEMBLE_TRAIN parameters.\n";
            return;
        }

        Destination destination1 = parseDestination(destinationString);
        int destIndex = static_cast<int>(destination1);
        TrainTrack &track = departureYard[destIndex];
        std::string trainName = track.generateTrainName();
        Train *newTrain = classificationYard.assembleTrain(destination1, trainName);

        if (newTrain == nullptr) {
            std::cout << "No wagons to assemble for " << destinationString << std::endl;
            return;
        }

        Train *splitTrain = nullptr;
        int splitCounter = 1;
        while ( (splitTrain = newTrain->verifyCouplersAndSplit(splitCounter)) != nullptr )
        {
            departureYard[destIndex].addTrain(splitTrain);
            std::cout << "Train " << splitTrain->getName() << " assembled after split with "<< splitTrain->getWagons()<< " wagons." << std::endl;
            splitCounter++;
        }  
        departureYard[destIndex].addTrain(newTrain);
        std::cout << "Train " << newTrain->getName() << " assembled with " << newTrain->getWagons() << " wagons." << std::endl;
    }

    // if DISPATCH_TRAIN
    //  use: std::cout << "Error: Invalid DISPATCH parameters.\n";
    //  use: std::cout << "Error: No trains to dispatch from track " << destStr << ".\n";
    //  use:  std::cout << "Dispatching " << train->getName() << " (" << t->getTotalWeight() << " tons)." << std::endl;
    else if (command == "DISPATCH_TRAIN")
    {
        if (!(ss >> destinationString)) {
            std::cout << "Error: Invalid DISPATCH parameters.\n";
            return;
        }

        Destination dest = parseDestination(destinationString);
        TrainTrack &track = departureYard[static_cast<int>(dest)];
        Train *departed = track.departTrain();
        if (departed == nullptr) {
            std::cout << "Error: No trains to dispatch from track " << destinationToString(dest) << ".\n";
        } else {
            std::cout << "Dispatching " << departed->getName() << " (" << departed->getTotalWeight() << " tons)." << std::endl;
            delete departed;
        }
    }

    // if PRINT_YARD
    //  use std::cout << "--- classification Yard ---\n";
    else if (command == "PRINT_YARD")
    {
        std::cout << "--- classification Yard ---\n";
        classificationYard.print();
    }

    // if PRINT_TRACK
    //  use std::cout << "Error: Invalid PRINT_TRACK parameters.\n";
    else if (command == "PRINT_TRACK")
    {
        if (!(ss >> destinationString)) {
            std::cout << "Error: Invalid PRINT_TRACK parameters.\n";
            return;
        }
        Destination dest = parseDestination(destinationString);
        departureYard[static_cast<int>(dest)].printTrack();
    }

    // if AUTO_DISPATCH <ON/OFF>
    // Enable or disable automatic dispatch when weight exceeds limits.
    // std::cout << "Error: Invalid AUTO_DISPATCH parameters.\n";
    // print "Auto dispatch "enabled" / "disabled"
    else if (command == "AUTO_DISPATCH")
    {
        if (!(ss >> s1)) {
            std::cout << "Error: Invalid AUTO_DISPATCH parameters.\n";
            return;
        }
        
        std::string mode = toUpper(s1);
        if (mode == "ON") {
            TrainTrack::autoDispatch = true;
            std::cout << "Auto dispatch enabled" << std::endl;
        } else if (mode == "OFF") {
            TrainTrack::autoDispatch = false;
            std::cout << "Auto dispatch disabled" << std::endl;
        } else {
             std::cout << "Error: Invalid AUTO_DISPATCH parameters.\n";
        }
    }

    // if CLEAR
    // Completely reset the system (yard + departure tracks).
    // std::cout << "System cleared." << std::endl;
    else if (command == "CLEAR")
    {
        classificationYard.clear();
        for (int i = 0; i < NUM_DESTINATIONS_INT; ++i)
        {
            departureYard[i] = TrainTrack(static_cast<Destination>(i));
        }
        std::cout << "System cleared." << std::endl;
    }

    // else std::cout << "Error: Unknown command '" << cmd << "'" << std::endl;
    else
    {
        std::cout << "Error: Unknown command '" << command << "'" << std::endl;
    }
}

void RailMarshal::dispatchFromTrack(Destination track)
{
    TrainTrack &trackRef = departureYard[static_cast<int>(track)];
    std::string destStr = destinationToString(track);
    Train *departed = trackRef.departTrain();
    if (departed == nullptr) 
    {
        std::cout << "Error: No trains to dispatch from track " << destStr << ".\n";
    } 
    else 
    {
        int destIndex = static_cast<int>(departed->getDestination());
        std::cout << "Train " << departed->getName()<< " departed from Track " << destIndex<< " (" << destinationToString(static_cast<Destination>(destIndex)) << ").\n";
        std::cout << "Dispatching " << departed->getName() << " (" << departed->getTotalWeight() << " tons)." << std::endl;
        delete departed;
    }
    // TODO: Dispatch the next train (frontmost) from the specified track.
    // std::cout << "Error: No trains to dispatch from Track " << destIndex << ".\n";
    /*std::cout << "Train " << t->getName()
              << " departed from Track " << destIndex
              << " (" << destinationToString(static_cast<Destination>(destIndex)) << ").\n";
     */
}

void RailMarshal::printDepartureYard() const
{
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i)
    {
        std::cout << "Track " << i << " ("
                  << destinationToString(static_cast<Destination>(i)) << "):\n";
        departureYard[i].printTrack();
    }
}

// Debug helper functions
void RailMarshal::printStatus() const
{
    std::cout << "--- classification Yard ---\n";
    classificationYard.print();

    std::cout << "--- Departure Yard ---\n";
    for (int i = 0; i < static_cast<int>(Destination::NUM_DESTINATIONS); ++i)
    {
        departureYard[i].printTrack();
    }
}
