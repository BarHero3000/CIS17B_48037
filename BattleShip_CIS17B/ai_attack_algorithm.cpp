//Author Brandon Robinson 2016
#include "ai_attack_algorithm.h"

#include <QString>		//std::string
//#include <iostream>		//std::qDebug()
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <string>       //std::to_string()
#include <windows.h>    //Needed for milliseconds
#include <QDebug>       //Qdebug() comparable to qDebug()
#include <QChar>
using namespace std;


//Constructor, creates a new AI Attack Algorithm instance
AI_Attack_Algorithm::AI_Attack_Algorithm() {
    //Set debugging to true for debugging output
    debugging = false;
    //Set key to zero, random attacking
    key = 0;
    //Initialize MOVE vector with possMovesLeft, 10 x 10 grid = 100 possible moves
    possMoves.resize(100);
    //Fill possMoves with all possible moves in order
    fillMoves();
    //Randomize move possible move list
    randomizeMoves(); 
    //Delay else there isnt enough time between the random seeds for it it to come up with the a different list
    //Sleep(1000);	//Future fix is to do the random seed with milliseconds and not seconds
}
//Destructor
AI_Attack_Algorithm::~AI_Attack_Algorithm()
{
}
//Create the move list in order
void AI_Attack_Algorithm::fillMoves(){
    for (int i = 0; i < 10; i++){
        for (int x = 0; x < 10; x++){
            //Add move to vector (Row letter + Column number)
            possMoves[(i * 10) + x] = num2Letter(i) + intNum2StrNum(x + 1);    //Must convert integer to a QString
        }
    }
    if (debugging){
        //Print possible moves in grid view 10x10 (not randomized)
        QString temp = "";
        for (int i = 0; i < signed(possMoves.size()); i++){
            temp += possMoves[i] + " ";
            if (i % 10 == 9){
                qDebug() << temp;
                temp = "";
            }
        }
    }
}
//Randomize the Possible Move List
void AI_Attack_Algorithm::randomizeMoves(){
    //Create temporary move vector and copy vectors
    vector<QString> tempMoves(possMoves);
    //Array of numbers the size of the total deck to be randomized.
    int randArray[100];	//Visual Studio does not like [possMoves.size()]
    //int randArray[possMoves.size()];
    //Fill randArray from 0 to totalPossiblemoves - 1
    for (int i = 0; i < signed(possMoves.size()); i++){
        randArray[i] = i;
    }
    //Randomize the randArray
    SYSTEMTIME st;
    GetSystemTime(&st);
    //Millisecond percision is needed because the computer runs other function with random seed so fast
    std::srand(unsigned(st.wMilliseconds));	//So that cards are actually random for every game.
    //std::srand(unsigned(std::time(NULL)));   //So that cards are actually random for every game.
    std::random_shuffle(&randArray[0], &randArray[possMoves.size() - 1]);
    //Randomize Possible Moves Vector using the randomized array
    for (int i = 0; i < signed(possMoves.size()); i++){
        possMoves[i] = tempMoves[randArray[i]];
    }

    //Switch last coordinate in Possible Move List with another coordinate based on randomNum that was generated.
    //This is needed because the random_shuffle function does not change the last instance of array
    //Initialize the random seed
    std::srand(unsigned(std::time(NULL)));
    //Generate a random number between 0 and 99
    int randomNum = rand() % 99 + 0;
    //Do the swap!
    QString temp = possMoves[possMoves.size() - 1];
    possMoves.at(possMoves.size() - 1) = possMoves.at(randomNum);
    possMoves.at(randomNum) = temp;


    if (debugging){
        //Print possible moves in grid view (randomized)
        qDebug() << endl;
        for (int i = 0; i < signed(possMoves.size()); i++){
            qDebug() << possMoves[i] << " ";
            if (i % 10 == 9)
                qDebug() << endl;
        }
    }

}
//Pop of Move from the back of the Possible Move List (Random Move))
QString AI_Attack_Algorithm::randomFire(){
    QString temp;
    //Take move from the back of the vector
    temp = possMoves.at(possMoves.size() - 1);
    possMoves.pop_back();
    lastMove = temp;
    if (debugging){
        qDebug() << "AI fires at: " << temp;
    }
    return temp;
}
//Fire function for specific cell, not random, remove Move from Possible Move List
QString AI_Attack_Algorithm::fire(QString position){
    QString temp;
    //Find the index of the move
    int pos = find(possMoves.begin(), possMoves.end(), position) - possMoves.begin();
    //If Move has coordinate has already been shot.....
    if (pos == signed(possMoves.size())){
        return "Unavailable";
    }
    //Take move from the back of the vector
    temp = possMoves.at(pos);
    possMoves.erase(possMoves.begin() + pos);
    lastMove = temp;

    if (debugging){
        qDebug() << "AI fires at: " << temp;
    }

    return temp;
}
//Confirmation of a ship sinking has been received, remove coordinates from Possible Ship List
void AI_Attack_Algorithm::sunkShip(int shipSize){
    //Code has not been checked to work yet.
    bool directOK = true;
    //Get current direction and reverse it.
    QChar checkDirection = switchDirection(direction);
    //Check if last n amount of moves were in the same direction
    //int size = signed(possShip.size());    //For debugging
    QString value1 = "", value2 = "";
    int n = 1;
    int bound = possShip.size() - shipSize;
    int startPos = (possShip.size() - 2);
    for (int i = startPos; i >= bound; i--){
        value1 = possShip.at(i);
        value2 = nextPosition(checkDirection, possShip.at(possShip.size() - n));
        n++;
        if (debugging){ qDebug() << "Comparing: " << value1 << " & " << value2 << endl; }
        if (value1 != value2)
            directOK = false;
    }

    if (directOK){   //Last n amount of hit moves were in the same direction, remove those.
        if (debugging){
            qDebug() << "Last " << shipSize << " hits were in a row";
            qDebug() << "Removed: ";
        }
        for (int i = 0; i < shipSize; i++){
            if (debugging){ qDebug() << possShip.at(possShip.size() - 1) << " "; }
            possShip.pop_back();
        }
    }
    else{ //Remove first hit move and last shipSize - 1 hit moves
        if (debugging){
            QString temp, temp1;
            qDebug() << "Last " << shipSize << " hits were NOT in a row";
            temp += "Removed: " + possShip.at(0) + " ";
            for (int i = 0; i < shipSize - 1; i++){
                temp += possShip.at(possShip.size() - 1 - i) + " ";
            }
            qDebug() << temp;
        }
        possShip.erase(possShip.begin());
        for (int i = 0; i < shipSize - 1; i++){
            possShip.pop_back();
        }
    }

    if (possShip.size() == 0){   //All possible ship hits have been removed from the list
        key = 0;    //Start random firing again.
        if (debugging){ qDebug() << "key: " + key; }
    }
    else{
        randomDirect(); //Randomize Direction List
        lastMove = possShip.at(0);  //Set lastMove to the first coordinate left in the Possible Ship List
        key = 1;    //key = 1 => randomly fire around the coordinate
        if (debugging){ qDebug() << "key: " + key; }
    }
}
//Create a new random Direct list
void AI_Attack_Algorithm::randomDirect(){
    //Randomly choose UP(U), DOWN(D), LEFT(L), RIGHT(R)
    QChar randArray[4];
    randArray[0] = 'U';
    randArray[1] = 'D';
    randArray[2] = 'L';
    randArray[3] = 'R';
    //Randomize the randArray
    SYSTEMTIME st;  //Windows machines only. Alternative is Time function but not as random with fast computers
    GetSystemTime(&st);
    std::srand(unsigned(st.wMilliseconds)); //So that cards are actually random for every game.
    //std::iterator g(srand);
    //std::srand ( unsigned ( std::time(NULL)));   //So that cards are actually random for every game.
    std::random_shuffle(&randArray[0], &randArray[3]);
    //Initialize Possible Direction vector with 4 possible directions
    possDirect.resize(4);
    //Copy randArray into possible Direction vector
    for (int i = 0; i < 4; i++){
        possDirect[i] = randArray[i];
    }
    //Switch last direction in Possible Direction List with another direction based on randomNum that was generated.
    //This is needed because the random_shuffle function does not change the last instance of array
    //Initialize the random seed
    std::srand(unsigned(std::time(NULL)));
    //Generate a random number between 0 and 3
    int randomNum = rand() % 3 + 0;
    //Do the swap!
    QChar temp = possDirect[possDirect.size() - 1];
    possDirect.at(possDirect.size() - 1) = possDirect.at(randomNum);
    possDirect.at(randomNum) = temp;

    if (debugging){
        //For debugging: Print possDirection
        for (int i = 0; i < 4; i++){
            qDebug() << possDirect[i] << " ";
        }
    }
}
//Returns the next coordinate based on the last Hit and set direction
QString AI_Attack_Algorithm::nextPosition(QChar direction, QString lastHit){
    QString nextPos;
    int temp = 0;   //Only needed for debugging. After code works can combine two lines into one in the switch statement.
    //Break last hit apart into a row and column QStrings
    //QString row = lastHit.substr(0, 1); //Letter
    //QString column = lastHit.substr(1, lastHit.size());  //Number
    QString row = lastHit.mid(0, 1); //Letter
    QString column = lastHit.mid(1, lastHit.size()); //Number

    char d;
    d = direction.toLatin1();
    switch (d){
    case 'U':{
        //Move up one, manipulating the letter... Ex: C5 => B5
        if (strLetter2Num(row) == 0) //Row A (Top of grid))
            return "Out";
        else{
            temp = strLetter2Num(row) - 1;
            row = num2StrLetter(temp);
        }
        break;
    }
    case 'D':{
        //Move down one, manipulating the letter... Ex: C5 => D5
        if (strLetter2Num(row) == 9) //Row J (Bottom of grid))
            return "Out";
        else{
            temp = strLetter2Num(row) + 1;
            row = num2StrLetter(temp);
        }
        break;
    }
    case 'L':{
        //Move left one, manipulating the number... Ex: C5 => C4
        if (strNum2Num(column) == 1) //Column 1 (Left of grid)
            return "Out";
        else{
            temp = strNum2Num(column) - 1;
            column = intNum2StrNum(temp);
        }
        break;
    }
    case 'R':{
        //Move right one, manipulating the number... Ex: C5 => C6
        if (strNum2Num(column) == 10) //Column 10 (Right of grid)
            return "Out";
        else{
            temp = strNum2Num(column) + 1;
            column = intNum2StrNum(temp);
        }
        break;
    }
    }

    //nextPos = row + column;   //Does not work. Adds the decimal Unicode instead of the combining the Characters
    nextPos += row;
    nextPos += column;

    return nextPos;
}
//Switch direction of Possible Direction
QChar AI_Attack_Algorithm::switchDirection(QChar direction){
    char d;
    d = direction.toLatin1();
    switch (d){
    case 'U': return 'D';
    case 'D': return 'U';
    case 'L': return 'R';
    case 'R': return 'L';
    }

    return 'W'; //Need so that the compiler does'nt throw a fit...
}
//AI's Attacking Algorithm
QString AI_Attack_Algorithm::moveAI(){
    QString holdFire = "";
    switch (key){
    case 0:{    //No ships coordinates found yet
        //Fire off random coordinates. popping off from the possible move vector
        randomFire();
        break;
    }
    case 1:{    //New Ship found, try a direction
        //Set direction to popped off direction from possible direction vector
        direction = possDirect.back();
        if (debugging){ qDebug() << "Direction: " << direction << endl; }
        //Remove direction from possible direction list
        possDirect.pop_back();
        if (debugging){
            qDebug() << "Possible Directions Left in List: ";
            for (int i = 0; i < signed(possDirect.size()); i++){
                qDebug() << possDirect.at(i) << " ";
            }
        }
        //Fire at one space over in set direction
        holdFire = nextPosition(direction, possShip[0]);
        if (holdFire == "Out"){
            moveAI();
        }
        else{
            if (fire(holdFire) == "Unavailable"){
                if (debugging){ qDebug() << holdFire << " is Unavailable" << endl; }
                moveAI();
            }
        }
        //QString check = fire(nextPosition(direction, lastMove));
        //}
        //fire(nextPosition(direction, lastMove));
        //key = 2; qDebug() << "key: " << key << endl;
        break;
    }
    case 2:{    //Continue attacking in set Direction
        //Fire at one space over in set direction
        holdFire = nextPosition(direction, lastMove);
        if (holdFire == "Out"){
            key = 1;
            if (debugging){ qDebug() << "key: " << key << endl; }
            moveAI();
        }
        else{
            fire(holdFire);
        }
        break;
    }
    case 3:{    //Direction has been set, but last move missed. Go in opposite direction starting at first hit
        direction = switchDirection(direction);
        if (debugging){ qDebug() << "Direction: " << direction << endl; }
        //Remove direction from possible direction list
        //Find the index of the move
        int pos = find(possDirect.begin(), possDirect.end(), direction) - possDirect.begin();
        //Removed direction from possible direction list
        possDirect.erase(possDirect.begin() + pos);
        if (debugging){
            qDebug() << "Possible Directions Left in List: ";
            for (int i = 0; i < signed(possDirect.size()); i++){
                qDebug() << possDirect.at(i) << " ";
            }
        }
        fire(nextPosition(direction, possShip.at(0)));
        key = 2;
        if (debugging){ qDebug() << "key: " << key << endl; }
        break;
    }
    case 4:{    //Direction has been set, but last move missed. Go in opposite direction starting at first hit
        direction = switchDirection(direction);
        if (debugging){ qDebug() << "Direction: " << direction;}
        //Remove direction from possible direction list
        //Find the index of the move
        int pos = find(possDirect.begin(), possDirect.end(), direction) - possDirect.begin();
        //Removed direction from possible direction list
        possDirect.erase(possDirect.begin() + pos);
        if (debugging){
            qDebug() << "Possible Directions Left in List: ";
            for (int i = 0; i < signed(possDirect.size()); i++){
                qDebug() << possDirect.at(i) << " ";
            }
        }
        fire(nextPosition(direction, possShip.at(0)));
        key = 2;
        if (debugging){ qDebug() << "key: " << key; }
        break;
    }
    }
    return lastMove;
}
//Take the result of Last Move and set necessary flags
void AI_Attack_Algorithm::moveResult(bool hit, bool sunk, int shipSize){
    if (hit){    //Ship was hit
        //Add coordinate to possible ship list
        possShip.push_back(lastMove);
        hits.push_back(lastMove);
        if (debugging){ qDebug() << lastMove << " added to Possible Ship List. List Size = " << possShip.size() << endl; }
        if (possShip.size() == 1){   //No active ships found
            //Create random direction list
            randomDirect();
            //New Ship has been found flag
            newShipFound = true;
            //Go to next step in AI flow chart
            key = 1;
            if (debugging){ qDebug() << "key: " << key; }
        }
        else if (sunk){
            sunkShip(shipSize);

        }
        else if (key == 1){
            key = 2;
            if (debugging){ qDebug() << "key: " << key; }
        }


    }
    //else if (key == 3){    //Direction was set, but last move missed, switch directions
    else if (key == 2){    //Direction was set, but last move missed, switch directions
        //key = 4;
        key = 3;
        if (debugging){ qDebug() << "key: " << key; }
        if (debugging){ qDebug() << lastMove << " is a MISS!"; }
    }
    else if (key == 1){
        key = 1;
        if (debugging){ qDebug() << "key: " << key; }
        if (debugging){ qDebug() << lastMove << " is a MISS!"; }
    }
    else{
        key = 0;
        if (debugging){ qDebug() << "key: " << key; }
        if (debugging){ qDebug() << lastMove << " is a MISS!"; }
    }
}


//Converting Functions

//Convert a QString letter to an integer number (A-J => 0-9)
int AI_Attack_Algorithm::strLetter2Num(QString letter){
    QChar cLetter = letter[0];
    char d;
    d = cLetter.toLatin1();
    switch (d){
    case 'A': return 0;
    case 'B': return 1;
    case 'C': return 2;
    case 'D': return 3;
    case 'E': return 4;
    case 'F': return 5;
    case 'G': return 6;
    case 'H': return 7;
    case 'I': return 8;
    case 'J': return 9;
    }

    return 99;  //Need so that the compiler does'nt throw a fit...
}
//Convert a QString number to an integer number (0-10)
int AI_Attack_Algorithm::strNum2Num(QString num){
    QChar cNum;
    if (num == "10")
        return 10;
    else
        cNum = num[0];

    char d;
    d = cNum.toLatin1();
    switch (d){
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    }

    return 99;  //Need so that the compiler does'nt throw a fit...
}
//Convert an integer number to a QString letter (0-9 => A-J)
QString AI_Attack_Algorithm::num2StrLetter(int x){
    //0-9 == A-J
    switch (x){
    case 0: return "A";
    case 1: return "B";
    case 2: return "C";
    case 3: return "D";
    case 4: return "E";
    case 5: return "F";
    case 6: return "G";
    case 7: return "H";
    case 8: return "I";
    case 9: return "J";
    }

    return "Wrong index given!";    //Need so that the compiler does'nt throw a fit...
}
//Convert integer number to QString number (0-10)
QString AI_Attack_Algorithm::intNum2StrNum(int num){
    switch (num){
    case 0: return "0";
    case 1: return "1";
    case 2: return "2";
    case 3: return "3";
    case 4: return "4";
    case 5: return "5";
    case 6: return "6";
    case 7: return "7";
    case 8: return "8";
    case 9: return "9";
    case 10: return "10";
    }

    return "Wrong index given!";    //Need so that the compiler does'nt throw a fit...
}
//Convert integer number to QChar letter (0-9 => A-J)
QChar AI_Attack_Algorithm::num2Letter(int x){
    //0-9 == A-J
    switch (x){
    case 0: return 'A';
    case 1: return 'B';
    case 2: return 'C';
    case 3: return 'D';
    case 4: return 'E';
    case 5: return 'F';
    case 6: return 'G';
    case 7: return 'H';
    case 8: return 'I';
    case 9: return 'J';
    }

    return 'W'; //Need so that the compiler does'nt throw a fit...
}
