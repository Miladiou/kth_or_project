#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;


double factorial (int n) {

    if ((n == 0) or (n == 1)) {return(1);}
    else {return(n*factorial(n-1));}

}


int main() {

    // mean arrival rate and mean service rate
    double lambda = 0.1;
    double mu = 0.01;
    // number of servers
    unsigned s = 30;
    double rho = lambda/(s*mu);

    double high_value = 1e30;

    // turns on/off tracking messages
    bool pollution = false;

    // parameters to compute the steady state probabilities
    unsigned vector_size = 50;

    // computes the mean square error of the steady-state probabilities vector
    double mean_square_error = 0;

    // number of customers currently in the queuing system (QS)
    unsigned current_state = 0;
    // number of customers currently in the queue
    unsigned current_queue = 0;

    // total number of iterations
    unsigned iteration_number = 5000000;

    // computes the mean waiting time of the customers in the QS
    double total_time_waited = 0;
    double total_time = 0;
    unsigned total_customers_served = 0;

    // initialization of the random distribution between 0 and 1
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
    exponential_distribution<double> expo_distribution_entry(lambda);
    exponential_distribution<double> expo_distribution_server(mu);

    // initialization of a boolean vector which indicates which servers are busy
    vector<bool> busy(s);
    // initialization of a double vector which indicates the time remaining
    // before the event corresponding to a given index happens
    vector<double> next_event(s+1,high_value);
    // initialization of a vector that counts the amount of time spent in each
    // state
    vector<double> steady_state_probabilities(vector_size, 0);

    // initialization of a timer
    //auto t_begin = chrono::high_resolution_clock::now();


    for (int j = 0; j<iteration_number; j++) {

        for (int i = 0; i < s; i++) {

            if ((next_event[i] == high_value) and (busy[i] == 1)) {

                double new_event_time = expo_distribution_server(generator);
                // checks if an event will already be happening at that time
                if (find(next_event.begin(), next_event.end(), new_event_time)
                    != next_event.end()) {cout
                     << "FATAL ERROR SERVER SIDE, NUMBER " << i << endl; break;}
                else {

                    next_event[i] = new_event_time;
                    total_time_waited += new_event_time;
                    total_customers_served += 1;

                }

                if (pollution == true){cout << "Server distrib is "
                                       << new_event_time << endl;}

            }

        }

        if (next_event[s] == high_value) {

            double new_event_time = expo_distribution_entry(generator);
            // checks if an event will already be happening at that time
            if (find(next_event.begin(), next_event.end(), new_event_time)
                != next_event.end()) {cout << "FATAL ERROR CLIENT SIDE" << endl;
                break;}
            else {next_event[s] = new_event_time;}

            if (pollution == true){cout << "Entry distrib is "
                                   << new_event_time << endl;}

        }


// ------------------------------- HERE IT CHANGES -------------------------- //


        auto time_pointer = min_element(next_event.begin(),
            next_event.end());
        double next_event_time = *time_pointer;
        auto index_pointer = find(next_event.begin(), next_event.end(),
            next_event_time);
        unsigned next_event_index = distance(next_event.begin(), index_pointer);

        if (current_state < vector_size) {

            steady_state_probabilities[current_state] += next_event_time;
            total_time += next_event_time;

        }

        if (current_queue > 0) {total_time_waited +=
                                current_queue*next_event_time;}


        if (next_event_index == (s+1)) {cout << "ERROR WITH FIND FUNCTION" <<
            endl;}

        if (next_event_index == s) {

            if (pollution == true){cout
                << "A new customer has joined the QS. Välkommen !" << endl;}
            current_state += 1;
            next_event[s] = high_value;
            auto index_pointer = find(busy.begin(), busy.end(), 0);
            unsigned index = distance(busy.begin(), index_pointer);

            if (index < busy.size()) {

                if (pollution == true){cout
                    << "The new customer has joined server " << index << endl;}
                busy[index] = 1;

            }

            else {current_queue += 1;}

        }

        else {

            if (pollution == true){cout
                 << "A customer leaves the QS from server number "
                 << next_event_index << endl;}
            current_state -= 1;
            next_event[next_event_index] = high_value;

            if (current_queue > 0) {current_queue -= 1;}
            else {busy[next_event_index] = 0;}

        }

        // updates "next_event"
        for (int i = 0; i < (s+1); i++) {

            if (next_event[i] != high_value)
                {next_event[i] = next_event[i] - next_event_time;}

        }

        if (pollution == true) {

            cout << "Current number of people in the QS " << current_state
                << endl;
            cout << "Current number of people in the queue " << current_queue
                << endl;
            cout << "The total number of iterations is currently " << j << endl;

        }

    }


    ofstream my_file;
    my_file.open("steady_state.txt");

    double value = 0;
    for (int i = 0; i<s; i++) {value += pow(lambda/mu,i)/factorial(i);}
    double P_0 = 1/(value +
        (pow(lambda/mu,s)/factorial(s))*(1/(1-lambda/(s*mu))));
    double observed_proba = steady_state_probabilities[0]/total_time;

    my_file << "Observed P_0 is " << observed_proba << endl;
    my_file << "Theoretical P_0 is " << P_0 << endl;
    mean_square_error += pow(observed_proba - P_0,2);

    for (int i = 1; i < (s+1); i++) {

        observed_proba = steady_state_probabilities[i]/total_time;
        my_file << "Observed P_" << i << " is " << observed_proba << endl;

        double theoretical_proba = (pow((lambda/mu),i)*P_0)/factorial(i);
        my_file << "Theoretical P_" << i << " is " << theoretical_proba << endl;

        mean_square_error += pow(observed_proba - theoretical_proba,2);

    }

    for (int i = (s+1); i < vector_size; i++) {

        observed_proba = steady_state_probabilities[i]/total_time;
        my_file << "Observed P_" << i << " is " << observed_proba << endl;

        double theoretical_proba =
            (pow((lambda/mu),i)*P_0)/(factorial(s)*pow(s,i-s));
        my_file << "Theoretical P_" << i << " is " << theoretical_proba << endl;

        mean_square_error += pow(observed_proba - theoretical_proba,2);

    }

    cout << "The mean square error is " << mean_square_error << endl;

    my_file.close();


    // computes the observed mean waiting time
    cout << "total_time_waited is " << total_time_waited << endl;
    cout << "total_customers_served is " << total_customers_served << endl;
    double mean_waiting_time = total_time_waited /
        ((double) total_customers_served);
    cout << "Mean waiting time is " << mean_waiting_time << endl;

    // computes the theoretical mean waiting time
    double waiting_time = 1/mu +
        (P_0*pow(lambda/mu,s)*rho)/(lambda*factorial(s)*(1-rho)*(1-rho));
    cout << "Expected waiting time is " << waiting_time << endl;

    return 0;

}
