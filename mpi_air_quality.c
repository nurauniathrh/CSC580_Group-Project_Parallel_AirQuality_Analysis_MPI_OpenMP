#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "dataset.h"

//Function to calculate air quality score
//Lower score = Better air quality
double calculate_score(double co, double nmhc, double nox, double temp, double humidity){
    double score = 0;

    //Pollution weights (higher = worse)
    score += 3.0 * co;   //co is most harmful
    score += 2.0 * nmhc; // NMHC weight
    score += 2.0 * nox;  // NOx weight

    //Temperature penalty (ideal = 22°C)
    score += 0.5 * fabs(temp - 22.0);

    //Humidity penalty (ideal = 50%)
    score += 0.3 * fabs(humidity - 50.0);

    return score;
}

int main (int argc, char** argv){
    //Initialize MPI
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Start timing
    double start_time = MPI_Wtime();

    int total_hours = 10;
    int hours_per_process = total_hours / size;
    int remainder = total_hours % size;

    //Calculate start and end for this process
    int start_hour, end_hour;
    if (rank<remainder){
        start_hour = rank * (hours_per_process + 1);
        end_hour = start_hour + hours_per_process + 1;
    }else{
        start_hour = rank * hours_per_process + remainder;
        end_hour = start_hour + hours_per_process;
    }

    //Each process finds local best and worst
    double local_best_score = 1e9;  //Very large number
    double local_worst_score = -1e9; //very small number
    int local_best_hour = -1;
    int local_worst_hour = -1;

    //Process assigned hours
    for (int hour = start_hour; hour<end_hour; hour++){
        double score = calculate_score(
            air_quality[hour][0],  //CO
            air_quality[hour][1],  //NMHC
            air_quality[hour][2],  //NOx
            air_quality[hour][3],  //Temperature
            air_quality[hour][4]  //Humidity
        );

        if(score<local_best_score){
            local_best_score = score;
            local_best_hour = hour;
        }

        if(score>local_worst_score){
            local_worst_score = score;
            local_worst_hour = hour;
        }
    }

    //Find global best (minimum score)
    struct{
        double value;
        int index;
    }local_best, global_best;

    local_best.value = local_best_score;
    local_best.index = local_best_hour;

    MPI_Allreduce(&local_best, &global_best, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

    //Find global worst (maximum score)
    struct{
        double value;
        int index;
    }local_worst, global_worst;

    local_worst.value = local_worst_score;
    local_worst.index = local_worst_hour;

    MPI_Allreduce(&local_worst, &global_worst, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);

    //End timing
    double end_time = MPI_Wtime();

    //Display results from root process
    if (rank == 0){
        printf("\n===== AIR QUALITY ANALYSIS RESULTS =====\n");
        printf("Parallel MPI Implementation\n");
        printf("Number of processes: %d\n\n", size);
        printf("BEST Air Quality (Lowest Score):\n");
        printf(" Hour: %d\n", global_best.index + 1);
        printf(" Score: %.2f\n", global_best.value);
        printf(" CO: %.1f mg per cubic meter\n", air_quality[global_best.index][0]);
        printf(" NMHC: %.0f micrograms per cubic meter\n", air_quality[global_best.index][1]);
        printf(" NOx: %.0f ppb\n", air_quality[global_best.index][2]);
        printf(" Temperature: %.1f degrees Celsius\n", air_quality[global_best.index][3]);
        printf(" Humidity: %.1f%%\n\n", air_quality[global_best.index][4]);

        printf("WORST Air Quality (Highest Score):\n");
        printf(" Hour: %d\n", global_worst.index + 1);
        printf(" Score: %.2f\n", global_worst.value);
        printf(" CO: %.1f mg per cubic meter\n", air_quality[global_worst.index][0]);
        printf(" NMHC: %.0f micrograms per cubic meter\n", air_quality[global_worst.index][1]);
        printf(" NOx: %.0f ppb\n", air_quality[global_worst.index][2]);
        printf(" Temperature: %.1f degrees Celsius\n", air_quality[global_worst.index][3]);
        printf(" Humidity: %.1f%%\n\n", air_quality[global_worst.index][4]);

        printf("\n========================================\n");
        printf("Total execution time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
