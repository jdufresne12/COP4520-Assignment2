/*
    John Dufresne 
    COP 4520
    Problem 2: Minotaur’s Crystal Vase
        The Minotaur decided to show his favorite crystal vase to his guests in a dedicated
        showroom with a single door. He did not want many guests to gather around the vase
        and accidentally break it. For this reason, he would allow only one guest at a time into
        the showroom. He asked his guests to choose from one of three possible strategies for
        viewing the Minotaur’s favorite crystal vase:

        SOLUTION CHOICE: QUEUE
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <random>
#include <vector>

using namespace std;

const int N = 20;                // Number of Guests
queue<int> Line;           // Line of people waiting to view vase
mutex mu;                       // object for lock
condition_variable viewStatus;  // CV to keep track of if the vase can be viewed or not 
int currentGuest;               // Which guest is currently viewing the vase 

void visitVase(int guest){
    unique_lock<mutex> lk(mu); // Initialize Lock
    Line.push(guest);          // Initialize Guest in Line

    //Using lock, wait until we are notified that we can enter.
    viewStatus.wait(lk, [guest] {return guest == currentGuest;});

    cout << "Guest " << guest+1 << " is viewing the vase." << endl; 

    //Exiting the showroom
    if(!Line.empty()){ // If Line isn't empty, pop
        Line.pop();
        if(!Line.empty()) // If it is still not empty, set our current guest to the guest index at the front of the line
            currentGuest = Line.front();
        else
            currentGuest = -1;
    }

    cout << "Guest " << guest+1 << " is leaving the showroom now." << endl; 

    lk.unlock();              // Unlock
    viewStatus.notify_all();  // Notify next guest to enter
}

int main(){
    vector<thread> guests; // Vector holding our threads (guests)
    
    // Start threads
    for(int i = 0; i < N; i++)
        guests.push_back(thread(visitVase, i));

    // Let the first guest in to visit the Vase
    {
        lock_guard<mutex> lk(mu);
        if(!Line.empty()){
            currentGuest = Line.front();
        }
    }
    viewStatus.notify_all();

    // Wait for all guests to view the vase
    for(auto& guest : guests){
        if(guest.joinable()){
            guest.join();
        }
    }

    return 0;
}