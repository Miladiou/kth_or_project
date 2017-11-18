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
    unsigned iteration_number = 5000000;

    // mean arrival rate and mean service rate for queues with single servers
    double lambda_1 = 0.02;
    double lambda_2 = 0.02;
    double mu_1 = 0.003;
    double mu_2 = 0.003;

    // mean service rates for dual servers
    double mu_11 = 0.003;
    double mu_22 = 0.003;

    // proportion of customers sent in the queue with dual servers
    double alpha_1 = 0.000000000000001;
    double alpha_2 = 0.999999999999999;

    // number of servers
    unsigned s_1 = 10;
    unsigned s_2 = 10;
    unsigned s_12 = 10;

    // true if you want to have the program display each event
    bool display = false;


    // initializes the three queues
    Queue queue_1 (alpha_1*lambda_1, mu_1, s_1);
    Queue queue_2 (alpha_2*lambda_2, mu_2, s_2);
    DoubleQueue queue_12 ((1-alpha_1)*lambda_1 + (1-alpha_2)*lambda_2, mu_11, mu_22, s_12);


    // initialization of the random exponential distributions
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);

    exponential_distribution<double> entry_distrib_queue_1(alpha_1*lambda_1);
    exponential_distribution<double> entry_distrib_queue_2(alpha_2*lambda_2);

    exponential_distribution<double> server_distrib_queue_1(mu_1);
    exponential_distribution<double> server_distrib_queue_2(mu_2);

    exponential_distribution<double>
        entry_distrib_queue_11((1-alpha_1)*lambda_1);
    exponential_distribution<double>
        entry_distrib_queue_22((1-alpha_2)*lambda_2);
    exponential_distribution<double> server_distrib_queue_11(mu_11);
    exponential_distribution<double> server_distrib_queue_22(mu_22);


    // keeps track of the elapsed time and the number of customers
    double total_time_waited_1 = 0;
    double total_time_1 = 0;
    unsigned total_customers_served_1 = 0;

    // keeps track of the elapsed time and the number of customers
    double total_time_waited_2 = 0;
    double total_time_2 = 0;
    unsigned total_customers_served_2 = 0;

    // keeps track of the elapsed time and the number of customers
    double total_time_waited_12 = 0;
    double total_time_12 = 0;
    unsigned total_customers_served_12 = 0;

    // initialization of a vector that counts the amount of time spent in each
    // state
    vector<double> steady_state_probabilities_1(10*queue_1.getServer(), 0);
    vector<double> steady_state_probabilities_2(10*queue_2.getServer(), 0);
    vector<double> steady_state_probabilities_12(10*queue_12.getServer(), 0);

    // stores the time before next event for each of the queues
    vector<double> best_event_time(3);

    // initialization: computes arrival time of first customer in each queue
    double new_event_time = entry_distrib_queue_1(generator);
    queue_1.setEvent(new_event_time, queue_1.getServer());
    best_event_time[0] = new_event_time;

    new_event_time = entry_distrib_queue_2(generator);
    queue_2.setEvent(new_event_time, queue_2.getServer());
    best_event_time[1] = new_event_time;

    double new_event_time_11 = entry_distrib_queue_11(generator);
    double new_event_time_22 = entry_distrib_queue_22(generator);

    queue_12.setEvent(new_event_time_11, queue_12.getServer());
    queue_12.setEvent(new_event_time_22, queue_12.getServer() + 1);
    best_event_time[2] = min(new_event_time_11,new_event_time_22);

    // initialization of a timer
    auto t_begin = chrono::high_resolution_clock::now();


// -------------------------------------------------------------------------- //
// ----------------------------- MAIN LOOP BEGINS --------------------------- //
// -------------------------------------------------------------------------- //


    for (int j = 0; j<iteration_number; j++) {

        if (display == true) {cout << endl;}

        unsigned last_event;

        // queue_1
        if (last_event == 1) {

            // if this condition is verified it means that a customer just
            // joined server number getIndex() so the following computes the
            // exit time
            if (queue_1.getIndex() != queue_1.getServer()) {

                double new_event_time = server_distrib_queue_1(generator);

                // checks if an event will already be happening at that time
                if (queue_1.findElement(new_event_time) !=
                    (queue_1.getServer()+1)) {

                    cout << "FATAL ERROR SERVER SIDE, NUMBER " <<
                        queue_1.getIndex() << endl;
                    break;

                }

                queue_1.setEvent(new_event_time, queue_1.getIndex());
                total_time_waited_1 += new_event_time;
                total_customers_served_1 += 1;

            }


            // if this condition is verified it means that the arrival time of
            // the next customer is not yet known so the following computes it
            if (queue_1.nextArrival()) {

                double new_event_time = entry_distrib_queue_1(generator);

                // checks if an event will already be happening at that time
                if (queue_1.findElement(new_event_time) !=
                    (queue_1.getServer()+1)){

                    cout << "FATAL ERROR ENTRY SIDE" << endl; break;

                }

                queue_1.setEvent(new_event_time, queue_1.getServer());

            }

        }



        // queue_2
        else if (last_event == 2) {

            // if this condition is verified it means that a customer just
            // joined server number getIndex() so the following computes the
            // exit time
            if (queue_2.getIndex() != queue_2.getServer()) {

                double new_event_time = server_distrib_queue_2(generator);

                // checks if an event will already be happening at that time
                if (queue_2.findElement(new_event_time) !=
                    (queue_2.getServer()+1)) {

                    cout << "FATAL ERROR SERVER SIDE, NUMBER " <<
                        queue_2.getIndex() << endl;
                    break;

                }

                queue_2.setEvent(new_event_time, queue_2.getIndex());
                total_time_waited_2 += new_event_time;
                total_customers_served_2 += 1;

            }


            // if this condition is verified it means that the arrival time of
            // the next customer is not yet known so the following computes it
            if (queue_2.nextArrival()) {

                double new_event_time = entry_distrib_queue_2(generator);

                // checks if an event will already be happening at that time
                if (queue_2.findElement(new_event_time) !=
                    (queue_2.getServer()+1)){

                    cout << "FATAL ERROR ENTRY SIDE" << endl; break;

                }

                queue_2.setEvent(new_event_time, queue_2.getServer());

            }

        }


        // queue_12
        else {

            // if this condition is verified it means that a customer just
            // joined server number getIndex() so the following computes the
            // exit time
            if (queue_12.getIndex() < queue_12.getServer()) {

                // if this condition is verified it means a customer of service
                // class 1 just joined a server

                double new_event_time;

                if (last_event == 11) {

                    new_event_time = server_distrib_queue_11(generator);

                }

                // else a customer of service class 2 just joined a server
                else {

                    new_event_time = server_distrib_queue_22(generator);

                }

                // checks if an event will already be happening at that time
                if (queue_12.findElement(new_event_time) !=
                    (queue_12.getServer()+2)) {

                    cout << "FATAL ERROR SERVER SIDE, NUMBER " <<
                        queue_12.getIndex() << endl;
                    break;

                }

                queue_12.setEvent(new_event_time, queue_12.getIndex());
                total_time_waited_12 += new_event_time;
                total_customers_served_12 += 1;

            }


            // if this condition is verified it means that the arrival time of
            // the next customer is not yet known so the following computes it
            if (queue_12.nextArrival()) {

                if (queue_12.getEvent(queue_12.getServer()) ==
                    queue_12.getHigh()) {

                    double new_event_time = entry_distrib_queue_11(generator);
                    queue_12.setEvent(new_event_time, queue_12.getServer());

                }

                else {

                    double new_event_time = entry_distrib_queue_22(generator);
                    queue_12.setEvent(new_event_time, queue_12.getServer() + 1);

                }

            }

        }


// -------------------------------------------------------------------------- //
// ------------------------------- HERE IT CHANGES -------------------------- //
// -------------------------------------------------------------------------- //


        // finds the next event that will happen for each queue
        double next_event_time = queue_1.findMinElement();
        best_event_time[0] = next_event_time;
        next_event_time = queue_2.findMinElement();
        best_event_time[1] = next_event_time;
        next_event_time = queue_12.findMinElement();
        best_event_time[2] = next_event_time;

        // gets the queue which has the shortest delay before next event takes
        // place
        auto pointer = min_element(best_event_time.begin(),
            best_event_time.end());
        last_event = distance(best_event_time.begin(), pointer);
        if (last_event == 2){last_event = 3;}
        else {last_event = last_event + 1;}

        next_event_time = *pointer;


        // updates time trackers
        if ((queue_1.getQueue() + queue_1.getServer()) < 10*queue_1.getServer())
        {

            steady_state_probabilities_1[queue_1.busyNumber() +
                queue_1.getQueue()] += next_event_time;
            total_time_1 += next_event_time;

        }

        if ((queue_2.getQueue() + queue_2.getServer()) < 10*queue_2.getServer())
        {
            steady_state_probabilities_2[queue_2.busyNumber() +
                queue_2.getQueue()] += next_event_time;
            total_time_2 += next_event_time;

        }

        if ((queue_12.getQueue() + queue_12.getServer()) <
            10*queue_12.getServer()) {

            steady_state_probabilities_12[queue_12.busyNumber() +
                queue_12.getQueue()] += next_event_time;
            total_time_12 += next_event_time;

        }

        // adds the time spent by customers standing in the queue to the tracker
        total_time_waited_1 += queue_1.getQueue()*next_event_time;
        total_time_waited_2 += queue_2.getQueue()*next_event_time;
        total_time_waited_12 += queue_12.getQueue()*next_event_time;


        // case where the next event happens in relation with queue_1
        if (last_event == 1) {

            unsigned next_event_index = queue_1.findMinElement(next_event_time);

            // case where the next event is a customer entering the queue
            if (next_event_index == queue_1.getServer()) {

                if (display == true) {
                    cout << "A customer joins queue_1." << endl;
                }

                queue_1.setEvent(queue_1.getHigh(), queue_1.getServer());

                if (queue_1.busyNumber() != queue_1.getServer()) {

                    unsigned free_server = queue_1.findFreeServer();
                    queue_1.setIndex(free_server);

                    if (display == true) {
                        cout << "This customer now joins server number " <<
                            free_server << "." << endl;
                    }

                }

                else {

                    queue_1.incrementQueue();
                    queue_1.setIndex(queue_1.getServer());

                }

            }

            // case where the next event is a customer leaving a server
            else {

                if (display == true) {
                    cout << "A customer leaves queue_1 at server number " <<
                        next_event_index << "." << endl;
                    }

                queue_1.setEvent(queue_1.getHigh(), next_event_index);

                if (queue_1.getQueue() > 0) {

                    queue_1.decrementQueue();
                    queue_1.setIndex(next_event_index);

                    if (display == true) {
                        cout <<
                            "A new customer from the queue joins this server."
                             << endl;
                    }

                }

                else {queue_1.setIndex(queue_1.getServer());}

            }

        }



        // case where the next event happens in relation with queue_2
        if (last_event == 2) {

            unsigned next_event_index = queue_2.findMinElement(next_event_time);

            // case where the next event is a customer entering the queue
            if (next_event_index == queue_2.getServer()) {

                if (display == true) {
                    cout << "A customer joins queue_2." << endl;
                }

                queue_2.setEvent(queue_2.getHigh(), queue_2.getServer());

                if (queue_2.busyNumber() != queue_2.getServer()) {

                    unsigned free_server = queue_2.findFreeServer();
                    queue_2.setIndex(free_server);

                    if (display == true) {
                    cout << "This customer now joins server number " <<
                        free_server << "." << endl;
                    }

                }

                else {

                    queue_2.incrementQueue();
                    queue_2.setIndex(queue_2.getServer());

                }

            }

            // case where the next event is a customer leaving a server
            else {

                if (display == true) {
                    cout << "A customer leaves queue_2 at server number " <<
                        next_event_index << "." << endl;
                }

                queue_2.setEvent(queue_2.getHigh(), next_event_index);

                if (queue_2.getQueue() > 0) {

                    queue_2.decrementQueue();
                    queue_2.setIndex(next_event_index);

                    if (display == true) {
                        cout <<
                            "A new customer from the queue joins this server."
                            << endl;
                    }

                }

                else {queue_2.setIndex(queue_2.getServer());}

            }

        }



        // case where the next event happens in relation with queue_12
        if (last_event == 3) {

            unsigned next_event_index =
                queue_12.findMinElement(next_event_time);


            // case where the next event is a customer of service class 1
            // entering the queue
            if ((next_event_index == queue_12.getServer()) ||
                (next_event_index == (queue_12.getServer() + 1))) {

                if (next_event_index == queue_12.getServer()) {

                    last_event = 11;

                    if (display == true) {
                        cout <<
                            "A new customer of service class 1 joins queue_12."
                            << endl;
                    }

                }

                else {

                    last_event = 22;

                    if (display == true) {
                        cout <<
                            "A new customer of service class 2 joins queue_12."
                            << endl;
                    }

                }

                queue_12.setEvent(queue_12.getHigh(), next_event_index);

                if (queue_12.busyNumber() != queue_12.getServer()) {

                    unsigned free_server = queue_12.findFreeServer();
                    queue_12.setIndex(free_server);

                    if (display == true) {
                        cout << "This customer now joins server number " <<
                            free_server << "." << endl;
                    }

                }

                else {

                    queue_12.incrementQueue();
                    queue_12.setIndex(queue_12.getServer());
                    if (last_event == 11) {queue_12.setOrder(false);}
                    else {queue_12.setOrder(true);}

                }

            }

            // case where the next event is a customer leaving a server
            else {

                if (display == true) {
                    cout << "A customer leaves queue_12 at server number " <<
                        next_event_index << "." << endl;
                }

                queue_12.setEvent(queue_12.getHigh(), next_event_index);

                if (queue_12.getQueue() > 0) {

                    if (display == true) {
                        cout <<
                            "A new customer from the queue joins this server."
                            << endl;
                    }

                    if (queue_12.getOrder() == false) {last_event = 11;}
                    else {last_event = 22;}
                    queue_12.decrementQueue();
                    queue_12.setIndex(next_event_index);

                }

                else {queue_12.setIndex(queue_12.getServer());}

            }

        }

        /*
        cout << "For queue_1 ";
        queue_1.printEvent();
        cout << "For queue_2 ";
        queue_2.printEvent();
        cout << "For queue_12 ";
        queue_12.printEvent();

        cout << endl;
        */

        // updates the time it took to reach this event
        queue_1.updateEvent(next_event_time);
        queue_2.updateEvent(next_event_time);
        queue_12.updateEvent(next_event_time);

        /*
        cout << "For queue_1 ";
        queue_1.printEvent();
        cout << "For queue_2 ";
        queue_2.printEvent();
        cout << "For queue_12 ";
        queue_12.printEvent();
        */


    }


    auto t_end = chrono::high_resolution_clock::now();
    double timer = chrono::duration_cast<chrono::milliseconds>(t_end - t_begin).count();

    cout << "Loop time is " << timer << endl;


// -------------------------------------------------------------------------- //
// --------------------------- STEADY STATE STUDY --------------------------- //
// -------------------------------------------------------------------------- //


    // opens file where to store results
    ofstream my_file_1;
    ofstream my_file_2;
    ofstream my_file_12;
    my_file_1.open("steady_state_multi_1.txt");
    my_file_2.open("steady_state_multi_2.txt");
    my_file_12.open("steady_state_multi_12.txt");

    // computes the mean square error of the steady-state probabilities vector
    double mean_square_error_1 = 0;
    double mean_square_error_2 = 0;


    // computes the P_0 probability of the steady state for queue_1
    double value = 0;
    for (int i = 0; i < queue_1.getServer(); i++)
        {value += pow(queue_1.getLambda()/queue_1.getMu(),i)/factorial(i);}
    double P_0_1 = 1/(value +
                 (pow(queue_1.getLambda()/queue_1.getMu(),queue_1.getServer())/
                 factorial(queue_1.getServer()))*(1/(1-queue_1.getLambda()/
                 (queue_1.getServer()*queue_1.getMu()))));
    double observed_proba = steady_state_probabilities_1[0]/total_time_1;

    my_file_1 << "Observed P_0 is " << observed_proba << endl;
    my_file_1 << "Theoretical P_0 is " << P_0_1 << endl;
    mean_square_error_1 += pow(observed_proba - P_0_1, 2);


    // computes the P_0 probability of the steady state for queue_2
    value = 0;
    for (int i = 0; i < queue_2.getServer(); i++)
        {value += pow(queue_2.getLambda()/queue_2.getMu(),i)/factorial(i);}
    double P_0_2 = 1/(value +
                 (pow(queue_2.getLambda()/queue_2.getMu(),queue_2.getServer())/
                 factorial(queue_2.getServer()))*(1/(1-queue_2.getLambda()/
                 (queue_2.getServer()*queue_2.getMu()))));
    observed_proba = steady_state_probabilities_2[0]/total_time_2;

    my_file_2 << "Observed P_0 is " << observed_proba << endl;
    my_file_2 << "Theoretical P_0 is " << P_0_2 << endl;
    mean_square_error_2 += pow(observed_proba - P_0_2, 2);


    // first loop to compute the steady state probabilities for queue_1
    for (int i = 1; i < (queue_1.getServer()+1); i++) {

        observed_proba = steady_state_probabilities_1[i]/total_time_1;

        my_file_1 << "Observed P_" << i << " is " << observed_proba << endl;

        double theoretical_proba =
            (pow((queue_1.getLambda()/queue_1.getMu()),i)*P_0_1)/factorial(i);

        my_file_1 << "Theoretical P_" << i << " is " << theoretical_proba <<
            endl;

        mean_square_error_1 += pow(observed_proba - theoretical_proba,2);

    }

    // second loop to compute the steady state probabilities for queue_1
    for (int i = (queue_1.getServer()+1); i < 10*queue_1.getServer(); i++) {

        observed_proba = steady_state_probabilities_1[i]/total_time_1;

        my_file_1 << "Observed P_" << i << " is " << observed_proba << endl;

        double theoretical_proba =
            (pow((queue_1.getLambda()/queue_1.getMu()),i)*P_0_1)/
            (factorial(queue_1.getServer())*
            pow(queue_1.getServer(),i-queue_1.getServer()));

        my_file_1 << "Theoretical P_" << i << " is " << theoretical_proba <<
            endl;

        mean_square_error_1 += pow(observed_proba - theoretical_proba,2);

    }


    // first loop to compute the steady state probabilities for queue_2
    for (int i = 1; i < (queue_2.getServer()+1); i++) {

        observed_proba = steady_state_probabilities_2[i]/total_time_2;

        my_file_2 << "Observed P_" << i << " is " << observed_proba << endl;

        double theoretical_proba =
            (pow((queue_2.getLambda()/queue_2.getMu()),i)*P_0_2)/factorial(i);

        my_file_2 << "Theoretical P_" << i << " is " << theoretical_proba <<
            endl;

        mean_square_error_2 += pow(observed_proba - theoretical_proba,2);

    }

    // second loop to compute the steady state probabilities for queue_2
    for (int i = (queue_2.getServer()+1); i < 10*queue_2.getServer(); i++) {

        observed_proba = steady_state_probabilities_2[i]/total_time_2;

        my_file_2 << "Observed P_" << i << " is " << observed_proba << endl;

        double theoretical_proba =
            (pow((queue_2.getLambda()/queue_2.getMu()),i)*P_0_2)/
            (factorial(queue_2.getServer())*
            pow(queue_2.getServer(),i-queue_2.getServer()));

        my_file_2 << "Theoretical P_" << i << " is " << theoretical_proba <<
            endl;

        mean_square_error_2 += pow(observed_proba - theoretical_proba,2);

    }

    // computes the steady state probabilities for queue_12
    for (int i = 0; i < 10*queue_12.getServer(); i++) {

        observed_proba = steady_state_probabilities_12[i]/total_time_12;

        my_file_12 << "Observed P_" << i << " is " << observed_proba << endl;

    }

    cout << "Mean square error for queue_1 is " << mean_square_error_1 << endl;
    cout << "Mean square error for queue_2 is " << mean_square_error_2 << endl;


    // computes the observed mean waiting time for queue_1
    double mean_waiting_time_1 = total_time_waited_1/
                                ((double) total_customers_served_1);
    cout << "Observed waiting time for queue_1 is " << mean_waiting_time_1
        << endl;

    // computes the theoretical mean waiting time for queue_1
    double waiting_time_1 = 1/queue_1.getMu() +
        (P_0_1*pow(queue_1.getLambda()/queue_1.getMu(),queue_1.getServer())
        *queue_1.rho())/(queue_1.getLambda()*factorial(queue_1.getServer())
        *(1-queue_1.rho())*(1-queue_1.rho()));
    cout << "Theoretical waiting time for queue_1 is " << waiting_time_1 <<
        endl;


    // computes the observed mean waiting time for queue_2
    double mean_waiting_time_2 = total_time_waited_2/
                                ((double) total_customers_served_2);
    cout << "Observed waiting time for queue_2 is " << mean_waiting_time_2
        << endl;

    // computes the theoretical mean waiting time for queue_2
    double waiting_time_2 = 1/queue_2.getMu() +
        (P_0_2*pow(queue_2.getLambda()/queue_2.getMu(),queue_2.getServer())
        *queue_2.rho())/(queue_2.getLambda()*factorial(queue_2.getServer())
        *(1-queue_2.rho())*(1-queue_2.rho()));
    cout << "Theoretical waiting time for queue_2 is " << waiting_time_2 <<
        endl;


    my_file_1.close();
    my_file_2.close();
    my_file_12.close();

    return 0;

}
