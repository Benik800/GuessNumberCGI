#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include <sstream>
#include <map>
#include <cstdlib>
#include "encrypt.h"
#include "Base64.h"

using namespace std;

string keyWord = "i1want2to3pass4cgi";

string create_encryption_data(const string& data) 
{
    string s = data;
    return encrypt_vigenere(s, keyWord);
}

string create_encryption_Number(int num) 
{
    string s = to_string(num);
    return encrypt_vigenere(s, keyWord);
}

string decryption_data(string s) 
{
    string newKeyWord = extend_key(s, keyWord);
    string number = decrypt_vigenere(s, newKeyWord);
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
                decodeURL += char(stoi(value, NULL, 16));
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

string textAttempts(int Attempts) 
{
    if (Attempts == 1) 
    {
        return "У вас осталась 1 попытка.";
    }
    else if(Attempts == 0)
    {
        return "У вас не осталось попыток! ";
    }
    else if (Attempts >= 2 && Attempts <= 4) 
    {
        return "У вас осталось " + to_string(Attempts) + " попытки.";
    }
    else 
    {
        return "У вас осталось " + to_string(Attempts) + " попыток.";
    }

}

int main() {

    srand(time(NULL));

    char* request_method = getenv("REQUEST_METHOD");
    char* content_length = getenv("CONTENT_LENGTH");

    const int min = 1;
    const int max = 100;

    int guessedNumber = rand() % max + min;
    string encryptionNumber = create_encryption_Number(guessedNumber);

    const int maxAttempts = ceil(log2(max - min + 1));
    int currentAttempts = maxAttempts;
    string encryptionAttempts = (create_encryption_Number(currentAttempts));
    string prevAttempts = "";

    string control_sum;
    string message = "";

    bool cheat = false;
    bool victory = false;


    if (request_method && string(request_method) == "POST")
    {
        if (content_length)
        {
            int length = stoi(content_length);
            string text(length, ' ');
            cin.read(&text[0], length);
            map<string, string> elements = parsing(text);
            if (elements.count("encryptionAttempts") && elements.count("encryptionNumber") && elements.count("ControlSum") && elements.count("prevAttempts"))
            {
                control_sum = elements["ControlSum"];
                string NewHashedNumber = elements["encryptionNumber"];
                string NewPrevAttempts = elements["prevAttempts"];
                string NewHashedAttempts = elements["encryptionAttempts"];
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
                    encryptionNumber = NewHashedNumber;
                    prevAttempts = NewPrevAttempts;
                    currentAttempts = stoi(decryption_data(NewHashedAttempts));
                }
            }

            if (elements.count("usernum") && cheat == false && currentAttempts > 0)
            {
                string userNumberStr = elements["usernum"];
                int userNumber = stoi(userNumberStr);
                guessedNumber = stoi(decryption_data(encryptionNumber));
                string hashedUserNumber = create_encryption_Number(userNumber);

                if (userNumber < min || userNumber > max)
                {
                    message = "Выход за пределы!";
                    encryptionAttempts = create_encryption_Number(currentAttempts);
                }

                else
                {
                    if (currentAttempts > 0)
                    {
                        --currentAttempts;
                        encryptionAttempts = create_encryption_Number(currentAttempts);
                    }

                    if (hashedUserNumber == encryptionNumber && currentAttempts >= 0)
                    {
                        victory = true;
                        prevAttempts += to_string(userNumber) + " ";
                        message = "Вы угадали! Было загадано число " + to_string(guessedNumber);

                    }

                    else if (userNumber < guessedNumber && currentAttempts > 0)
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

    control_sum = create_encryption_data(encryptionAttempts + encryptionNumber + prevAttempts);

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
    cout << "<input type=\"hidden\" name=\"encryptionNumber\" value=\"" << encryptionNumber << "\">";
    cout << "<input type=\"hidden\" name=\"encryptionAttempts\" value=\"" << encryptionAttempts << "\">";
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
    message = textAttempts(currentAttempts);
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
    if (currentAttempts == 0 || victory || cheat) 
    {
        cout << "number.disabled = true;" << endl;
        cout << "submit.disabled = true;" << endl;
        cout << "reset.focus();" << endl;
    }

    if ((currentAttempts == 0 || cheat == true) && !victory)
    {
        cout << "number.style.backgroundColor = '#fb6262'" << endl;
        cout << "document.querySelector('header').style.color = 'red';" << endl;
        cout << "document.querySelector('header').style.textShadow = '0 0 5px #75b9f0, 0 0 10px #1100ff, 0 0 15px #4400ff, 0 0 20px #506fa0, 0 0 35px #2684b7, 0 0 40px #7a054b, 0 0 50px #376fe7, 0 0 75px #4a5084';" << endl;
    }
    else if(victory)
    {
        cout << "number.style.backgroundColor = '#f3de2c'" << endl;
        cout << "document.querySelector('header').style.color = 'green';" << endl;
        cout << "document.querySelector('header').style.textShadow = '0 0 5px #75b9f0, 0 0 10px #1100ff, 0 0 15px #4400ff, 0 0 20px #506fa0, 0 0 35px #2684b7, 0 0 40px #7a054b, 0 0 50px #376fe7, 0 0 75px #4a5084';" << endl;

    }
    cout << "});" << endl;
    cout << "</script>" << endl;
    cout << "<script src=\"./bootstrap/js/bootstrap.min.js\"></script>" << endl;
    cout << "</body>" << endl;
    cout << "</html>" << endl;

    return 0;
}
