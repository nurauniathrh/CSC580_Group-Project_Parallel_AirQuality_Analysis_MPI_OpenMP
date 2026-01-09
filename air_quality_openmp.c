#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define ROWS 10
#define COLS 5

double air_quality[ROWS][COLS] = {
    {1.9, 145, 128, 19.6, 35.9},    // Hour 1
    {1.2,  88,  69, 15.1, 58.2},    // Hour 2
    {3.1, 342, 207, 13.8, 58.2},    // Hour 3
    {4.4, 776, 256, 18.4, 46.3},    // Hour 4
    {2.8, 386, 196, 26.7, 28.7},    // Hour 5
    {1.6, 144, 133, 12.6, 50.8},    // Hour 6
    {1.4,  88,  95,  9.3, 75.0},    // Hour 7
    {2.9, 468, 180, 19.4, 44.1},    // Hour 8
    {4.2, 589, 211, 22.8, 32.1},    // Hour 9
    {0.5,  33,  29, 16.3, 52.8}     // Hour 10
};

void run_analysis(int num_threads) {
    omp_set_num_threads(num_threads);
    
    double min_score = 1e9;
    double max_score = -1e9;
    int min_index = -1, max_index = -1;
    
    double start_time = omp_get_wtime();
    
    #pragma omp parallel
    {
        double local_min_score = 1e9;
        double local_max_score = -1e9;
        int local_min_index = -1;
        int local_max_index = -1;
        
        #pragma omp for
        for (int i = 0; i < ROWS; i++) {
            double co   = air_quality[i][0];
            double nmhc = air_quality[i][1];
            double nox  = air_quality[i][2];
            double temp = air_quality[i][3];
            double hum  = air_quality[i][4];
            
            double score = 3.0 * co + 
                           2.0 * nmhc + 
                           2.0 * nox + 
                           0.5 * fabs(temp - 22.0) + 
                           0.3 * fabs(hum - 50.0);
            
            if (score < local_min_score) {
                local_min_score = score;
                local_min_index = i;
            }
            if (score > local_max_score) {
                local_max_score = score;
                local_max_index = i;
            }
        }
        
        #pragma omp critical
        {
            if (local_min_score < min_score) {
                min_score = local_min_score;
                min_index = local_min_index;
            }
            if (local_max_score > max_score) {
                max_score = local_max_score;
                max_index = local_max_index;
            }
        }
    }
    
    double end_time = omp_get_wtime();
    double elapsed_time = end_time - start_time;
    
    // OUTPUT: Shows "Hour" instead of "Index"
    printf("\n========== OPENMP AIR QUALITY ANALYSIS ==========\n");
    printf("Scoring Formula: 3*CO + 2*NMHC + 2*NOx + 0.5*|temp-22| + 0.3*|hum-50|\n");
    printf("Number of Threads: %d\n", num_threads);
    printf("Execution Time: %.6f seconds\n", elapsed_time);
    printf("Total Records Processed: %d hours\n", ROWS);
    
    printf("\nBEST AIR QUALITY (LOWEST SCORE):\n");
    printf("   Hour: %d, Score: %.2f\n", min_index + 1, min_score);
    printf("   CO: %.1f mg/m3, NMHC: %.0f ug/m3, NOx: %.0f ppb, Temp: %.1f C, Humidity: %.1f%%\n", 
           air_quality[min_index][0], air_quality[min_index][1], air_quality[min_index][2],
           air_quality[min_index][3], air_quality[min_index][4]);
    
    printf("\nWORST AIR QUALITY (HIGHEST SCORE):\n");
    printf("   Hour: %d, Score: %.2f\n", max_index + 1, max_score);
    printf("   CO: %.1f mg/m3, NMHC: %.0f ug/m3, NOx: %.0f ppb, Temp: %.1f C, Humidity: %.1f%%\n", 
           air_quality[max_index][0], air_quality[max_index][1], air_quality[max_index][2],
           air_quality[max_index][3], air_quality[max_index][4]);
    
    printf("\n=================================================\n");
}

int main() {
    printf("OPENMP PERFORMANCE TEST - 3 THREAD COUNTS\n");
    printf("=========================================\n");
    
    // Test 1: 1 thread
    printf("\n\nTEST 1: 1 THREAD (SEQUENTIAL EQUIVALENT)\n");
    printf("-----------------------------------------\n");
    run_analysis(1);
    
    // Test 2: 2 threads
    printf("\n\nTEST 2: 2 THREADS\n");
    printf("-----------------------------------------\n");
    run_analysis(2);
    
    // Test 3: 4 threads
    printf("\n\nTEST 3: 4 THREADS\n");
    printf("-----------------------------------------\n");
    run_analysis(4);
    
    printf("\n\n=========================================\n");

    return 0;
}