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
#include <sstream>
// Module inclusions
// Lots of hand waving here as I do not know what most of these do :

constexpr const char* RESET   = "\033[0m";
constexpr const char* RED     = "\033[31m";
constexpr const char* GREEN   = "\033[32m";
constexpr const char* YELLOW  = "\033[33m";
// Sets the [COLOR] to some ansi escape code

int mode = 0;
// Global variable mode: tells the program what buy mode is active [1, 10, 100, max]

std::ostringstream frame;
// Secret sauce to mitigate flickers upon screen update.
// The idea is to first write all information to a "frame",
// then print the whole thing at once.

void turn_on_cursor() {
    // Turn the cursor back on after hiding 
    frame << "\033[?25h" << std::flush;
    _getch();
}


std::pair<double, double> print_bar(double resource_time_target, int resource_id, std::string resource_name, double all_money, bool resource_owned, int mul, double resource_cost, double rate, int resource_level, double resource_time, double count, double revenue, double segments = 20){
    // This function writes the UI to a "frame".
    // More explicitly, the print_bar function will write each line of the UI to a string that acts as a frame.
    // Instead of using std::endl; to print the line, we use \n at the end to start a new line.

    int funds = all_money;
    double epsilon = 1e-8;
    double q = count / resource_time;
    double result = std::fmod((count), (resource_time));

    int mod = (std::abs(q - std::round(q)) < epsilon)
        ? static_cast<int>(std::round(q))
        : static_cast<int>(std::floor(q));
    // Mod represents the number of completed time cycles for a resource
    // Mod rounds up to the nearest integer when within range epsilon
    // Otherwise, it stays on the current cycle

    if (resource_owned){
        frame << "["<<resource_id<<"] " << resource_name << " \033[K" ;
        frame << std::fixed << std::setprecision(2) << resource_time - result << "s \033[K" << "\n";
    }
    else{
        frame << "["<<resource_id<<"] ???" << "\033[K" << "\n";
    }
    frame << "Lv \033[K" << resource_level << " ";
    if(resource_level != 0){
        for (int x = resource_level; x < 999;){
            frame << " \033[K";
            x *= 10;
        }
    }
    else{
        frame << "   \033[K";
    }
    // Print logic for the UI. Writes the id, name and level of each resource
    // Handles the number of spaces needed after each level to keep progress bar in same place.

    if (std::abs(result) < 1e-6 || std::abs(result) >= resource_time - 1e-6 ) {
        result = 0;
    }
    if (resource_owned){
        if ((rate*mod) >= (revenue+rate) && count != 0){
            frame << "[";
            double ratio = std::fmod(count, resource_time) / resource_time;
            for (int i = 0; i < segments; ++i) {
                double threshold = static_cast<double>(i + 1) / segments;
                if (ratio >= threshold - 0.0001 || resource_time == .1) {
                    frame << GREEN << "#" << RESET;
                } else {
                    frame << " ";
                }
            }
            // Writes each # in the progress bar by checking the current time ratio agains the segment ratio,
            // Each time the time ratio is greater than x segment's ratio limit, a # is placed. 
            // Once the time ratio is lower, the remaining segments are filled with spaces

            if (count != 0 && (0.0001 >= result || resource_time - result <= 0.0001 || count >= resource_time_target)){
                revenue += rate;
                resource_time_target += resource_time;
                frame << "] " <<YELLOW << " +$" << rate << "! \033[K" << RESET << "\n";
            }
            else{
                frame << "] \033[K" << "\n";
            }
            // Handles updating the revenue and target time for a resource
            // Once the required time for a resource has passed, the program will update the revenue and target time.
            // Target time is the next time point that must be met to say that the resource should pay out.
            // If the checks fail, the program simply ends the progress bar without updating time or revenue.

        }
        // Additional print logic for the UI. Writes the progress bar.

        else{
            frame << "[";
            double ratio = std::fmod(count, resource_time) / resource_time;
            for (double i = 0; i < segments; ++i) {
                double threshold = static_cast<double>(i + 1) / segments;
                if (ratio >= threshold - 0.0001) {
                    frame << GREEN << "#" << RESET;
                } else {
                    frame << " ";
                }
            }
            frame <<"]\033[K" << "\n";
        }
        if (mode != 3){
            double cost = 0;
            int x = 0;
            for (x = 0; x < mul; x++){
                cost += std::round(resource_cost * std::pow(1.04, x) * 100.0) / 100.0;
            } 
            frame << "Buy       \033[K";        
            frame << "Cost: $\033[K" << std::fixed << std::setprecision(2) << cost << "\n";
            frame << mul << "x      ";
            if(mul != 1){
                for (int x = mul; x < 100;){
                    frame << " \033[K";
                    x *= 10;
                }
            }
            else{
                frame << "  \033[K";
            }
            frame << "$" << std::fixed << std::setprecision(2) << rate * 1/resource_time << " /sec\033[K" << "\n";
            frame << "\033[K" << "\n";  
            // Calculate how much it costs to buy 1, 10, or 100 upgrades       
            // In addition, to accomodate the size of 1x, 10x, 100x, the code also finds the number of spaces needed following the multiplier
            // This is used to keep the $cost /sec in the same position regardless of multiplier size
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
            frame << "Buy       \033[K";        
            frame << "Cost: $\033[K" << std::fixed << std::setprecision(2) << cost << "\n";
            frame << x << "x      ";
            if(x > 1){
                for (int i = x; i < 100;){
                    frame << " \033[K";
                    i *= 10;
                }
            }
            else{
                frame << "  \033[K";
            }
            frame << "$" << std::fixed << std::setprecision(2) << rate * 1/resource_time << " /sec\033[K" << "\n";
            frame << "\033[K" << "\n";         
            // Similar to the if portion, but differs in two ways.
            // Firstly, this version finds the maximum number of upgrades that can be bought for a given resource
            //          It then determines the cost of buying the maximum number of upgrades
            // Secondly, this bersion is always updating the cost and number of upgrades available to purchase
            // The space logic is the same as the previous version
        }

        return {revenue, resource_time_target};
    }
    else{        
        frame << "[";
        double ratio = std::fmod(count, resource_time) / resource_time;
        for (double i = 0; i < segments; ++i) {
            frame << " ";
        }
        frame <<"]\033[K" << "\n"; 
        frame << "Not yet discovered\033[K" << "\n";       
        frame << "Unlock Cost: $\033[K" << resource_cost << "\n";
        frame << "\033[K" << "\n";
 
        return {0, resource_time_target};
        // When the resource has not been purchased, the user is unable to see the name but can see the cost.
        // The progress bar is empty
    }
}

// This is my main function
int main() {
    std::atexit(turn_on_cursor);
    // Turn the cursor back on after the game ends

    sf::SoundBuffer selectBuffer;
    if (!selectBuffer.loadFromFile("fx.wav")) { // Replace with your sound file path
        frame << "Error: Could not load sound file!" << "\n";
        return -1;
    }
    sf::Sound selectSound(selectBuffer);
    selectSound.setVolume(200.0f); // Set volume to 50%
    // Load background music
    sf::Music music;
    if (!music.openFromFile("23.wav")) { // Replace with your music file path
        frame << "Error: Could not load music file!" << "\n";
        return -1;
    }
    music.setVolume(50.0f);
    music.setLooping(true); // Loop the music
    music.play(); // Start playing the music

    std::array<int, 4> mul = {1, 10, 100, 0};
    // mul represents the buy multiplier for each mode, 1x, 10x, 100x
    // 0 is there as a place holder for MAX

    std::array<double, 10> revenue = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<double, 10> rate =  {3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    std::array<double, 10> resource_time = {2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0};
    std::array<double, 10> resource_time_target = {2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0};    
    std::array<bool, 10> resource_owned = {true, false, false, false, false, false, false, false, false, false};
    std::array<int, 10> resource_id = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};    
    std::array<int, 10> resource_level = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0};    
    std::array<double, 10> resource_cost = {6, 8, 10, 12, 14, 16, 18, 20, 22, 24};        
    std::array<int, 10> lim = {50, 50 ,50 ,50 ,50, 50 ,50, 50, 50, 50}; 
    std::array<double, 10> count = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::array<std::string, 10> resource_name = {"Chisel & Stone", "Quill & Ink", "Pencil", "Pen", "Typewriter", "Keyboard", "Gesture Typing", "Voice Typing", "AI Agent", "Neural Link"};
    // Resource information
    
    double deficit = 0.0; // Money spent
    double all_money = 0.0;
    bool playing = true;
    using clock = std::chrono::steady_clock;
    auto next_tick = clock::now();
    system("cls");
    while(playing){
        // Game update loop

        frame << "\033[H";      // Put cursor on top left
        frame << "\033[?25l";  // Hide cursor
        auto current_time = clock::now();
        std::chrono::duration<double> delta_time = current_time - next_tick + std::chrono::duration<double>(0.1);
        // delta_time is used to accurately lop how much time has passed

        all_money = std::accumulate(revenue.begin(), revenue.end(), 0.0);
        all_money -= deficit;
        // all_money first sums the total revenue of all resources
        // Then subtract the total deficit/money spent on all upgrades from that total 
        if (mode != 3){
            frame << "[m] Buy " << mul[mode] << "x\033[K" << "\n";
        }
        else{
            frame << "[m] Buy MAX\033[K" << "\n";
        }
        frame << "Total funds: $\033[K" << std::fixed << std::setprecision(2) << all_money << " / $1000000" << "\n";
        frame << "" << "\n";
        for (int i = 0; i < revenue.size(); i++){
            std::pair<double, double> results = print_bar(resource_time_target[i], resource_id[i], resource_name[i], all_money, resource_owned[i], mul[mode], resource_cost[i], rate[i]*resource_level[i], resource_level[i], resource_time[i], count[i], revenue[i]);
            revenue[i] = results.first;
            resource_time_target[i] = results.second;
            // Calls the print_bar function to generate the UI
            // print)bar returns the revenue and target_time for a resource

            if (resource_owned[i]){
                count[i] += delta_time.count();
            }
            // If the resource is owned, that resource's count/time tracker is updated by delta_time
        }
        frame << "Push the [m] key to change purchase mode." << "\n";
        frame << "Enter the number of the resource you want to upgrade." << "\n";
        frame << "Each upgrade increases the revenue gained from a resource." << "\n";
        frame << "More resources are unlocked by purchasing with sufficient funds." << "\n";
        frame << "After reaching level 50, every 25 upgrades doubles production speed." << "\n";
        frame << "" << "\n";
        if (_kbhit()) {
            char ch = _getch();  // Read the character without needing to press Enter
            selectSound.play();  // Sfx for pushing a key
            if (ch == 'q') {  // Press 'q' to quit
                break;
            }
            if (ch >= '0' && ch <= '9'){
                // checks if key press was a number

                ch -= '0';
                // converst ch to the numeric value of ch
                // this value matches the resource id

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
                        // If the resource is ouned and mode is 3 (MAX)
                        // Calculates cost and number of upgrades bought under the MAX mode
                        // Buying 1 upgrade increases the costs by 4%
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
                        // If mode is not MAX
                        // Also calculates the cost, but number of upgrades is determined by mul[mode] (1, 10, 100)
                    }
                    while (resource_level[ch] >= lim[ch] && resource_time[ch] != .1){
                        resource_time[ch] /= 2;
                        if (resource_time[ch] <= .1){
                            resource_time[ch] = .1;
                        } 
                        lim[ch] += 25;
                    }
                    // Each resourece has a limit that determines when the production speed doubles
                    // If that limit is hit, the time needed for payout from a resources is cut in half
                    // This continues every 25 levels after the limit, but stops when the production speed is 10x per second
                }
                else{
                    if (resource_owned[ch] == false && all_money >= resource_cost[ch]){
                        resource_owned[ch] = true;
                        resource_level[ch] += 1;
                        deficit += resource_cost[ch];
                    }
                    // If the resource is not owned yet, it will be unlocked and added to the list of owned resources
                    // deficit increases by the unlock cost
                }
            }
            if (ch == 'm'){
                mode += 1;
                mode %= 4;
            }
            // Changes the mode if key pressed is m
            // mode changes from (0, 1, 2, 3)
        }
        if (all_money >= 1000000) {
            playing = false;
            int minutes = count[0] /60;
            frame << "You Win!\033[K" << "\n";
            frame << "Time taken: " << minutes << " minutes " << std::fmod(count[0], 60) << " seconds" << "\n";
            frame << "Push any key to exit the game" << "\n";
            
            
        }
        // Ends the game if user earns one million dollars
        // Returns time taken

        next_tick = clock::now();
        next_tick += std::chrono::duration_cast<clock::duration>(std::chrono::duration<double>(.1));
        std::this_thread::sleep_until(next_tick);
        // The program updates at 10 frames per second

        std::cout << frame.str() << std::flush;
        // This is the secret sauce
        // Updating the terminal line by line introduces flickering 
        // So every line written before this was written to a string/frame/buffer
        // By including this line here, the completed frame is printed to the terminal all at once
        // This removes the flickering, creating seamless visual updates

        frame.str("");         // Clear the string buffer
        frame.clear();         // clear the frame
    }
}