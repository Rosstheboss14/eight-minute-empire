#include <iostream>
#include <string>
#include "Player.h"
#include "../Map/Map.h"
#include "../BiddingFacility/BiddingFacility.h"

//Constructors

Player::Player(std::string name, int age) {
    Player::cities = new int(3);
    Player::armies = new int(14);
    Player::coins = new int(14);
    Player::name = new std::string(name);
    Player::age = new int(age);
    Player::biddingFacility = new BiddingFacility();
}

Player::Player(std::string name, int numOfPlayers, int age) {
    Player::cities = new int(3);
    Player::armies = new int(14);
    Player::name = new std::string(name);
    Player::age = new int(age);
    Player::biddingFacility = new BiddingFacility();

    // Amount of coins available to Players is determined by # of Players
    switch (numOfPlayers) {
        case 2 :
            Player::coins = new int(14);
            break;
        case 3 :
            Player::coins = new int(11);
            break;
        case 4 :
            Player::coins = new int(9);
            break;
        case 5 :
            Player::coins = new int(8);
            break;
    }
}

// Deconstructor

Player::~Player() {
    delete this->coins;
    delete this->armies;
    delete this->cities;
    delete this->age;
    delete this->name;
    delete this->biddingFacility;
};

// Gameplay Methods

bool Player::payCoin(int cost) {

    if (*coins < cost) {
        std::cout << "You don't have enough coins to purchase that." << std::endl;
        return false;
    }
    else {
        setCoins(*coins - cost);
        std::cout << "Successful Purchase, you have " << *coins << " amount of coins remaining" << std::endl;
        return true;
    }
}

bool Player::placeNewArmies(int totalArmies, Map &gameBoard)
{
    // check to see if player has available armies to place
    if (*armies == 0) {
        std:: cout << "Sorry, " << *name << " you have no more available armies." << endl;
        return false;
    }

    int spendableArmies;
    std::string regionName;
    std::vector<std::string> placementRegions;

    // if player doesn't have enough armies to fulfill the value on card, place remaining armies
    if (totalArmies > *armies) {
        spendableArmies = *armies;
    }
    else {
        spendableArmies = totalArmies;
    }

    // inform player where they can place units (ie. starting area and where they have cities)
    if (spendableArmies == 1) {
        std::cout << *name << ", you have " << spendableArmies << " army to place on the map." << endl;
    }
    else {
        std::cout << *name << ", you have " << spendableArmies << " armies to place on the map." << endl;
    }
    std::cout << "Here are the areas in which you may place an army: " << endl;
    placementRegions = gameBoard.getRegionsToAddArmies(*name);
    for (int i = 0; i < placementRegions.size(); i++) {
        std::cout << "- " << placementRegions.at(i) << endl;
    }

    // place new armies
    for (int i = 0; i < spendableArmies; i++) {
        std::cout << "Where would you like to place your army? " << endl;
        regionName = validateRegion(placementRegions);
        (*armies)--;
        gameBoard.addArmy(regionName, *name);
        std::cout << "Successfully added an army to " << regionName << "." << endl << endl;
    }

    return true;
}

void Player::moveArmies(int totalMoves, Map &gameBoard, bool waterMove)
{
    for (int i = 0; i < totalMoves; i++) {
        std::string origin;
        std::string destination;
        std::vector<std::string> regionsWithArmies;
        std::vector<std::string> connections;

        // display regions where player currently has armies
        std::cout << *name << ", you have armies in the following locations: " << endl;
        regionsWithArmies = gameBoard.getRegionsWithArmies(*name);
        for (int i = 0; i < regionsWithArmies.size(); i++) {
            std::cout << "- " << regionsWithArmies.at(i) << endl;
        }

        // player selects a region to move an army from
        std::cout << "Please select a region in which to move an army from: ";
        origin = validateRegion(regionsWithArmies);

        // display connected regions
        std::cout << "The follow regions are connected to " << origin << " by land: " << endl;
        if (waterMove) {
            connections = gameBoard.getRegionsConnectedByLandAndWater(origin);
        }
        else {
            connections = gameBoard.getRegionsConnectedByLand(origin);
        }
        for (int i = 0; i < connections.size(); i++) {
            std::cout << "- " << connections.at(i) << endl;
        }

        // player selects a region to move an army to
        std::cout << "Please select a region in which to move an army to: ";
        destination = validateRegion(connections);

        // move army
        gameBoard.moveArmy(origin, destination, *name);
        std::cout << "Sucessfully moved 1 troop from " << origin << " to " << destination << "." << endl << endl;
    }
}

bool Player::buildCity(Map &gameBoard)
{
    // check to see if player has available cities to place
    if (*cities == 0) {
        std:: cout << "Sorry, " << *name << "you have no more available cities." << endl;
        return false;
    }

    std::string regionName;
    std::vector<std::string> placementRegions;

    // inform player where they can place a city
    std::cout << *name << ", here are the regions in which you may place a city: " << std::endl;
    placementRegions = gameBoard.getRegionsToAddCities(*name);
    for (int i = 0; i < placementRegions.size(); i++) {
        std::cout << "- " << placementRegions.at(i) << endl;
    }

    // place new city
    std::cout << "Where would you like to place your city? " << endl;
    regionName = validateRegion(placementRegions);
    (*cities)--;
    gameBoard.addCity(regionName, *name);
    std::cout << "Successfully added a city to " << regionName << "." << endl << endl;

    return true;
}

// TODO: Every turn should output the current state of the game
void Player::destroyArmy(Map &gameBoard, std::vector<Player*> allPlayers)
{
    std::string playerName;
    std::string regionName;
    std::vector<std::string> regionsWithEnemies;
    bool invalidPlayerName = false;

    // select another player whose army will be destroyed
    std::cout << *name << ", please select a player to destroy one of their armies: ";
    do {
        std::cin >> playerName;
        for (int i = 0; i < allPlayers.size(); i++) {
            if (playerName == *name || playerName != *allPlayers.at(i)->getName()) {
                invalidPlayerName = true;
            }
            else {
                invalidPlayerName = false;
                break;
            }
        }
        if (invalidPlayerName) {
            std::cout << "ERROR: Invalid player name provided. Please select another player: ";
        }
    } while (invalidPlayerName);

    // inform player where the armies from the targeted player are
    std::cout << *name << ", here are the regions in which you may place a city: " << std::endl;
    regionsWithEnemies = gameBoard.getRegionsWithArmies(playerName);
    for (int i = 0; i < regionsWithEnemies.size(); i++) {
        std::cout << "- " << regionsWithEnemies.at(i) << endl;
    }

    // select a region to destroy a unit
    std::cout << "Which region would you like to remove an army from: ";
    regionName = validateRegion(regionsWithEnemies);
    gameBoard.destroyArmy(regionName, playerName);
    std::cout << "Successfully removed one of " << playerName <<"'s armies from " << regionName << "." << endl << endl;
}

// Utility Methods

std::string Player::validateRegion(std::vector<std::string> placementRegions)
{
    bool invalidInput = true;
    std::string regionName;

    // accept and validate user input
    do {
        std::cin >> regionName;
        for (int i = 0; i < placementRegions.size(); i++) {
            if (placementRegions.at(i) == regionName) {
                invalidInput = false;
                break;
            }
        }
        if (invalidInput) {
            std::cout << "ERROR: Invalid region. Please enter a region from the list. " << endl;
        }
    } while (invalidInput);

    return regionName;
}

// Accessors

int* Player::getCoins() const {
    return coins;
}

int* Player::getArmies() const {
    return armies;
}

int* Player::getCities() const {
    return cities;
}

int* Player::getAge() const {
    return age;
}

std::string* Player::getName() const {
    return name;
}

BiddingFacility* Player::getBiddingFacility() const {
    return biddingFacility;
}

// Mutators

void Player::setCoins(int coins) {
    *Player::coins = coins;
}

void Player::setArmies(int armies) {
    *Player::armies = armies;
}

void Player::setCities(int cities) {
    *Player::cities = cities;
}

void Player::setAge(int age) {
    *Player::age = age;
}

void Player::setName(std::string name) {
    *Player::name = name;
}
