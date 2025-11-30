#include "GenJson/gen_json.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <clocale>

using namespace std;

void find_in_file(const string& founder) {
    int i = 1;
    bool founded = false;
    while (true) {
        string filename = "data" + to_string(i) + ".json";
        ifstream fin(filename);
        if (!fin.is_open()) break;
        string line;
        while (getline(fin, line)) {
            size_t pos_start_name = line.find("\"name\":\"");
            if (pos_start_name == string::npos) continue;
            pos_start_name += 8;
            size_t pos_end_name = line.find("\",\"", pos_start_name);
            if (pos_end_name == string::npos) continue;
            string name = line.substr(pos_start_name, pos_end_name - pos_start_name);
            size_t pos_start_substr = line.find("\"content\":\"");
            if (pos_start_substr == string::npos) continue;
            pos_start_substr += 11;
            size_t pos_end_substr = line.find("\"", pos_start_substr);
            if (pos_end_substr == string::npos) continue;
            string sub = line.substr(pos_start_substr, pos_end_substr - pos_start_substr);
            if (sub.find(founder) != string::npos) {
                cout << filename << "   " << name << "   " << sub << endl;
                founded = true;
            }
        }

        i++;
    }
    if (founded == false){
        cout << "Подстрока " << founder << " не была найдена ни в одном файле";
    }
}

int main(){
    setlocale(LC_ALL, "Russian");
    GenerateJSON(400000, 500000); // генерация джейсон файлов перед запуском, после работы они удаляются и при новом запуске генерируются новые
    string need_found;
    cout << "Введите"<< endl;
    cin >> need_found;
    find_in_file(need_found); 
    return 0;
}