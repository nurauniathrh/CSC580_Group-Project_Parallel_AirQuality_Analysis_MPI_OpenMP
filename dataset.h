#ifndef DATASET_H
#define DATASET_H

//Air Quality Dataset - 10 hours x 5 parameters
//Columns: CO, NMHC, NOx, Temperature, Humidity
double air_quality[10][5]={
    {1.9, 145, 128, 19.6, 35.9},   //Hour 1
    {1.2, 88, 69, 15.1, 58.2},     //Hour 2
    {3.1, 342, 207, 13.8, 58.2},   //Hour 3
    {4.4, 776, 256, 18.4, 46.3},   //Hour 4
    {2.8, 386, 196, 26.7, 28.7},   //Hour 5
    {1.6, 144, 133, 12.6, 50.8},   //Hour 6
    {1.4, 88, 95, 9.3, 75.0},      //Hour 7
    {2.9, 468, 180, 19.4, 44.1},   //Hour 8
    {4.2, 589, 211, 22.8, 32.1},   //Hour 9
    {0.5, 33, 29, 16.3, 52.8},     //Hour 10
};

#endif