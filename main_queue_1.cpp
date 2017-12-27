#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <fstream>
#include "queue.h"

using namespace std;


int main() {

    // total number of iterations
    unsigned iteration_number = 10e6;
    unsigned begin_steady_state = (iteration_number/20.0);

    // mean arrival rate and mean service rate for queues with single servers
    double lambda_1 = 0.01;
    double mu_1 = 0.02;

    // number of servers
    unsigned s_1 = 1;

    // number of steady-state probabilities we consider
    unsigned vector_size = 50*s_1;

    // initializes the three queues
    Queue queue_1 (lambda_1, mu_1, s_1);


    // initialization of the random exponential distributions
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);

    exponential_distribution<double> entry_distrib_queue_1(lambda_1);
    exponential_distribution<double> server_distrib_queue_1(mu_1);


    // keeps track of the elapsed time and the number of customers
    double total_time_waited = 0;
    double total_time = 0;
    unsigned total_customers_served = 0;

    // initialization of a vector that counts the amount of time spent in each
    // state
    vector<double> steady_state_probabilities(vector_size, 0);

    // initialization of a timer
    auto t_begin = chrono::high_resolution_clock::now();


// -------------------------------------------------------------------------- //
// ----------------------------- MAIN LOOP BEGINS --------------------------- //
// -------------------------------------------------------------------------- //


    for (int j = 0; j<iteration_number; j++) {


        // if this condition is verified it means that a customer just joined
        // server number getIndex() so the following computes the exit time
        if (queue_1.getIndex() != queue_1.getServer()) {

            double new_event_time = server_distrib_queue_1(generator);

            // checks if an event will already be happening at that time
            if (queue_1.findElement(new_event_time) != (queue_1.getServer()+1)){

                cout << "FATAL ERROR SERVER SIDE, NUMBER " << queue_1.getIndex()
                << endl; break;

            }

            queue_1.setEvent(new_event_time, queue_1.getIndex());
            total_time_waited += new_event_time;
            total_customers_served += 1;

        }

        // if this condition is verified it means that the arrival time of the
        // next customer is not yet known so the following computes it
        if (queue_1.nextArrival()) {

            double new_event_time = entry_distrib_queue_1(generator);

            // checks if an event will already be happening at that time
            if (queue_1.findElement(new_event_time) != (queue_1.getServer()+1)){

                cout << "FATAL ERROR ENTRY SIDE" << endl; break;

            }

            queue_1.setEvent(new_event_time, queue_1.getServer());

        }


// -------------------------------------------------------------------------- //
// ------------------------------- HERE IT CHANGES -------------------------- //
// -------------------------------------------------------------------------- //


        // finds the next event that will happen
        double next_event_time = queue_1.findMinElement();
        unsigned next_event_index = queue_1.findMinElement(next_event_time);

        // updates time trackers when QS is in steady_state
        if (j > begin_steady_state) {
            if ((queue_1.getQueue() + queue_1.getServer()) <
                vector_size) {

                steady_state_probabilities[queue_1.busyNumber() +
                    queue_1.getQueue()] += next_event_time;
                    total_time += next_event_time;

            }

            // checks if steady_state_probabilities is large enough for all the
            // states visited
            else {

                cout << "The number of customers in the queue is too large"
                << endl;
            }

        }

        // adds the time spent by customers standing in the queue to the tracker
        if (queue_1.getQueue() > 0) {total_time_waited +=
                                queue_1.getQueue()*next_event_time;}


        // case where the next event is a customer entering the queue
        if (next_event_index == queue_1.getServer()) {

            queue_1.setEvent(queue_1.getHigh(), queue_1.getServer());

            if (queue_1.busyNumber() != queue_1.getServer()) {

                unsigned free_server = queue_1.findFreeServer();
                queue_1.setIndex(free_server);

            }

            else {

                queue_1.incrementQueue();
                queue_1.setIndex(queue_1.getServer());

            }

        }

        // case where the next event is a customer leaving a server
        else {

            queue_1.setEvent(queue_1.getHigh(), next_event_index);

            if (queue_1.getQueue() > 0) {

                queue_1.decrementQueue();
                queue_1.setIndex(next_event_index);

            }

            else {queue_1.setIndex(queue_1.getServer());}

        }

        // updates the time it took to reach this event
        queue_1.updateEvent(next_event_time);

    }


    auto t_end = chrono::high_resolution_clock::now();
    double timer = chrono::duration_cast<chrono::seconds>(t_end - t_begin).count();

    cout << "Loop time is " << timer << " seconds" << endl;


// -------------------------------------------------------------------------- //
// --------------------------- STEADY STATE STUDY --------------------------- //
// -------------------------------------------------------------------------- //


    // opens file where to store results
    ofstream my_file;
    my_file.open("steady_state_multi.txt");

    // computes the P_0 probability of the steady state
    double value = 0;
    for (int i = 0; i < queue_1.getServer(); i++)
        {value += pow(queue_1.getLambda()/queue_1.getMu(),i)/factorial(i);}
    double P_0 = 1/(value +
                 (pow(queue_1.getLambda()/queue_1.getMu(),queue_1.getServer())/
                 factorial(queue_1.getServer()))*(1/(1-queue_1.getLambda()/
                 (queue_1.getServer()*queue_1.getMu()))));
    double observed_proba = steady_state_probabilities[0]/total_time;

    // computes the mean square error of the steady-state probabilities vector
    double mean_square_error = 0;

    my_file << "Observed P_0 is " << observed_proba << endl;
    my_file << "Theoretical P_0 is " << P_0 << endl;
    mean_square_error += pow(observed_proba - P_0,2);

    // first loop to compute the steady state probabilities
    for (int i = 1; i < (queue_1.getServer()+1); i++) {

        observed_proba = steady_state_probabilities[i]/total_time;

        my_file << "Observed P_" << i << " is " << observed_proba << endl;

        double theoretical_proba =
            (pow((queue_1.getLambda()/queue_1.getMu()),i)*P_0)/factorial(i);

        my_file << "Theoretical P_" << i << " is " << theoretical_proba << endl;

        mean_square_error += pow(observed_proba - theoretical_proba,2);

    }

    // second loop to compute the steady state probabilities
    for (int i = (queue_1.getServer()+1); i < vector_size; i++) {

        observed_proba = steady_state_probabilities[i]/total_time;

        my_file << "Observed P_" << i << " is " << observed_proba << endl;

        double theoretical_proba =
            (pow((queue_1.getLambda()/queue_1.getMu()),i)*P_0)/
            (factorial(queue_1.getServer())*
            pow(queue_1.getServer(),i-queue_1.getServer()));

        my_file << "Theoretical P_" << i << " is " << theoretical_proba << endl;

        mean_square_error += pow(observed_proba - theoretical_proba,2);

    }

    cout << "The mean square error is " << mean_square_error << endl;

    // computes the observed mean waiting time
    double mean_waiting_time = total_time_waited/
                                ((double) total_customers_served);
    cout << "Observed waiting time is " << mean_waiting_time << endl;

    // computes the theoretical mean waiting time
    double waiting_time = 1/queue_1.getMu() +
        (P_0*pow(queue_1.getLambda()/queue_1.getMu(),queue_1.getServer())
        *queue_1.rho())/(queue_1.getLambda()*factorial(queue_1.getServer())
        *(1-queue_1.rho())*(1-queue_1.rho()));
    cout << "Theoretical waiting time is " << waiting_time << endl;

    my_file.close();

    return 0;
}
