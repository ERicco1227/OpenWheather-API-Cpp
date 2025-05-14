//Author: Dean Ricco
//Last Updated: 5/14/25
// Standard libraries
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <curl/curl.h>  // For HTTP requests using libcurl
#include <nlohmann/json.hpp>  // For parsing JSON responses

using json = nlohmann::json;
using namespace std;

const string API_KEY = "02089b5310f9858663f4c29b350c33bb";  //OpenWeather API key
const string BASE_URL = "https://api.openweathermap.org/data/2.5/weather?q="; //OpenWeather URL

// List to store up to 3 favorite cities
vector<string> favoriteCities;

// Callback function for libcurl to write data received from HTTP request into a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) 
{
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Fetches weather data for a given city from the OpenWeather API
string fetchWeatherData(const string& city) 
{
    CURL* curl = curl_easy_init();
    string readBuffer;

    if (curl) 
    {
        // Build full URL with escaped city name, API key, and units in metric
        string url = BASE_URL + curl_easy_escape(curl, city.c_str(), 0) + "&appid=" + API_KEY + "&units=metric";

        // Set curl options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform HTTP GET request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) 
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }

        // Cleanup curl
        curl_easy_cleanup(curl);
    }

    return readBuffer;
}

// Parses and displays weather information for a given city
void displayWeather(const string& city) 
{
    string response = fetchWeatherData(city);

    try 
    {
        json data = json::parse(response);
        if (data.contains("main")) 
        {
            cout << "Weather in " << data["name"] << ":\n";
            cout << "  Temperature: " << data["main"]["temp"] << "°C\n";
            cout << "  Humidity: " << data["main"]["humidity"] << "%\n";
            cout << "  Condition: " << data["weather"][0]["description"] << "\n";
        }
        else 
        {
            cout << "City not found\n";
        }
    }
    catch (...) 
    {
        // Handles unexpected responses
        cout << "Failed to parse weather data.\n";
    }
}

// Adds a city to the user's list of favorite cities
void addFavoriteCity(const string& city) 
{
    if (favoriteCities.size() >= 3) 
    {
        cout << "You can only have 3 favorite cities.\n";
        return;
    }

    if (find(favoriteCities.begin(), favoriteCities.end(), city) != favoriteCities.end()) 
    {
        cout << city << " is already in your favorites.\n";
        return;
    }

    favoriteCities.push_back(city);
    cout << city << " added to favorites.\n";
}

// Lists weather details for all favorite cities
void listFavoriteCities() 
{
    if (favoriteCities.empty()) 
    {
        cout << "No favorite cities added yet.\n";
        return;
    }

    for (const auto& city : favoriteCities) 
    {
        displayWeather(city);
        cout << "-------------------\n";
    }
}

// Removes a selected city from the user's favorites
void updateFavoriteCities() 
{
    if (favoriteCities.empty()) 
    {
        cout << "No cities to update.\n";
        return;
    }

    cout << "Current Favorites:\n";
    for (size_t i = 0; i < favoriteCities.size(); ++i) 
    {
        cout << i + 1 << ". " << favoriteCities[i] << "\n";
    }

    cout << "Enter number of city to remove: ";
    int index;
    cin >> index;

    if (index < 1 || index > favoriteCities.size()) 
    {
        cout << "Invalid selection.\n";
        return;
    }

    string removed = favoriteCities[index - 1];
    favoriteCities.erase(favoriteCities.begin() + index - 1);
    cout << removed << " removed from favorites.\n";
}

// Displays the main menu and handles user input
void menu() 
{
    int choice;
    do 
    {
        cout << "\nWeather App Menu:\n";
        cout << "1. Search for weather\n";
        cout << "2. Add city to favorites\n";
        cout << "3. List favorite cities\n";
        cout << "4. Update favorite cities\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        string city;
        switch (choice) 
        {
        case 1:
            cout << "Enter city name: ";
            cin.ignore();  // Clear newline character from input buffer
            getline(cin, city);  // Allow full city name with spaces
            displayWeather(city);
            break;
        case 2:
            cout << "Enter city name: ";
            cin.ignore();
            getline(cin, city);
            addFavoriteCity(city);
            break;
        case 3:
            listFavoriteCities();
            break;
        case 4:
            updateFavoriteCities();
            break;
        case 5:
            cout << "Goodbye!\n";
            break;
        default:
            cout << "Invalid option.\n";
        }

    } while (choice != 5);  // Continue until user chooses to exit
}

// Main function
int main() 
{
    cout << "Welcome to the Weather App!";
    curl_global_init(CURL_GLOBAL_DEFAULT);  // Initialize curl globally
    menu();
    curl_global_cleanup();  // Cleanup curl resources
    return 0;
}
