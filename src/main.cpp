#include <iostream> // I need to include the I/O libray for input and output
#include <SFML/Audio.hpp> // Include SFML Audio module
#include <cstdlib>
#include <chrono>
#include <thread>
#include <numeric>
#include <cmath>
#include <conio.h>
#include <iomanip>
#include <tuple>

constexpr const char* RESET   = "\033[0m";
constexpr const char* RED     = "\033[31m";
constexpr const char* GREEN   = "\033[32m";
constexpr const char* YELLOW  = "\033[33m";
int mode = 0;


std::pair<double, double> print_bar(double resource_time_target, int resource_id, std::string resource_name, double all_money, bool resource_owned, int mul, double resource_cost, double rate, int resource_level, double resource_time, double count, double revenue, double segments = 20){
    //std::fmod(count, rate[i]);
    int funds = all_money;
    double epsilon = 1e-8;
    double q = count / resource_time;
    double result = std::fmod((count), (resource_time));

    int mod = (std::abs(q - std::round(q)) < epsilon)
        ? static_cast<int>(std::round(q))
        : static_cast<int>(std::floor(q));

    if (resource_owned){
        std::cout << "["<<resource_id<<"] " << resource_name << " \033[K" ;
        std::cout << std::fixed << std::setprecision(2) << resource_time - result << "s \033[K" << std::endl;

    }
    else{
        std::cout << "["<<resource_id<<"] ???" << "\033[K" << std::endl;
    }
    std::cout << "Lv \033[K" << resource_level << " ";
    if(resource_level != 0){
        for (int x = resource_level; x < 999;){
            std::cout << " \033[K";
            x *= 10;
        }
    }
    else{
        std::cout << "   \033[K";
    }

    if (std::abs(result) < 1e-6 || std::abs(result) >= resource_time - 1e-6 ) {
        result = 0;
    }
    if (resource_owned){
        if ((rate*mod) >= (revenue+rate) && count != 0){
            std::cout << "[";
            double ratio = std::fmod(count, resource_time) / resource_time;
            for (int i = 0; i < segments; ++i) {
                double threshold = static_cast<double>(i + 1) / segments;
                if (ratio >= threshold - 0.0001 || resource_time == .1) {
                    std::cout << GREEN << "#" << RESET;
                } else {
                    std::cout << " ";
                }
            }
            if (count != 0 && (0.0001 >= result || resource_time - result <= 0.0001 || count >= resource_time_target)){
                revenue += rate;
                resource_time_target += resource_time;
                std::cout << "] " <<YELLOW << " +$" << rate << "! \033[K" << RESET << std::endl;
            }
            else{
                std::cout << "] \033[K" << std::endl;
            }
        }
        else{
            std::cout << "[";
            double ratio = std::fmod(count, resource_time) / resource_time;
            for (double i = 0; i < segments; ++i) {
                double threshold = static_cast<double>(i + 1) / segments;
                if (ratio >= threshold - 0.0001) {
                    std::cout << GREEN << "#" << RESET;
                } else {
                    std::cout << " ";
                }
            }
            std::cout <<"]\033[K" << std::endl;
        }
        if (mode != 3){
            double cost = 0;
            int x = 0;
            for (x = 0; x < mul; x++){
                cost += std::round(resource_cost * std::pow(1.04, x) * 100.0) / 100.0;
            } 
            std::cout << "Buy       \033[K";        
            std::cout << "Cost: $\033[K" << std::fixed << std::setprecision(2) << cost << std::endl;
            std::cout << mul << "x      ";
            if(mul != 1){
                for (int x = mul; x < 100;){
                    std::cout << " \033[K";
                    x *= 10;
                }
            }
            else{
                std::cout << "  \033[K";
            }
            std::cout << "$" << std::fixed << std::setprecision(2) << rate * 1/resource_time << " /sec\033[K" << std::endl;
            std::cout << "\033[K" << std::endl;         
        }
        else{
            double cost = resource_cost;
            int x = 0;
            for (x = 0; cost <= funds; x++){
                cost += std::round(resource_cost * std::pow(1.04, x) * 100.0) / 100.0;
            } 
            cost = 0;
            for (int i = 0; i < x; i++){
                cost += std::round(resource_cost * std::pow(1.04, i) * 100.0) / 100.0;

            }
            std::cout << "Buy       \033[K";        
            std::cout << "Cost: $\033[K" << std::fixed << std::setprecision(2) << cost << std::endl;
            std::cout << x << "x      ";
            if(x > 1){
                for (int i = x; i < 100;){
                    std::cout << " \033[K";
                    i *= 10;
                }
            }
            else{
                std::cout << "  \033[K";
            }
            std::cout << "$" << std::fixed << std::setprecision(2) << rate * 1/resource_time << " /sec\033[K" << std::endl;
            std::cout << "\033[K" << std::endl;         
        
        }

        return {revenue, resource_time_target};
    }
    else{        
        std::cout << "[";
        double ratio = std::fmod(count, resource_time) / resource_time;
        for (double i = 0; i < segments; ++i) {
            std::cout << " ";
        }
        std::cout <<"]\033[K" << std::endl; 
        std::cout << "Not yet discovered\033[K" << std::endl;       
        std::cout << "Unlock Cost: $\033[K" << resource_cost << std::endl;
        std::cout << "\033[K" << std::endl;
 
        return {0, resource_time_target};
    }
}

// This is my main function
int main() {
    sf::SoundBuffer selectBuffer;
    if (!selectBuffer.loadFromFile("fx.wav")) { // Replace with your sound file path
        std::cout << "Error: Could not load sound file!" << std::endl;
        return -1;
    }
    sf::Sound selectSound(selectBuffer);
    selectSound.setVolume(200.0f); // Set volume to 50%
    // Load background music
    sf::Music music;
    if (!music.openFromFile("23.wav")) { // Replace with your music file path
        std::cout << "Error: Could not load music file!" << std::endl;
        return -1;
    }
    music.setVolume(50.0f);
    music.setLooping(true); // Loop the music
    music.play(); // Start playing the music
    
    using clock = std::chrono::steady_clock;
    auto next_tick = clock::now();
    bool playing = true;
    std::array<double, 10> revenue = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<double, 10> rate =  {3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    std::array<double, 10> resource_time = {2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0};
    std::array<int, 10> resource_id = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::array<bool, 10> resource_owned = {true, false, false, false, false, false, false, false, false, false};
    std::array<int, 10> resource_level = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0};    
    std::array<double, 10> resource_cost = {6, 8, 10, 12, 14, 16, 18, 20, 22, 24};        
    std::array<int, 4> mul = {1, 10, 100, 0};
    std::array<int, 10> lim = {50, 50 ,50 ,50 ,50, 50 ,50, 50, 50, 50}; 
    std::array<double, 10> count = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<std::string, 10> resource_name = {"Chisel & Stone", "Quill & Ink", "Pencil", "Pen", "Typewriter", "Keyboard", "Gesture Typing", "Voice Typing", "AI Agent", "Neural Link"};
    std::array<double, 10> resource_time_target = {2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0};    
    double segments = 5.0;
    double deficit = 0.0;
    double all_money = 0.0;
    system("cls");
    while(playing){
        all_money = std::accumulate(revenue.begin(), revenue.end(), 0.0);
        all_money -= deficit;
        if (mode != 3){
            std::cout << "[m] Buy " << mul[mode] << "x\033[K" << std::endl;
        }
        else{
            std::cout << "[m] Buy MAX\033[K" << std::endl;
        }
        std::cout << "Total funds: $\033[K" << std::fixed << std::setprecision(2) << all_money << " / 1000000" << std::endl;
        std::cout << "" << std::endl;
        for (int i = 0; i < revenue.size(); i++){
            std::pair<double, double> results = print_bar(resource_time_target[i], resource_id[i], resource_name[i], all_money, resource_owned[i], mul[mode], resource_cost[i], rate[i]*resource_level[i], resource_level[i], resource_time[i], count[i], revenue[i]);
            revenue[i] = results.first;
            resource_time_target[i] = results.second;
            
            if (resource_owned[i]){
                count[i] += .1;
            }
        }
        std::cout << "Push the [m] key to change purchase mode." << std::endl;
        std::cout << "Enter the number of the resource you want to upgrade." << std::endl;
        std::cout << "Each upgrade increases the revenue gained from a resource." << std::endl;
        std::cout << "More resources are unlocked by purchasing with sufficient funds." << std::endl;
        std::cout << "After reaching level 50, every 25 upgrades doubles production speed." << std::endl;
        std::cout << "" << std::endl;
        if (_kbhit()) {
            char ch = _getch();  // Read the character without needing to press Enter
            selectSound.play();
            std::cout << "Last pressed key: " << ch << std::endl;
            if (ch == 'q') {  // Press 'q' to quit
                break;
            }
            if (ch >= '0' && ch <= '9'){
                ch -= '0';
                if (resource_owned[ch]){
                    if (mode == 3){
                        double cost = resource_cost[ch];
                        int x = 0;
                        for (x = 0; cost <= all_money; x++){
                            cost += resource_cost[ch] * std::pow(1.04, x+1);
                        } 

                        cost = 0;
                        for (int i = 0; i < x; i++){
                            cost += resource_cost[ch] * std::pow(1.04, i);
                        }
                        if (all_money >= cost){
                            deficit += cost;
                            resource_level[ch] += x;
                            resource_cost[ch] *= std::pow(1.04, x);
                            resource_cost[ch] = std::round(resource_cost[ch] * 100.0) / 100.0;
                        }
                    }
                    else {
                        double cost = 0;
                        int x = 0;
                        for (x = 0; x < mul[mode]; x++){
                            cost += resource_cost[ch] * std::pow(1.04, x);
                        } 
                        if (all_money >= cost){
                            deficit += cost;
                            resource_level[ch] += mul[mode];
                            resource_cost[ch] *= std::pow(1.04, mul[mode]);
                            resource_cost[ch] = std::round(resource_cost[ch] * 100.0) / 100.0;
                        }
                    }
                    while (resource_level[ch] >= lim[ch]){
                        if (resource_time[ch] <= .1){
                            resource_time[ch] = .1;
                        }
                        else{
                            resource_time[ch] /= 2;
                        }
                        lim[ch] += 25;
                    }
                    
                }
                else{
                    if (resource_owned[ch] == false && all_money >= resource_cost[ch]){
                        resource_owned[ch] = true;
                        resource_level[ch] += 1;
                        deficit += resource_cost[ch];
                        
                    }
                }
            }
            if (ch == 'm'){
                mode += 1;
                mode %= 4;
            }
        }
        if (all_money >= 1000000) {
            playing = false;
            int minutes = count[0] /60;
            std::cout << "You Win!\033[K" << std::endl;
            std::cout << "Time taken: " << minutes << " minutes " << std::fmod(count[0], 60) << " seconds" << std::endl;
        }

        next_tick = clock::now();
        next_tick += std::chrono::duration_cast<clock::duration>(std::chrono::duration<double>(.1));
        std::this_thread::sleep_until(next_tick);
        std::cout << "\033[H";
    }
}