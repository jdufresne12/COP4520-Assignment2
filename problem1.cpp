/*
    John Dufresne
    COP 4520
    Problem 1: Minotaur's Birthday Party
        A minotaur invites N guests to his birthday party. They all arrive, but can only go in one at a time, when he calls them.
        He may call the same guest multiple times. Guests announce they have all entered the labyrinth, so that the minotaur is aware.
        Now the guests must come up with a strategy to let the Minotaur know that every guest entered the Minotaur’s labyrinth.
*/
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <format>
#include <random>

using namespace std;

mutex mu;                           // object for lock
const int N = 5;                    // Number of Guests
bool cupcakeAvailable = true;       // Initially, a cupcake is available
vector<bool> hadCupcake(N, false);  // Keeps track if guest has had cupcake
int entered = 0;                    // Number of guests entered, kept track of by counterGuest
int currentGuest;                   // Current guest called by minotaur

// Random number generator
int getRandomInt(int min, int max){
    random_device seed;
    mt19937 rng(seed());
    uniform_int_distribution<mt19937::result_type> dist(min, max);
    return dist(rng);
}

// Guest keeping track of amount cupcakes replaced 
void counterGuest_inLab(){
    while (entered < N){   
        mu.lock();
        if(currentGuest == 0){ // waiting until thread is called
            if (!hadCupcake[0] && cupcakeAvailable){ // If we haven't had a cupcake and theres one available, eat it.
                cupcakeAvailable = false;
                hadCupcake[0] = true;
                cout << "Guest 1: I have had a cupcake\n" << endl;
            }
            else if (!cupcakeAvailable){ // If theres not cupcake, somebody ate it, replace it. 
                cupcakeAvailable = true;
                entered++;
                cout << "Guest 1: I replaced a cupcake\n" << endl;
            }
        }
        mu.unlock();
    }
}

// Other guests
void guest_inLab(int idx){
    while (entered < N){
        mu.lock();
        if (currentGuest == idx){ // waiting until thread is called
            if (!hadCupcake[idx]){
                if (cupcakeAvailable){ // If we haven't had a cupcake and theres one available, eat it.
                    cupcakeAvailable = false;
                    hadCupcake[idx] = true;
                    cout << "Guest " << idx + 1 << ": I have had a cupcake\n" << endl;
                }
            }
        }
        mu.unlock();
    }
}

int main()
{
    // Vector holding our 8 threads
    vector<thread> guests;

    // Start threads
    guests.push_back(thread(counterGuest_inLab)); // Thread to count how many times it replaces the cake
    for (int i = 1; i < N; i++){
        guests.push_back(thread(guest_inLab, i));
    }

    // Simulating Minotaur randomly calling guests
    while (entered < N){
        currentGuest = getRandomInt(0, N); // Grab random guest
    }

    // Wait for all threads to complete
    for (auto &t : guests){
        t.join();
    }

    cout << "Guest 1: Everybody has had a cupcake" << endl;
    return 0;
}