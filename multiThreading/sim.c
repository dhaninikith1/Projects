#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include "random437.h"


int MAXWAITPEOPLE = 800; //global declarations to keep a check on everything as said in the prompt
int CARNUM ; // N
int MAXPERCAR; // M


struct car_info {
    int * waiting_time_list;
    int single_car_load;
};


void time_check(int iteration, char* time_buf)
{
    int hour = 9 + iteration / 60;
    int minute = iteration % 60;
    int second = 0;

    char time_loop[10];
    
    // Initializes values to 0.
    struct tm time_struct = {0};

    time_struct.tm_sec = second;
    time_struct.tm_min = minute;
    time_struct.tm_hour = hour;

    strftime(time_loop, 10, "%H:%M:%S", &time_struct);

    int i;
    for(i = 0; i < 8; ++i)
    {
        time_buf[i] = time_loop[i];
    }
    time_buf[8] = '\0';
}



// We just store the data for doing the plos for Arrivals, rejects, waiting list. We also have the total statistics here.

void output_figure_data(int arrivals_list[], int rejects_list[], int waiting_list[], int waiting_time_sum, int total_steps, FILE* ride_status_file)
{
    FILE *plot_data_file = fopen("plot_data.csv", "w");
    int i;
    int total_people_arrived = 0;
    int total_people_rode = 0;
    int total_people_rejected = 0;
    int max_people_waiting_in_line = 0;
    int max_people_waiting_per_iteration = 0;
    char time_buf[10];

    fprintf(plot_data_file, "Arrivals, Rejects, Waiting Time\n");
    for(i = 0; i < total_steps; ++i)
    {
        // Writing plot data
        fprintf(plot_data_file, "%d,%d,%d\n", arrivals_list[i], rejects_list[i], waiting_list[i]);

        total_people_arrived += arrivals_list[i];
        total_people_rejected += rejects_list[i];

        if(max_people_waiting_in_line < MAXWAITPEOPLE)
        {
            if(waiting_list[i] > max_people_waiting_in_line)
            {
                max_people_waiting_in_line = waiting_list[i];
                max_people_waiting_per_iteration = i;
            }
        }
    }

    // We decided to reject all the people who were waiting after the park closes.
    total_people_rejected += ((waiting_list[599]) - (arrivals_list[599] - rejects_list[599]));
    total_people_rode = total_people_arrived - total_people_rejected;
    int average_waiting_time = waiting_time_sum / total_people_rode;

    // Printing all the statistics
    printf("\n-------------------------\n");
    printf("    End of day report\n");
    printf("-------------------------\n");
    printf("Total Number of Passenger Arrivals: %d\n", total_people_arrived);
    printf("Total Number of passengers that rode a particular ride: %d\n", total_people_rode);
    printf("Total Number of passengers who turned away because of wait: %d\n", total_people_rejected);
    printf("Average waiting time: %d minutes\n", average_waiting_time);
    time_check(max_people_waiting_per_iteration, time_buf);
    printf("Highest waiting line: %d passengers, occured at %s",
                    max_people_waiting_in_line, time_buf);
    printf("\n\n");

    // Sending all the statistics to ride_status.txt
    fprintf(ride_status_file, "\n-------------------------\n");
    fprintf(ride_status_file, "    End of day report\n");
    fprintf(ride_status_file, "-------------------------\n");
    fprintf(ride_status_file, "Total Number of Passenger arrivals: %d\n", total_people_arrived);
    fprintf(ride_status_file, "Total Number of Passengers that rode a particular ride: %d\n", total_people_rode);
    fprintf(ride_status_file, "Total Number of Passengers turned away because of wait: %d\n", total_people_rejected);
    fprintf(ride_status_file, "Average waiting time: %d minutes\n", average_waiting_time);
    fprintf(ride_status_file, "Highest waiting line: %d passengers, occured at %s",
                    max_people_waiting_in_line, time_buf);
    fprintf(ride_status_file, "\n\n");

    float rejection_list = 0.0;
    if(total_people_rejected > 0)
    {
        rejection_list = ((float)total_people_arrived) / ((float)total_people_rejected);
    }
    FILE *table_data_file = fopen("table_data.csv", "w");
    fprintf(table_data_file, "%d,%d,%d,%d,%f,%d,%d,%s\n", CARNUM, MAXPERCAR, total_people_arrived, total_people_rejected,
                    rejection_list, average_waiting_time, max_people_waiting_in_line, time_buf);

    fclose(plot_data_file);
    fclose(table_data_file);
}



// Generate a number of passengers arrival per minute.
int total_arrivals(int iteration)
{
    int avg_arrivals = 0;

    // We are given what the mean arrivals are for given time periods.
    // 0-119 25pp/min, 120-299 45pp/min, 300-419 35pp/min, 420-600 25pp/min
    if(iteration < 120) {
        avg_arrivals = 25;
    }
    else if(iteration < 300) {
        avg_arrivals = 45;
    }
    else if(iteration < 420) {
        avg_arrivals = 35;
    }
    else {
        avg_arrivals = 25;
    }
    return(poissonRandom(avg_arrivals));
}

// We ensure that there is only one car throught the tour
// We called this in thread creation.
void* loading_car(void *car_info_instance)
{
    int i;
    struct car_info *car = (struct car_info *) car_info_instance;

    for(i = 0; i < MAXWAITPEOPLE; ++i)
    {
        if(i < MAXWAITPEOPLE - car->single_car_load)
        {
            car->waiting_time_list[i] = car->waiting_time_list[i + car->single_car_load];
        }
        else
        {
            car->waiting_time_list[i] = -1;
        }
    }
    return 0;
}


int main(int argc, char *argv[])
{
   
    srand(time(NULL));
    CARNUM = atoi(argv[1]);
    MAXPERCAR = atoi(argv[2]);
    FILE *ride_status_file = fopen("ride_status.txt", "w");

    int total_steps = 600;

    // This will be used to create the plots
    int arrivals_list[total_steps];
    int rejects_list[total_steps];
    int waiting_list[total_steps];

    
    int number_of_arrivals = 0;
    int num_waiting = 0;
    int single_car_load = 0;
    int i, j, k;
    pthread_t tid;
    char time_buf[10];

    // So the idea behind this is if we calculate the waiting_time then it would be easier to find average waiting time.
    // And it worked out well
    int waiting_time_list[MAXWAITPEOPLE];
    for(j = 0; j < MAXWAITPEOPLE; ++j)
    {
        waiting_time_list[j] = -1;
    }
    int waiting_time_sum = 0;
    int num_to_add_to_waiting = -1;

    for(i = 0; i < total_steps; ++i)
    {
        // Making sure to see the people in the waiting line have waited
        for(j = 0; j < MAXWAITPEOPLE; ++j)
        {
            if(waiting_time_list[j] > -1)
            {
                waiting_time_list[j]++;
            }
        }

        number_of_arrivals = total_arrivals(i);

        // Adding people into waiting list
        num_waiting += number_of_arrivals;

        // If we added over the max, we have to reject them.
        int num_rejected = 0;
        if(num_waiting > MAXWAITPEOPLE)
        {
            num_rejected = num_waiting - MAXWAITPEOPLE;
            num_waiting = MAXWAITPEOPLE;
        }

        // Putting actual people for waiting list into waiting_time_list
        num_to_add_to_waiting = number_of_arrivals - num_rejected;
        for(j = 0; j < MAXWAITPEOPLE; ++j)
        {
            if(waiting_time_list[j] == -1)
            {
                waiting_time_list[j] = 0;
                num_to_add_to_waiting--;

                if(num_to_add_to_waiting == 0) break;
            }
        }

        arrivals_list[i] = number_of_arrivals;
        waiting_list[i] = num_waiting;
        rejects_list[i] = num_rejected;

        time_check(i, time_buf);
        fprintf(ride_status_file, "%d. Arrivals: %d, Rejected: %d Wait-Line: %d at %s\n", \
                        i, number_of_arrivals, num_rejected, num_waiting, time_buf);

        // This is to check and send the waiting people for their tours.
        for(j = 0; j < CARNUM; ++j)
        {
            if(num_waiting > 0)
            {
                if(num_waiting < MAXPERCAR) {
                    single_car_load = num_waiting;
                }
                    else {
                        single_car_load = MAXPERCAR;
                    }

                for(k = 0; k < single_car_load; ++k)
                {
                    if(k < single_car_load) {
                        waiting_time_sum += waiting_time_list[k];
                    }
                }

                // Here we were having troubles to get all the information, but we finally decided on getting a structure that has all the information.
                // So we load the passengers into the car according to their waiting lists and add them accordinglyl.
                struct car_info *car_info_instance = (struct car_info *)malloc(sizeof(struct car_info));
                car_info_instance->waiting_time_list = waiting_time_list;
                car_info_instance->single_car_load = single_car_load;

                // So we decided to have this to pass the car via thread.
                pthread_create(&tid, NULL, loading_car, (void *) car_info_instance);
                pthread_join(tid, NULL);

                num_waiting -= single_car_load;
            }

        }

    }

    output_figure_data(arrivals_list, rejects_list, waiting_list, waiting_time_sum, total_steps, ride_status_file);
    fclose(ride_status_file);
}
