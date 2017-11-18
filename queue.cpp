#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include "queue.h"

using namespace std;


// constructor for class "Queue"
Queue::Queue (double a, double b, unsigned c) {
    lambda = a;
    mu = b;
    server_number = c;
    current_queue = 0;
    new_server_time_index = c;
    // initialization value for the vector of next events
    high_value = 1e10;
    for (int i = 0; i < (c+1); i++) {
        next_event.push_back(high_value);
    }
    exponential_distribution<double> entry_expo_distribution(a);
    exponential_distribution<double> server_expo_distribution(b);
}

// constructor for derived class "DoubleQueue"
DoubleQueue::DoubleQueue (double a, double b, double d, unsigned c)
    : Queue(a,b,c) {
    mu_2 = d;
    // the size of current_order can be increased to better model an infinite
    // queue
    vector<bool> current_order;
    Expand();
}


// getters

double Queue::getLambda() {
    return lambda;
}

double Queue::getMu() {
    return mu;
}

double Queue::getHigh() {
    return high_value;
}

unsigned Queue::getIndex() {
    return new_server_time_index;
}

unsigned Queue::getServer() {
    return server_number;
}

unsigned Queue::getQueue() {
    return current_queue;
}

double Queue::getEvent(unsigned index) {
    return next_event[index];
}

vector<double> Queue::getVector() {
    return next_event;
}


// setters

void Queue::setLambda(double a) {
    lambda = a;
}

void Queue::setEvent(double value, unsigned index) {
    next_event[index] = value;
}

void Queue::incrementQueue() {
    current_queue ++;
}

void Queue::decrementQueue() {
    current_queue --;
}

void Queue::setIndex(unsigned index) {
    new_server_time_index = index;
}

// other methods

// computes the rho coefficient for a queue with single servers
double Queue::rho () {
    return lambda/(server_number*mu);
    }

// returns the index of the first occurence of an element in next_event
unsigned Queue::findElement(double element) {
    auto pointer = find(next_event.begin(), next_event.end(), element);
    unsigned index = distance(next_event.begin(), pointer);
    return index;
}

// returns the value of the minimum element in next_event
double Queue::findMinElement() {
    auto pointer = min_element(next_event.begin(), next_event.end());
    double next_event_time = *pointer;
    return next_event_time;
}

// returns the index of the minimum value in next_event
unsigned Queue::findMinElement(double value) {
    auto pointer = find(next_event.begin(), next_event.end(), value);
    unsigned next_event_index = distance(next_event.begin(), pointer);
    return next_event_index;
}

// returns the number of servers that are currently busy
unsigned Queue::busyNumber() {
    unsigned counter = count(next_event.begin(), (next_event.end()-1),
        high_value);
    return (server_number - counter);
}

// returns a boolean value to assess if a new time should be computed for the
// next arrival
bool Queue::nextArrival() {
    if (next_event[server_number] == high_value) {return true;}
    return false;
}

// finds a free server and returns its index
unsigned Queue::findFreeServer() {
    for (int i = 0; i<server_number; i++) {
        if (next_event[i] == high_value) {
            return i;
        }
    }
}

// update the delay before the next events as one event happens
void Queue::updateEvent(double delay) {
    for (int i = 0; i<(server_number+1); i++) {
        if (next_event[i] != high_value) {
            next_event[i] -= delay;
        }
    }
}

// prints the components of next_event
void Queue::printEvent() {
    cout << "The event vector is [";
    for (int i = 0; i < server_number; i++) {
        cout << next_event[i] << " ,";
    }
    cout << next_event[server_number];
    cout << "]" << "\n";
}

// expands the next_event vector for use by the DoubleQueue class
void Queue::Expand() {
    next_event.push_back(high_value);
}


// getters for the derived class "DoubleQueue"


// returns 0 or 1 depending on the service class of the next customer in the
// queue
bool DoubleQueue::getNext() {
    return current_order[0];
}

// returns 0 or 1 depending on the service class of the customer that has
// been in the queue for the longest period of time
bool DoubleQueue::getOrder() {
    bool result = current_order[getQueue() - 1];
    current_order.pop_back();
    return result;
}

// stores the service class of the entering customer
void DoubleQueue::setOrder(bool service) {
    if (service == false) {current_order.insert(current_order.begin(), false);}
    else {current_order.insert(current_order.begin(), true);}
}



// returns a boolean value to assess if a new time should be computed for
// one the next arrivals
bool DoubleQueue::nextArrival() {
    if ((getEvent(getServer()) != getHigh()) and
        (getEvent(getServer() + 1) != getHigh())) {
        return false;
    }
    return true;
}

// update the delay before the next events as one event happens
void DoubleQueue::updateEvent(double delay) {
    for (int i = 0; i<(getServer()+2); i++) {
        if (getEvent(i) != getHigh()) {
            setEvent((getEvent(i) - delay), i);
        }
    }
}

// prints the components of next_event
void DoubleQueue::printEvent() {
    cout << "The event vector is [";
    for (int i = 0; i < (getServer() + 1); i++) {
        cout << getEvent(i) << " ,";
    }
    cout << getEvent(getServer() + 1);
    cout << "]" << "\n";
}

// returns the number of servers that are currently busy
unsigned DoubleQueue::busyNumber() {
    vector<double> next_event = getVector();
    unsigned counter = count(next_event.begin(), (next_event.end()-2),
        getHigh());
    return (getServer() - counter);
}




// returns factorial of input n
double factorial (int n) {

    if ((n == 0) or (n == 1)) {return(1);}
    else {return(n*factorial(n-1));}

}
