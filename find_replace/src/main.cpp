#include "../include/gen_json.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <clocale>
#include <filesystem>
#include <chrono>
#include <ctime>

using namespace std;
chrono::system_clock::time_point session_start;
namespace fs = std::filesystem;

struct SessionStats {
    long long total_matches = 0;
    long long total_replacements = 0;
    long long total_commands = 0;
    vector<pair<string, long long>> replacements_per_file;
};

SessionStats stats;

void write_report() {
    ofstream fout("../docs/report.txt");

    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);

    auto duration = chrono::duration_cast<chrono::seconds>(now - session_start);
    long long hours = duration.count() / 3600;
    long long minutes = (duration.count() % 3600) / 60;
    long long seconds = duration.count() % 60;

    fout << "=== ОТЧЁТ ПО СЕССИИ ===\n";
    fout << "Дата и время отчёта: " << ctime(&now_time);
    fout << "Продолжительность сессии: " 
         << hours << "ч " << minutes << "м " << seconds << "с\n";
    fout << "Общее количество введённых команд: " << stats.total_commands << "\n";
    fout << "Общее количество найденных совпадений: " << stats.total_matches << "\n";
    fout << "Общее количество выполненных замен: " << stats.total_replacements << "\n";
    fout << "Количество замен по файлам:\n";
    for (auto &p : stats.replacements_per_file) {
        fout << "  " << p.first << ": " << p.second << "\n";
    }
    fout << "========================\n";

    fout.close();
}

void close_program() {
    write_report();
    for (const auto& entry : fs::directory_iterator("../data")) {
        if (entry.path().extension() == ".json") {
            fs::remove(entry.path());
        }
    }
    cout << "Все JSON-файлы удалены. Программа завершена." << endl;
    exit(0);
}

void find_in_file(const string& sub_to_found) {
    stats.total_commands++;
    int i = 1;
    bool founded = false;
    long long int count_find = 0;
    while (true) {
        string filename = "../data/data" + to_string(i) + ".json";
        string shortname = filename.substr(filename.find_last_of("/") + 1);
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
            if (sub.find(sub_to_found) != string::npos) {
                cout << shortname << "   " << name << "   " << sub << endl;
                count_find +=1;
                founded = true;
            }
        }
        i++;
    }
    stats.total_matches += count_find;
    cout << "Всего было найдено " << count_find << " совпадений." << endl;
    if (!founded){
        cout << "Подстрока " << sub_to_found << " не была найдена ни в одном файле." << endl;
    }
}

void replace_in_file(string name_json, string name_file, string substr, string substr_replaced){
    stats.total_commands++;

    ifstream fin("../data/" + name_json + ".json");
    if (!fin.is_open()) {
        cout << "Ошибка: файл не найден." << endl;
        return;
    }

    bool file_found = false;
    {
        string temp;
        while (getline(fin, temp)) {
            if (temp.find(name_file) != string::npos) {
                file_found = true;
                break;
            }
        }
        fin.clear();
        fin.seekg(0);
    }

    if (!file_found) {
        cout << "Ошибка: имя \"" << name_file << "\" не найдено в файле." << endl;
        return;
    }

    ofstream fout("../data/" + name_json + "edited.json");

    string line;
    size_t count_replace = 0;
    while (getline(fin, line)) {
        if (line.find(name_file) != string::npos && line.find("\"content\"") != string::npos) {
            while (line.find(substr) != string::npos) {
                size_t pos = line.find(substr);
                line.replace(pos, substr.size(), substr_replaced);
                count_replace++;
            }
        }
        fout << line << endl;
    }

    fin.close();
    fout.close();

    if (count_replace == 0) {
        remove(("../data/" + name_json + "edited.json").c_str());
        cout << "Ошибка: подстрока \"" << substr << "\" не найдена." << endl;
        return;
    }

    stats.replacements_per_file.push_back({name_json + "edited.json", count_replace});
    stats.total_replacements += count_replace;

    cout << "Файл " << name_json + "edited.json создан." << endl;
    cout << "Всего было совершено " << count_replace << " замен." << endl;
}


void replace_all(string name_json, string substr, string substr_replaced){
    stats.total_commands++;
    ifstream fin("../data/" + name_json + ".json");
    if (!fin.is_open()){
        cout << "Не удалось найти данный файл." << endl;
        return;
    }
    string line;
    ofstream fout("../data/" + name_json + "edited.json");

    size_t count_replace = 0;
    while (getline(fin, line)) {
        if (line.find("\"content\"") != string::npos && line.find(substr) != string::npos) {
            while (line.find(substr) != string::npos) {
                size_t pos = line.find(substr);
                line.replace(pos, substr.size(), substr_replaced);
                count_replace++;
            }
        }
        fout << line << endl;
    }
    fin.close();
    fout.close();
    if (count_replace == 0) {
        remove(("../data/" + name_json + "edited.json").c_str());
        cout << "Ошибка: подстрока \"" << substr << "\" не найдена." << endl;
        return;
    }
    stats.replacements_per_file.push_back({name_json + "edited.json", count_replace});
    stats.total_replacements += count_replace;
    cout << "Файл " << name_json + "edited.json создан." << endl;
    cout << "Всего было совершено " << count_replace << " замен." << endl;
}


int main(){
    setlocale(LC_ALL, "Russian");
    session_start = chrono::system_clock::now();
    string start_command;
    cout << "Для запуска программы с генерацией json файлов введите --start-generate, для запуска без генерации введите --start." << endl;

    while (start_command != "--start" && start_command != "--start-generate") {
        cout << "Введите команду (--start или --start-generate): ";
        cin >> start_command; 

        if (start_command == "--start-generate") {
            GenerateJSON(400000, 500000);
            cout << "Файлы json были успешно сгенерированы, введите необходимую команду, или \"--help\" для получения списка команд." << endl;
        } else if (start_command == "--start") {
            cout << "Запущен режим работы с уже имеющимися json файлами, введите необходимую команду, или \"--help\" для получения списка команд." << endl;
        } else {
            cout << "Ошибка. Неизвестная команда, доступны режимы --start или --start-generate." << endl;
        }
    }

    string command;

    while (true){
        cin >> command;
        if (command != "--dry-run"){
            if (command == "--help"){
                cout << "Список доступных команд: " << 
                "\nПолучение информации о доступных командах - \"--help\"" <<
                "\nПоиск слова в содержании файлов - \"--find\"" << 
                "\nЗамена содержимого во всех файлах - \"--replaceall\"" << 
                "\nЗамена содержимого только в одном файле - \"--replacein\"" << 
                "\nРежим \"ТОЛЬКО ПОИСК\" - \"--dry-run\"" << 
                "\nЗавершение работы и удаление json файлов - \"--exit\"" << 
                "\nЗавершение работы без удаления json файлов - \"--exitn\"" << endl;

            } else if (command == "--find"){
                cout << "Введите искомую подстроку: ";
                string sub_found;
                cin >> sub_found;
                cout << "Файл json  Файл с подстрокой  Содержание" << endl;
                find_in_file(sub_found);

            } else if (command == "--replaceall"){
                cout << "Для замены подстроки во всех файлах содержании введите по порядку: \nназвание json файла без расширения -> что заменить -> на что заменить." << endl;
                string name;
                string sub;
                string sub_r;
                cin >> name >> sub >> sub_r;
                replace_all(name, sub, sub_r);

            } else if (command == "--replacein"){
                cout << "Для замены подстроки в определенном файле содержании введите по порядку: \nназвание json файла без расширения -> название файла содержании -> что заменить -> на что заменить." << endl;
                string name_j;
                string name_f;
                string sub;
                string sub_r;
                cin >> name_j >> name_f >> sub >> sub_r;
                replace_in_file(name_j, name_f, sub, sub_r);
            } else if (command == "--exit"){
                close_program();
            } else if (command == "--exitn") {
                cout << "Программа завершена." << endl;
                write_report();
                exit(0); 
            } else {
                cout << "Введена неверная команда, попробуйте снова." << endl;
            }
        } else {
            while (command != "--dry-end") {
                cout << "Включен режим \"ТОЛЬКО ПОИСК\", чтобы посмотреть доступные команды введите --help." << endl;
                string in_dr_command;
                cin >> in_dr_command;
                if (in_dr_command == "--dry-end"){
                    cout << "Режим dry-run успешно завершен." << endl;
                    command = "--dry-end";   
                } else {
                    if (in_dr_command == "--help"){
                        cout << "Список доступных команд: " << 
                        "\nПолучение информации о доступных командах - \"--help\"" <<
                        "\nПоиск слова в содержании файлов - \"--find\"" << 
                        "\nЗавершить режим \"Только поиск\" - \"--dry-end\"" <<
                        "\nЗавершение работы и удаление json файлов - \"--exit\"" << 
                        "\nЗавершение работы без удаления json файлов - \"--exitn\"" << endl;
                    } else if (in_dr_command == "--find"){
                        cout << "\nВведите искомую подстроку: ";
                        string sub_found;
                        cin >> sub_found;
                        cout << "  Файл json  Файл с подстрокой  Содержание" << endl;
                        find_in_file(sub_found);
                    } else if (in_dr_command == "--exit"){
                        close_program();
                    } else if (in_dr_command == "--exitn") {
                        cout << "Программа завершена." << endl;
                        write_report();
                        exit(0); 
                    } else {
                        cout << "Введена неверная команда, попробуйте снова." << endl;
                    }
                }
            }
        }
    } 
    return 0;
}
