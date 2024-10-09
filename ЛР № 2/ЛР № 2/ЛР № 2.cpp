#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include <sstream>
#include <regex>
#include <map>
#include <cstdlib>
#include "encrypt.h"
#include "Base64.h"

using namespace std;

string key = "i1want2to3pass4cgi";

string create_encryption_data(const string& data) 
{
    string s = data;
    return encrypt_vigenere(s, key);
}

string create_encryption_Number(int num) 
{
    string s = to_string(num);
    return encrypt_vigenere(s, key);
}

string decryption_data(string s) 
{
    string newKey = extend_key(s, key);
    string number = decrypt_vigenere(s, newKey);
    return number;
}


string decURL(const string& str) 
{
    string decodeURL;
    for (int i = 0; i < str.length(); i++) 
    {
        if (str[i] == '%') 
        {
            if (i + 2 < str.length()) 
            {
                string value = str.substr(i + 1, 2);
                decodeURL += char(stoi(value, nullptr, 16));
                i += 2;
            }
        }
        else if (str[i] == '+') 
        {
            decodeURL += ' ';
        }
        else 
        {
            decodeURL += str[i];
        }
    }
    return decodeURL;
}

map<string, string> parsing(const string& request) 
{
    map<string, string> elements;
    istringstream strinput (request);
    string keyValue;

    while (getline(strinput, keyValue, '&'))
    {
        int equal = keyValue.find('=');
        if (equal != string::npos) 
        {
            string key = decURL(keyValue.substr(0, equal));
            string value = decURL(keyValue.substr(equal+1));
            elements[key] = value;
        }
    }
    return elements;
}

int main() {

    srand(time(NULL));

    char* request_method = getenv("REQUEST_METHOD");
    char* content_length = getenv("CONTENT_LENGTH");

    const int min = 1;
    const int max = 100;
    int guessedNumber = rand() % max + min;
    string hashedNumber = create_encryption_Number(guessedNumber);
    const int maxAttempts = ceil(log2(max - min + 1));
    int currentAttempts = maxAttempts;
    string hashedAttempts = (create_encryption_Number(currentAttempts));
    string prevAttempts = "";
    string message = "";
    bool cheat = false;
    bool victory = false;
    string control_sum;

    if (request_method && string(request_method) == "POST") 
    {
        if (content_length) 
        {
            int length = stoi(content_length);
            string text(length, ' ');
            cin.read(&text[0], length);
            map<string, string> elements = parsing(text);
            if (elements.count("hashedAttempts") && elements.count("hashedNumber") && elements.count("ControlSum") && elements.count("prevAttempts"))
            {
                control_sum = elements["ControlSum"];
                string NewHashedNumber = elements["hashedNumber"];
                string NewPrevAttempts = elements["prevAttempts"];
                string NewHashedAttempts = elements["hashedAttempts"];
                string NewControlSum = NewHashedAttempts + NewHashedNumber + NewPrevAttempts;
                if (create_encryption_data(NewControlSum) != control_sum) 
                {
                    message = "Жульничать плохо! Игра окончена!";
                    currentAttempts = 0;
                    prevAttempts = " ";
                    cheat = true;
                }
                else if (currentAttempts == 0)
                {
                    message = "Игра окончена! Начните заново!";
                    prevAttempts = " ";
                }
                else 
                {
                    hashedNumber = NewHashedNumber;
                    prevAttempts = NewPrevAttempts;
                    currentAttempts = stoi(decryption_data(NewHashedAttempts));
                }
            }

            if (elements.count("usernum") && cheat == false && currentAttempts > 0) 
            {
                string userNumberStr = elements["usernum"];
                int userNumber = stoi(userNumberStr);
                guessedNumber = stoi(decryption_data(hashedNumber));
                string hashedUserNumber = create_encryption_Number(userNumber);

                if (userNumber < min || userNumber > max) 
                {
                    message = "Выход за пределы!";
                }

                else 
                {
                    if (currentAttempts > 0)
                    {
                        --currentAttempts;
                        hashedAttempts = create_encryption_Number(currentAttempts);
                    }

                    if (hashedUserNumber == hashedNumber && currentAttempts >= 0)
                    {
                        victory = true;
                        prevAttempts +=  to_string(userNumber) + " ";
                        message = "Вы угадали! Было загадано число " + to_string(guessedNumber);

                    }

                    else if(userNumber < guessedNumber && currentAttempts > 0)
                    {
                        prevAttempts += to_string(userNumber) + ", ";
                        message = "Загаданное число больше! ";
                    }
                    else if (userNumber > guessedNumber && currentAttempts > 0) 
                    {
                        prevAttempts += to_string(userNumber) + ", ";
                        message = "Загаданное число меньше! ";
                    }
                    else if (currentAttempts == 0) 
                    {
                        message = "Игра окончена! Начните заново! Было загадано число " + to_string(guessedNumber);
                        prevAttempts += to_string(userNumber) + " ";
                    }

                }

            }

        }
    }

    control_sum = create_encryption_data(hashedAttempts + hashedNumber + prevAttempts);

    cout << "Content-type: text/html " << endl << endl;
    cout << "<!DOCTYPE html>" << endl;
    cout << "<html lang=\"ru\">" << endl;
    cout << "<head>" << endl;
    cout << "<meta charset=\"UTF-8\">" << endl;
    cout << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << endl;
    cout << "<link rel=\"icon\" href=\"./images/game-launcher-svgrepo-com.svg\">" << endl;
    cout << "<link rel=\"stylesheet\" href=\"./bootstrap/css/bootstrap.min.css\">" << endl;
    cout << "<link rel=\"stylesheet\" href=\"./css/style.css\">" << endl;
    cout << "<title>Угадай число</title>" << endl;
    cout << "</head>" << endl;
    cout << "<body>" << endl;
    cout << "<header class=\"bg-dark text-center p-4\">Guess the Number</header>" << endl;
    cout << "<section id=\"game\" class=\"container mt-5\">" << endl;
    cout << "<div class=\"g row align-items-center justify-content-center\">" << endl;
    cout << "<div class=\"col-md-10\">" << endl;
    cout << "<h2>Игра «Угадай число»</h2>" << endl;
    cout << "<p class=\"rules\" id=\"rule__one\">" << endl;
    cout << "Компьютер загадывает число в диапазоне [" << min << ";" << max << "]." << endl;
    cout << "</p>" << endl;
    cout << "<p class=\"rules\" id=\"rule__two\">" << endl;
    cout << "У пользователя есть " << maxAttempts << "<span id = \"count\"></span> попыток угадать это число." << endl;
    cout << "</p>" << endl;
    cout << "<form id=\"guessForm\" class=\"d-flex justify-content-between align-items-start\">" << endl;
    cout << "<div class=\"d-flex form-group\">" << endl;
    cout << "<input type=\"hidden\" name=\"hashedNumber\" value=\"" << hashedNumber << "\">";
    cout << "<input type=\"hidden\" name=\"hashedAttempts\" value=\"" << hashedAttempts << "\">";
    cout << "<input type=\"hidden\" name=\"prevAttempts\" value=\"" << prevAttempts << "\">";
    cout << "<input type=\"hidden\" name=\"ControlSum\" value=\"" << control_sum << "\">";
    cout << "<input type=\"number\" name=\"usernum\" class=\"form-control\" id=\"guess\" placeholder=\"Введите число\">" << endl;
    cout << "</div>" << endl;
    cout << "<div class=\"form-btn d-flex justify-content-center align-items-center\">" << endl;
    cout << "<button type=\"submit\" formmethod=\"POST\" action=\"index.cgi\" id=\"submit\" class=\"btn btn__ btn-primary px-4\">" << endl;
    cout << "Ввод" << endl;
    cout << "</button>" << endl;
    cout << "<button type=\"submit\" formmethod=\"GET\" action=\"index.cgi\" id=\"reset\" class=\"btn btn__ btn-warning px-4\">" << endl;
    cout << "Заново" << endl;
    cout << "</button>" << endl;
    cout << "</div>" << endl;
    cout << "</form>" << endl;
    cout << "<div class=\"container other d-flex flex-column align-items-start mt-2\">" << endl;
    cout << "<p class=\"answer\" id=\"answer\"> " << message << "</p>" << endl;
    cout << "<p class=\"prev__numbers\" id=\"prev\">Введённые ранее числа: " << prevAttempts << "</p>" << endl;
    message = (currentAttempts > 0) ? ("У вас осталось " + to_string(currentAttempts) + " попыток.") : "Попыток больше нет.";
    cout << "<div style=\"text-align: center;\"><p style=\"font-weight: bold;\">" << message << "</p></div>" << endl;;
    cout << "</div>" << endl;
    cout << "</div>" << endl;
    cout << "</div>" << endl;
    cout << "</div>" << endl;
    cout << "</div>" << endl;
    cout << "</section>" << endl;
    cout << "<script>" << endl;
    cout << "document.addEventListener ('DOMContentLoaded', function() {" << endl;
    cout << "const number = document.getElementById('guess');" << endl;
    cout << "const submit = document.getElementById('submit');" << endl;
    cout << "const reset = document.getElementById('reset');" << endl;
    cout << "number.focus();" << endl;
    if (currentAttempts == 0 || victory) {
        cout << "number.disabled = true;" << endl;
        cout << "submit.disabled = true;" << endl;
        cout << "reset.focus();" << endl;
    }
    cout << "});" << endl;
    cout << "</script>" << endl;
    cout << "<script src=\"./bootstrap/js/bootstrap.min.js\"></script>" << endl;
    cout << "</body>" << endl;
    cout << "</html>" << endl;

    return 0;
}
