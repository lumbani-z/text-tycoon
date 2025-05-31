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

// Sturcture for Player stats
struct Shop{
    std::string name = "Business";
    std::string inventory[3] = {"Health Potion", "Mana Potion", "Sword"}; // This is the inventory of the shop
    int inventoryCount = 7; // This is the number of items in the shop's inventory
    int potionCost = 10;
    int swordCost = 10;
    int numHealthPotions = 3;
    int numManaPotions = 3;
    int numSwords = 1;
};



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


    //std::cout << "count: \033[K" << count << std::endl;
    if (std::abs(result) < 1e-6 || std::abs(result) >= resource_time - 1e-6 ) {
        result = 0;
    }
    //std::cout << "fmod(count, time): \033[K" << result << std::endl;
    //std::cout << "mod: \033[K" << mod << std::endl;
    //std::cout << "Raw cost: \033[K" << resource_cost << std::endl;

    if (resource_owned){
        if ((rate*mod) >= (revenue+rate) && count != 0){
      //      std::cout << "Money: " << revenue << std::endl;
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
            //int cost = resource_cost * std::pow(1.04, mul);
            if (resource_level == 0){
                std::cout << "Not yet discovered\033[K" << std::endl;       
                std::cout << "Unlock Cost: $\033[K" << resource_cost << std::endl;
                std::cout << "\033[K" << std::endl;        
            }
            else{
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
        }
        else{
            if (resource_level == 0){
                std::cout << "Not yet discovered\033[K" << std::endl;       
                std::cout << "Unlock Cost: $\033[K" << resource_cost << std::endl;
                std::cout << "\033[K" << std::endl;          

            }
            else{
                double cost = resource_cost;
                int x = 0;
                for (x = 0; cost <= funds; x++){
                    cost += std::round(resource_cost * std::pow(1.04, x) * 100.0) / 100.0;
                } 

                cost = 0;
                //int num = (funds / resource_cost);
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
        }

        return {revenue, resource_time_target};
    }
    else{        
 //       std::cout << "Money: " << revenue << std::endl;
        std::cout << "[";
        double ratio = std::fmod(count, resource_time) / resource_time;
        for (double i = 0; i < segments; ++i) {
            std::cout << " ";
        }
        std::cout <<"]\033[K" << std::endl; 
        if (mode != 3){
            //int cost = resource_cost * std::pow(1.04, mul);
            if (resource_level == 0){
                std::cout << "Not yet discovered\033[K" << std::endl;       
                std::cout << "Unlock Cost: $\033[K" << resource_cost << std::endl;
                std::cout << "\033[K" << std::endl;       

            }
            else{
                double cost = 0;
                int x = 0;
                for (x = 0; x < mul; x++){
                    cost += std::round(resource_cost * std::pow(1.04, x) * 100.0) / 100.0;
                } 
                std::cout << "Cost: \033[K" << cost << std::endl;
                std::cout << "Buy: \033[K" << mul << std::endl;        

            }
        }
        else{
            if (resource_level == 0){
                std::cout << "Not yet discovered\033[K" << std::endl;       
                std::cout << "Unlock Cost: $\033[K" << resource_cost << std::endl;
                std::cout << "\033[K" << std::endl;       

            }
            else{
                double cost = resource_cost;
                int x = 0;
                for (x = 0; cost <= funds; x++){
                    cost += std::round(resource_cost * std::pow(1.04, x+1) * 100.0) / 100.0;
                } 

                cost = 0;
                //int num = (funds / resource_cost);
                for (int i = 0; i < x; i++){
                    cost += std::round(resource_cost * std::pow(1.04, i) * 100.0) / 100.0;
                }
                std::cout << "Not yet discovered\033[K" << std::endl;       
                std::cout << "Unlock Cost: $\033[K" << resource_cost << std::endl;
                std::cout << "\033[K" << std::endl;   

            }
        }
 
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
            std::cout << "Buy " << mul[mode] << "x\033[K" << std::endl;
        }
        else{
            std::cout << "Buy MAX\033[K" << std::endl;
        }
        std::cout << "Total funds: $\033[K" << std::fixed << std::setprecision(2) << all_money << std::endl;
        std::cout << "" << std::endl;
        for (int i = 0; i < revenue.size(); i++){
//            std::cout << "Total funds: " << all_money << std::endl;
            std::pair<double, double> results = print_bar(resource_time_target[i], resource_id[i], resource_name[i], all_money, resource_owned[i], mul[mode], resource_cost[i], rate[i]*resource_level[i], resource_level[i], resource_time[i], count[i], revenue[i]);
//            all_money = std::accumulate(revenue.begin(), revenue.end(), 0);
            revenue[i] = results.first;
            resource_time_target[i] = results.second;
            
            if (resource_owned[i]){
                count[i] += .1;
            }
        }
        std::cout << "Push the [m] key to change purchase mode." << std::endl;
        std::cout << "Enter the number of the resource you want to upgrade." << std::endl;
        std::cout << "Each upgrade increases the revenue gained from the resource." << std::endl;
        std::cout << "More resources are unlocked once you reach sufficient funds." << std::endl;
        std::cout << "After reaching level 50, every 25 upgrades doubles production speed." << std::endl;
        if (_kbhit()) {
            char ch = _getch();  // Read the character without needing to press Enter
            std::cout << "Last pressed key: " << ch << std::endl;
            if (ch == 'q') {  // Press 'q' to quit
                break;
            }
            if (ch >= '0' && ch <= '9'){
                ch -= '0';
                if (resource_owned[ch]){
                    if (mode == 3){
                        //num = all_money / resource_cost[ch];
                        double cost = resource_cost[ch];
                        int x = 0;
                        for (x = 0; cost <= all_money; x++){
                            cost += resource_cost[ch] * std::pow(1.04, x+1);
                        } 

                        cost = 0;
                        //int num = (funds / resource_cost);
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
            std::cout << "You Win!!\033[K" << std::endl;
        }

        next_tick = clock::now();
        next_tick += std::chrono::duration_cast<clock::duration>(std::chrono::duration<double>(.1));
        std::this_thread::sleep_until(next_tick);
        std::cout << "\033[H";

    }
}
    /*
    // Instance of the player struct
    Player player;
    player.health = 100; // Default value for HP
    player.gold = 100; // Default value for gold
    player.level = 1; // Default value for level
    player.xp = 0; // Default value for XP

    Shop shop;

    //  std::cout is used to print to the console
    //  std::endl is used to end the line
    //  cout means character output stream
    //  endl means end line
    system("cls");
    std::cout << "Welcome to the title screen." << std::endl;
    std::cout << "For this introductory project, you will experience the tutorial shop of an RPG." << std::endl;


    // We ask the player for their name
    std::cout << "Before we begin, what is your name? ";
    std::cin >> player.name;
    selectSound.play();

    // We greet the player


    // int choice holds the player's decision
    int choice;
    // int nestedchoice holds the user's nested decision
    int nestedChoice;
    // The player is shopping
    bool shopping = true;
    bool invalidChoice = false; // Flag for invalid choice

    while(shopping){
        system("cls");
        std::cout << "Welcome to "<< shop.name << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl;
        std::cout << "What will you do?" << std::endl;
        std::cout << "1. Purchase items" << std::endl;
        std::cout << "2. Check status" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Please enter your choice [1-3]: " << std::endl;
        if (invalidChoice){
            std::cout << "Invalid choice!" << std::endl;
            invalidChoice = false; // Reset the invalidChoice flag
        }
        std::cin >> choice; // Logs the user's choice
        selectSound.play(); // Play the sound effect
        // Checks the user's choice and diplays one of the following messages
        if (choice == 1){
                bool buying = true;
                bool noGold = false;
                bool outOfStock = false;
                bool succesfulPurchase = false;
                int healthPotionStock = shop.numHealthPotions;
                int manaPotionStock = shop.numManaPotions;
                int swordStock = shop.numSwords;
                while(buying){
                    system("cls");
                    std::cout << "Available products!" << std::endl;
                    std::cout << "--------------------------------------------------------" << std::endl;
                    std::cout << "1. Health Potion - 10 Gold [" << shop.numHealthPotions << " Left]" << std::endl;
                    std::cout << "2. Mana Potion - 10 Gold [" << shop.numManaPotions << " Left]"  << std::endl;
                    std::cout << "3. Sword - 10 Gold [" << shop.numSwords << " Left]"  << std::endl;
                    std::cout << "4. Back" << std::endl;
                    std::cout << "Please enter your choice [1-4]: " << std::endl;
                    if (noGold){
                        std::cout << "You do not have enough gold!" << std::endl;
                        noGold = false; // Reset the noGold flag
                    }
                    else if (outOfStock){
                        std::cout << "Out of stock!" << std::endl;
                        outOfStock = false; // Reset the outOfStock flag
                    }
                    else if (invalidChoice){
                        std::cout << "Invalid choice!" << std::endl;
                        invalidChoice = false; // Reset the invalidChoice flag
                    }
                    if (succesfulPurchase){
                        std::cout << "You have successfully purchased a " << player.inventory[player.inventoryCount - 1] << "!" << std::endl;
                        succesfulPurchase = false; // Reset the succesfulPurchase flag
                    }

                    std::cin >> nestedChoice; // Logs the user's choice
                    selectSound.play();
                    if (std::cin.fail()){
                        std::cin.clear(); // Clear the error flag
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
                        invalidChoice = true; // Set the invalidChoice flag
                    }
                    else if (nestedChoice == 1){
                        if (player.gold >= shop.potionCost){
                            if (shop.numHealthPotions > 0){
                                player.inventory[player.inventoryCount] = "Health Potion";
                                player.inventoryCount++;
                                player.gold -= shop.potionCost;
                                shop.numHealthPotions--;
                                shop.inventoryCount--;
                                succesfulPurchase = true; // Set the succesfulPurchase flag to true
                            }
                            else{
                                outOfStock = true;
                            }
                        }
                        else{
                            noGold = true;
                        }
                    }
                    else if (nestedChoice == 2){
                        if (player.gold >= shop.potionCost){
                            if (shop.numManaPotions > 0){
                                player.inventory[player.inventoryCount] = "Mana Potion";
                                player.inventoryCount++;
                                player.gold -= shop.potionCost;
                                shop.numManaPotions--;
                                shop.inventoryCount--;
                                succesfulPurchase = true;
                            }
                            else{
                                outOfStock = true;
                            }
                        }
                        else{
                            noGold = true; 
                        }
                    }
                    else if (nestedChoice == 3){
                        if (player.gold >= shop.potionCost){
                            if (shop.numSwords > 0){
                                player.inventory[player.inventoryCount] = "Sword";
                                player.inventoryCount++;
                                player.gold -= shop.swordCost;
                                shop.numSwords--;
                                shop.inventoryCount--;
                                succesfulPurchase = true;
                            }
                            else{
                                outOfStock = true;
                            }
                        }
                        else{
                            noGold = true;
                        }
                    }
                    else if (nestedChoice == 4){
                        buying = false;
                    }
                    else{
                        invalidChoice = true;
                    }
                    if (std::cin.fail()){
                        std::cin.clear(); // Clear the error flag
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
                        invalidChoice = true;
                    }
                     // Reset the invalidChoice flag
                    //nestedChoice = 0;
                }
            
        }
        else if (choice == 2){
            bool checking = true;
            while(checking){
                system("cls");
                std::cout << player.name << "'s status" << std::endl;
                std::cout << "--------------------------------------------------------" << std::endl;
                std::cout << "Level: " << player.level << std::endl;
                std::cout << "XP: " << player.xp << std::endl;
                std::cout << "Health: " << player.health << std::endl;
                std::cout << "Gold: " << player.gold << std::endl;
                std::cout << "Inventory: " << std::endl;
                if (player.inventoryCount > 0){
                    for (int i = 0; i < player.inventoryCount; i++){
                        if (!player.inventory[i].empty()){
                        std::cout << "- " << player.inventory[i] << std::endl; // Display the inventory
                        }
                    }
                }
                else{
                    std::cout << "No items in inventory." << std::endl;
                }
                if (invalidChoice){
                    std::cout << "Invalid choice!" << std::endl;
                    invalidChoice = false; // Reset the invalidChoice flag
                }
                std::cout << "Enter [1] to go back" << std::endl;
                std::cin >> nestedChoice; 
                selectSound.play();
                if (std::cin.fail()){
                    std::cin.clear(); // Clear the error flag
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
                    invalidChoice = true; // Set the invalidChoice flag
                }
                if (nestedChoice == 1){
                    checking = false;
                }
                else{
                    invalidChoice = true; // Set the invalidChoice flag
                }
                    
                

            }
        }
        // If the user decides to quit the game
        else if (choice == 3){
            shopping = false;
        }
        else {
            if (std::cin.fail()){
                std::cin.clear(); // Clear the error flag
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
            }
            invalidChoice = true; // Set the invalidChoice flag
        }
    }

    music.stop(); // Stop the music when the game ends

    // Return 0 signifies that the program has ended successfully
    return 0;
    */