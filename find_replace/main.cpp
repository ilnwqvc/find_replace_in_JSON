#include "gen_json.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;

void find_in_file(const string& founder){
    while (true){
        int i = 1;
        string line;
        string filename = "GenJson/data"+to_string(i)+".json";
        ifstream fin(filename);
        while(getline(fin, line)){
            size_t pos_start_substr = (line.find("\"content\":\""))+10;
            size_t pos_end_substr = (line.find("\"}"));
            size_t len_subsrt = (pos_end_substr-pos_start_substr-1);
            size_t pos_start_name = (line.find("\"name\":\""))+8;
            size_t pos_end_name = (line.find("\",\""));
            size_t len_name = (pos_start_name-pos_end_name);
            string name = line.substr(pos_start_name, len_name);
            string sub = line.substr(pos_start_substr, len_subsrt);
            if (sub.find(founder)){
                cout << filename << "   " << name << sub << endl;
            }
        }
    }
}

int main(){
    GenerateJSON(400000, 500000); // генерация джейсон файлов перед запуском, после работы они удаляются и при новом запуске генерируются новые
    string need_found;
    cout << "Введите искомую подстроку";
    find_in_file(need_found);
    return 0;
}