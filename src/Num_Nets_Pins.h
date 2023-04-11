#include <iostream>
#include <fstream>
#include <string>

int NumNetPin() {
    std::ifstream file("case1_case2.txt");
    std::string line;
    int count = 1;
    bool found = false;
    std::string NumNets = "NumPins = ";
    std::string *data_array = new std::string[100]; // create a dynamic array to store the data
    int index = 0;

    while (getline(file, line)) {
        if (!found && line.find(NumNets) != std::string::npos) {
            found = true;
            count++;
        } else if (found) {
            data_array[index++] = line;
        }
    }
    file.close();

    std::cout << "Number of Nets" << " : " << count << std::endl;
    std::cout << "Number of pins in each Net " << " : " << std::endl;
    for (int i = 0; i < index; i++) {
        std::cout << data_array[i] << std::endl;
    }
    delete[] data_array; // free the memory allocated for the dynamic array


    return 0;
}
