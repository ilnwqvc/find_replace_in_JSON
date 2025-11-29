#include "GenJson/gen_json.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;

int main(){
    GenerateJSON(400000, 500000); // генерация джейсон файлов перед запуском, после работы они удаляются и при новом запуске генерируются новые
    

}