#include <vector>

#ifndef QUEUE_H
#define QUEUE_H

using namespace std;


class Queue {

private:

    double lambda, mu, high_value;
    unsigned server_number, current_queue, new_server_time_index;
    vector<double> next_event;
    vector<bool> busy;

public:

    // constructor
    Queue (double a, double b, unsigned c);


    // getters
    double getLambda();
    double getMu();
    double getHigh();
    unsigned getIndex();
    unsigned getServer();
    unsigned getQueue();
    double getEvent(unsigned index);
    vector<double> getVector();

    // setters
    void setLambda(double a);
    void setEvent(double value, unsigned index);
    void incrementQueue();
    void decrementQueue();
    void setIndex(unsigned index);


    // computes the rho coefficient
    double rho ();

    // returns the index of the first occurence of an element in next_event
    unsigned findElement(double element);
    // returns the value of the minimum element in next_event
    double findMinElement();
    // returns the index of the minimum value in next_event
    unsigned findMinElement(double value);
    // returns the number of servers that are currently busy
    unsigned busyNumber();
    // returns a boolean value to assess if a new time should be computed for
    // the next arrival
    bool nextArrival();
    // finds a free server and returns its index
    unsigned findFreeServer();
    // update the delay before the next events as one event happens
    void updateEvent(double delay);
    // prints the components of next_event
    void printEvent();
    // expands the next_event vector for use by the DoubleQueue class
    void Expand();

};


// DoubleQueue inherits the attributes and methods of Queue
class DoubleQueue : public Queue {

private:

    double mu_2;
    vector<bool> current_order;

public:

    // constructor
    DoubleQueue (double a, double b, double d, unsigned c);

    // getters

    // returns 0 or 1 depending on the service class of the next customer in the
    // queue
    bool getNext();
    // returns 0 or 1 depending on the service class of the customer that has
    // been in the queue for the longest period of time
    bool getOrder();

    // setters

    // stores the service class of the entering customer
    void setOrder(bool service);

    // overloaded methods

    // returns a boolean value to assess if a new time should be computed for
    // one the next arrivals
    bool nextArrival();
    // update the delay before the next events as one event happens
    void updateEvent(double delay);
    // prints the components of next_event
    void printEvent();

};


// returns factorial of input n
double factorial(int n);


#endif
