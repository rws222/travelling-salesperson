#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main()
{
    // set the random seed
    srand(time(NULL));
    
    // specify the number of coordinates to generate
    int num_coords = 80;
    
    // open the output file
    ofstream outfile("coordinates.txt");
    
    // generate and save the coordinates
    for (int i = 0; i < num_coords; i++) {
        int x = rand() % 500;
        int y = rand() % 500;
        outfile << x << " " << y << endl;
    }
    
    // close the output file
    outfile.close();
    
    cout << "Coordinates saved to coordinates.txt." << endl;
    
    return 0;
}