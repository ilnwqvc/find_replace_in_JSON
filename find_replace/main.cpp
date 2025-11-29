#include "GenJson/gen_json.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;

string find_in_file(string founder){
    while (true){
        int i = 1;
        string line;
        string filename = "GenJson/data"+to_string(i)+".json";
        ifstream fin(filename);
        while(getline(fin, line)){
        size_t pos_start_substr = (line.find("\"content\":\""))+10;
        size_t pos_end_substr = (line.find("\"}"));
        size_t len_subsrt = (pos_end_substr-pos_start_substr-1);
        size_t pos_start_name = (line.find("\"name\":\""))
        string sub = line.substr(pos_start_substr, len_subsrt);
        if (sub.find(founder)){
            cout << filename << "   " << 
        }
        }

    }
}

int main(){
    GenerateJSON(400000, 500000); // генерация джейсон файлов перед запуском, после работы они удаляются и при новом запуске генерируются новые


}