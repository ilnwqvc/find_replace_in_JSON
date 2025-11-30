#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>

using namespace std;

//генератор количества файлов
int CountFiles(int min_range, int max_range){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> cnt_files(min_range, max_range);
    return cnt_files(gen);
}

int main() {

    

    return 0;
}