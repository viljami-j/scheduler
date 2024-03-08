#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include <io.h>
#include <fstream>
#include <map>

// todo: lis‰‰ kellonaikojen automaattinen j‰rjest‰minen
// todo: siivoa koodi

using namespace std;

static vector<string> csv_header_week = { " ", "Maanantai", "Tiistai", "Keskiviikko", "Torstai", "Perjantai", "Lauantai", "Sunnuntai" };

static class DayPlan {

};

static class WeeklyPlan {
    
};

class str_input_prompt {
private:
    /* Constants *******/
    std::vector<std::string> alphabet = { // Used for user-options
                    "a", "b", "c", "d", "e", "f", "g",
                    "h", "i", "j", "k", "l", "m", "n",
                    "o", "p", "q", "r", "s", "t", "u",
                    "v", "w", "x", "y", "z"
    };
    /****************/


    /* State *******/
    int input_user_option_choice = -1; // -1 if unused
    /****************/

    string invalid_input_message;
    vector<string> valid_input = {}; // allowed input
    string message;

    bool is_invalid_input = false;

    vector<string> options; // multi-choice options. these are meant to be created from user-input, to save them from typing the same thing. todo: rename into 'user_options', and make 'options' a separate hardcoded feature

    bool is_user_option(string input) {
        for (size_t i = 0; i < options.size(); i++)
        {
            bool input_was_user_option = (input == alphabet[i]);
            if (input_was_user_option) {
                input_user_option_choice = i;
                return true;
            }
        }
        return false;
    }

    void add_options(vector<string> options) {
        for (size_t i = 0; i < options.size(); i++)
        {
            for (size_t j = 0; j < this->options.size(); j++) // does option already exist?
            {
                if (options[i] == this->options[j])
                    continue; // skip iteration
            }

            this->options.push_back(options[i]);
        }
    }
public:
    string input = ""; // Contains input after 'prompt()' is called
    string value = ""; // Contains a value, if an user-option was picked. Wiped at beginning of each prompt. to-do: rename to prefill

    bool is_user_prefill() {
        return input_user_option_choice != -1;
    }

    str_input_prompt(string message, vector<string> valid_input=vector<string>(), string invalid_input_message="Virheellinen valinta - valitse uudelleen.") 
        : message(message), invalid_input_message(invalid_input_message) {
        this->valid_input = valid_input;
    }

    bool is_valid_input() {
        if (valid_input.size() == 0)
            return true;
        for (size_t i = 0; i < valid_input.size(); i++)
        {
            if (input == valid_input[i]) {
                system("CLS");
                return true;
            }
        }
        
        return false;
    }

    void clear_options() {
        this->options.clear();
    }

    string prompt(bool clear_terminal = true, bool reset_input=true, bool save_to_prefills=false) { // todo: migrate to binary flags
        do
        {
            input_user_option_choice = -1;
            if (reset_input)
                input = "";

            if (clear_terminal)
                system("CLS");
            if (is_invalid_input) {
                wcout << (invalid_input_message + "\n\n").c_str();
            }
            if (message != "")
                wcout << (message + "\n").c_str();
            if (options.size() != 0) {
                
                wcout << "\n\n";

                try
                {
                    for (size_t i = 0; i < options.size(); i++)
                    {
                        wcout << (alphabet[i] + ". " + options[i]).c_str() << std::endl;
                        if (valid_input.size() != 0) // if there are validators, add options to them
                            valid_input.push_back(alphabet[i]); // pushing back duplicates won't matter
                    }
                }
                catch (const std::exception&)
                {
                    cerr << ("options have a hard-limit of " + alphabet.size());
                }
            }

            wcout << "Valinta: ";
            cin >> input;

            // Flush cin
            cin.clear();
        } while (!is_valid_input());
        
        is_invalid_input = false; // reset trigger

        string input_store = input;
            

        if (save_to_prefills && !is_user_option(input_store))
            add_options({ input });
        if (is_user_option(input_store)) 
            value = options[input_user_option_choice];
        /*if (reset_input)
            input = "";*/

        return input_store;
    }
};

void print(string message, bool pause=true, bool newline = true) { // todo: migrate to binary flags
    string endline = newline ? "\n" : "";
    wcout << (message + endline).c_str();
    system("pause");
}


namespace file {
    string read(string filename) {
        std::ifstream f(filename);
        string data = "";

        if (f.is_open()) {
            std::string line;
            while (std::getline(f, line)) {
                data.append(line + "\n");
            }
            f.close();
            return data;
        }
        else {
            std::cerr << "Failed to read!" << std::endl;
        }
    }

    void write(string filename, string payload) {
        std::ofstream f(filename);

        if (f.is_open()) {
            f << payload;
            f.close();
        }
        else {
            std::cerr << "Failed to write!" << std::endl;
        }
    }
}

string to_comma_separated_string(vector<string> strings) {
    string str = "";

    for (size_t i = 0; i < strings.size(); i++)
    {
        str += strings[i] + ",";
    }
    str.pop_back(); // remove extra comma

    return str;
}



/* Prompts *************************/
auto p_end_session_prompt = str_input_prompt("Valmista tuli!\n\nLopeta sessio?\n1. Kyll‰\n2. Ei\n", { "1", "2" });
auto p_plan_type = str_input_prompt("Mit‰ suunnitellaan?:\n1. Suunnittele huominen\n2. Suunnittele viikko\n", { "1", "2" });

// Week plan prompts
auto p_weekly_work_hours = str_input_prompt("Monta tuntia aiot k‰ytt‰‰ tˆihin? (Oletusarvo: 37.5 tuntia)"); // unimplemented
auto p_subject = str_input_prompt("Aihe? (vapaamuotoinen)");
auto p_day = str_input_prompt("Viikonp‰iv‰? (ma, ti, ke, to, pe, la, su)", { "ma", "ti", "ke", "to", "pe", "la", "su" }, "Virheellinen valinta - jos haluat valita esimerkiksi maanantain, kirjoita 'ma'");
auto p_time = str_input_prompt("Kellonaika? (vapaamuotoinen)");
auto p_new_entry = str_input_prompt("Uusi lis‰ys?\n1. Kyll‰\n2. Ei", { "1", "2" });


/***********************************/

// File names
string output_day_plan = "p‰iv‰.csv";
string output_week_plan = "viikkosuunnitelma.csv";
//

typedef map<string, map<string, string>> timetable;
typedef map<string, string> dict;

vector<string> keys = { "ma", "ti", "ke", "to", "pe", "la", "su"};

timetable populate_timetable() {
    // key-value: day-row
    // key-value: time-subject
    timetable tt;


    for (size_t i = 0; i < keys.size(); i++)
    {
        tt[keys[i]] = dict();
        tt[keys[i]][""];
    }

    return tt;
}

template <typename KeyType, typename ValueType>
std::vector<KeyType> getKeys(const std::map<KeyType, ValueType>& inputMap) {
    std::vector<KeyType> keys;
    for (const auto& pair : inputMap) {
        keys.push_back(pair.first);
    }
    return keys;
}

int main()
{
    string csv_output = "";
    int retval = _setmode(_fileno(stdout), _O_U16TEXT); // umlauts

    while (true) {
        p_plan_type.prompt(true, false);

        if (p_plan_type.input == "1") { // Day planning
            print("WIP -> Keskener‰inen toiminto!");
        }
        else if (p_plan_type.input == "2") { // Week planning

            // Add commas
            csv_output += to_comma_separated_string(csv_header_week) + "\n";

            timetable output = populate_timetable();
            dict unique_times;

            while (true) {
                p_subject.prompt(true,true,true);     // mit‰
                p_day.prompt();                      // milloin
                p_time.prompt(true, true, true);    // monelta
                p_new_entry.prompt(true, false);

                // final CSV will be formed from output
                auto subject = p_subject.is_user_prefill() ? p_subject.value : p_subject.input;
                auto time    = p_time.is_user_prefill() ? p_time.value : p_time.input;

                output[p_day.input][time] = subject;

                unique_times[time] = ""; // data here doesnt matter, we are interested in keys


                if (p_new_entry.input == "2")
                    break;
            }

            // Populate csv_output
            vector<string> sample_row = csv_header_week;

            int row_count = unique_times.size();
            int col_count = 8; // col count -> 8 (blank + week days)

            // temp bodge
            vector<string> custom_keys;
            custom_keys.push_back(" ");
            for (size_t i = 0; i < keys.size(); i++)
                custom_keys.push_back(keys[i]);
            // temp bodge end

            auto time_keys = getKeys(unique_times);

            for (size_t i = 0; i < row_count; i++) // rows
            {
                for (size_t j = 0; j < col_count; j++) // cols
                {
                    //vector<string> times = getKeys(output[csv_header_week[j + 1]]);
                    auto time = time_keys[i]; // Get row time

                    if (j == 0) 
                        csv_output += time + ","; // Get row time
                    else 
                    { // day info
                        csv_output += output[custom_keys[j]][time] + ",";
                    }
                }
                csv_output.pop_back(); // pop extra comma 
                csv_output += "\n";
            }
        }

        p_end_session_prompt.prompt(true, false);

        if (p_end_session_prompt.input == "1") {
            system("CLS");
            file::write(output_week_plan, csv_output);
            print("Viikkoaikataulu tallennettu tiedostoon '" + output_week_plan + "'");
            system("pause");
            break;
        }
    }
}


