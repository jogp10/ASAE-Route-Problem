#include <fstream>
#include <vector>
#include <thread>

#include "ASAE.h"


int ASAE::numberOfLines(const string &myFile)
{
    int number_of_lines = 0;

    string line;
    fstream file;
    file.open(myFile, fstream::in);

    if (file.is_open())
    {
        getline(file, line); // trash

        while (!file.eof())
        {
            getline(file, line);
            number_of_lines++;
        }
        file.close();
    }
    return number_of_lines - 1;
}

void ASAE::readTimeDistances(const int &n)
{
        string myFile = distancesFile;

        ifstream file(myFile);

        if (file.is_open()) {
            int origin = 0;
            string line;
            char sep = ',';

            getline(file, line);   // trash
            getline(file, line, sep);   // trash


            while (!file.eof()) {
                string weight;

                int destin = 0;

                while(getline(file, weight, sep)) {


                    graph.addEdge(origin, destin, stof(weight));
                    destin++;
                    if(destin == n && destin != max_establishments) {
                        getline(file, weight); // trash
                        getline(file, weight, sep); // trash
                        if(hasSubstring(weight)) { break; }
                        break; }
                    if(hasSubstring(weight)) { break; }
                }

                origin++;
                if(origin == n) {
                    break;
                }
            }
        }
}

void ASAE::readEstablishments(const int &n)
{
    string line;
    ifstream file(establishmentsFile);

    if (file.is_open())
    {
        int nrEstablishmentsRead = 0;
        getline(file, line); // trash
        while (getline(file, line))
        {
            vector<string> extra;
            vector<int> opening_hours(24); 
            string element;
            stringstream str(line); 

            while(getline(str, element, ',')) 
            {
                if (element[1] == '[') {
                    int i = 0; 
                    element.erase(0, 2);

                    while(element[2] != ']')
                    {
                        opening_hours[i] = stoi(element);
                        getline(str, element, ',');

                        i++;
                    }

                    element.erase(2, element.size() - 2);
                    opening_hours[i] = stoi(element) ;
                }
                else if(element[0] == '"') 
                {
                    element.erase(0, 1);

                    while(element[element.size() - 1] != '"') 
                    {
                        string temp;
                        getline(str, temp, ',');
                        element += "," + temp;
                    }

                    element.erase(element.size() - 1);
                }

                extra.push_back(element);  
            }
            graph.setNode(stoi(extra[0]), extra[1], extra[2], extra[3], extra[4], stof(extra[5]), stof(extra[6]), stof(extra[7]), stoi(extra[8]), opening_hours);
            nrEstablishmentsRead++;
            if(nrEstablishmentsRead == n) {
                break;
            }
        }
    
        file.close();
    }
}

ASAE::ASAE()
{
    //srand(time(nullptr));
    max_establishments = numberOfLines(distancesFile);

    int number_of_establishments = 1001;

    this->graph = Graph(number_of_establishments, true, {0, 0, 0, 9, 0}, {0, 0, 0, 8, 0});

    readEstablishments(number_of_establishments);
    readTimeDistances(number_of_establishments);

    /*
    vector<list<int>> solution;

    list<int> vehicle1;
    vehicle1.push_back(0);
    vehicle1.emplace_back(708);
    vehicle1.emplace_back(463);
    vehicle1.emplace_back(29);
    vehicle1.emplace_back(747);
    vehicle1.emplace_back(603);
    vehicle1.emplace_back(97);
    vehicle1.emplace_back(526);
    vehicle1.emplace_back(490);
    vehicle1.emplace_back(584);
    vehicle1.emplace_back(810);
    vehicle1.emplace_back(521);
    vehicle1.emplace_back(659);
    vehicle1.emplace_back(725);
    vehicle1.emplace_back(0);


    vector<list<int>> solution2 = {vehicle1};

    graph.printDetailedSolution(solution2, true);
    graph.printSolution(solution2);
    cout << graph.totalOperationTime(solution2) << endl;
    */
    cout << "Done building graph." << endl;
}

void ASAE::menu() {
    string option = "";
    drawPlot();
    cout << endl << "Welcome to the ASAE!" << endl;
    cout << endl;
    while (true) {

        cout << "1 - Show all establishments" << endl;
        cout << "2 - Hill climbing" << endl;
        cout << "3 - Simulated annealing" << endl;
        cout << "4 - Tabu" << endl;
        cout << "5 - Genetic" << endl;
        cout << "0 - Exit" << endl;
        cout << "Option: ";

        std::getline(std::cin, option);
        // Check for CTRL + Z or CTRL + D input to close the program
        if (std::cin.eof()) {
            std::cout << "Come back any time soon!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits for 1 seconds before closing the window
            exit(EXIT_SUCCESS); // Closes the terminal window
        }
        bool correct = parseInput(0,5,option);
        if(correct){
            switch (std::stoi(option)) {

                case 1:
                    graph.showAllEstablishments();
                    break;
                case 2:

                    hill_climbing();
                    break;
                case 3:
                    simulated_annealing();
                    break;
                case 4:
                    tabu_search();
                    break;
                case 5:
                    genetic();
                    break;
                case 0:
                    cout << "Come back any time soon!" << endl;
                    std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits for 1 seconds before closing the window
                    exit(EXIT_SUCCESS); // Closes the terminal window
                default:
                    cout << "Invalid option." << endl;
                    break;
            }
        }
    }
}

bool ASAE::hasSubstring(const std::string& str)
{
    return str.find("p_") != std::string::npos;
}

void ASAE::drawPlot() {

    graph.plotGraph();
}

void ASAE::hill_climbing() {
    string iteration_number, mutation_func, evaluation_func;
    if(!ask_parameters(iteration_number, mutation_func, evaluation_func)) return;

    vector<list<int>> solution = (graph.*(&Graph::hillClimbing))(std::stoi(iteration_number), mutation_funcs[std::stoi(mutation_func)-1], evaluation_funcs[std::stoi(evaluation_func)-1], false);
    //graph.printDetailedSolution(solution, true);
    //graph.printSolution(solution);
    cout << graph.totalOperationTime(solution) << endl;
    cout << graph.evaluate_solution_1(solution) << endl;


}

void ASAE::simulated_annealing() {

    string iteration_number, mutation_func, evaluation_func;
    if(!ask_parameters(iteration_number, mutation_func, evaluation_func)) return;

    vector<list<int>> solution = (graph.*(&Graph::simulatedAnnealing))(std::stoi(iteration_number), mutation_funcs[std::stoi(mutation_func)-1], evaluation_funcs[std::stoi(evaluation_func)-1], false);
    //graph.printDetailedSolution(solution, true);
    //graph.printSolution(solution);
    cout << graph.totalOperationTime(solution) << endl;
    cout << graph.evaluate_solution_1(solution) << endl;

}

void ASAE::tabu_search() {

    string iteration_number, mutation_func, evaluation_func;
    if(!ask_parameters(iteration_number, mutation_func, evaluation_func)) return;

    vector<list<int>> solution = (graph.*(&Graph::tabuSearch))(std::stoi(iteration_number), 20, 3, mutation_funcs[std::stoi(mutation_func)-1] , evaluation_funcs[std::stoi(evaluation_func)-1], false);
    //graph.printDetailedSolution(solution, false);
    //graph.printSolution(solution);
    cout << graph.totalOperationTime(solution) << endl;
    cout << graph.evaluate_solution_1(solution) << endl;

}

void ASAE::genetic() {

    string iteration_number, mutation_func, evaluation_func;
    if(!ask_parameters(iteration_number, mutation_func, evaluation_func)) return;

    vector<list<int>> solution = (graph.*(&Graph::geneticAlgorithm))(std::stoi(iteration_number), 50, 4, 10, (&Graph::crossover_solutions_1), mutation_funcs[std::stoi(mutation_func)-1] , evaluation_funcs[std::stoi(evaluation_func)-1], true);
    //graph.printDetailedSolution(solution, true);
    //graph.printSolution(solution);
    cout << graph.totalOperationTime(solution) << endl;
    cout << graph.evaluate_solution_1(solution) << endl;

}

/**
 * Check if a number in the correct interval (a to b) is inputted
 * @param a
 * @param text
 * @return
 */
bool ASAE::parseInput(int a, int b, const std::string& text){
    std::vector<int> values;
    for(int i = a; i <= b; i++){
        values.push_back(i);
    }
    auto it = values.begin();
    if (isAllDigits(text)) {
        it = std::find(values.begin(), values.end(), stoi(text));
        if (it != values.end()) {
            return true;
        } else {
            std::cout << std::endl << "Input invalido!" << std::endl << std::endl;
        }
    } else {
        std::cout << std::endl << "Input invalido!" << std::endl << std::endl;
    }
    return false;
}

/**
 * Checks if a string represents a number
 * @param str
 * @return
 */
bool ASAE::isAllDigits(const std::string &str){

    // Iterates each character in a std::string and checks if it's an integer or not
    if(str.empty()) return false;
    for(char c: str){
        if (!isdigit(c)){
            return false;
        }
    }
    return true;
}


bool ASAE::ask_parameters(string &iteration_number, string &mutation_func, string &evaluation_func){

    bool back = false;
    cout << "Insert the parameters for the algorithm" << endl;
    while(true){
        cout << "Number of iterations:";
        std::getline(std::cin, iteration_number);
        // Check for CTRL + Z or CTRL + D input to close the program
        if (std::cin.eof()) {
            std::cout << "Come back any time soon!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits for 1 seconds before closing the window
            exit(EXIT_SUCCESS); // Closes the terminal window
        }
        bool correct = parseInput(1,10000,iteration_number);
        if(correct){
            break;
        }
        else{
            cout << "Invalid number of iterations. Insert a number superior from 1 to 10000" << endl;
            cout << "Recommended number of iterations: 1000" << endl;
        }
    }
    while(true){
        cout << "Mutation function:" << endl;
        cout << "1 - Exchange two establishments in a solution" << endl;
        cout << "2 - Exchange one establishment for one that wasn't visited" << endl;
        cout << "3 - Generate a new path between two nodes" << endl;
        cout << "4 - Scramble the order of a subset of nodes in a vehicle's path" << endl;
        cout << "5 - Remove a node from a vehicle's path and insert two new nodes" << endl;
        cout << "6 - Choose a random mutation" << endl;
        cout << "0 - Back" << endl;
        cout << "Option: ";
        std::getline(std::cin, mutation_func);
        // Check for CTRL + Z or CTRL + D input to close the program
        if (std::cin.eof()) {
            std::cout << "Come back any time soon!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits for 1 seconds before closing the window
            exit(EXIT_SUCCESS); // Closes the terminal window
        }
        bool correct = parseInput(0,6,mutation_func);
        if(correct){
            if (std::stoi(mutation_func) == 0) {
                back = true;
            }
            break;
        }
        else{
            cout << "Invalid mutation function." << endl;
        }
    }
    if(back){
        return false;
    }

    while (true){
        cout << "Evaluation function:" << endl;
        cout << "1 - Sum of the visited Establishments" << endl;
        cout << "2 - Minimize number of parish transfers" << endl;
        cout << "3 - Sum of the visited Establishments (penalize invalid solutions)" << endl;
        cout << "4 - Minimize travel time and waiting time" << endl;
        cout << "0 - Back" << endl;
        getline(cin, evaluation_func);
        // Check for CTRL + Z or CTRL + D input to close the program

        if (std::cin.eof()) {
            std::cout << "Come back any time soon!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits for 1 seconds before closing the window
            exit(EXIT_SUCCESS); // Closes the terminal window
        }
        bool correct = parseInput(0,4,evaluation_func);

        if(correct){
            if (std::stoi(evaluation_func) == 0) {
                back = true;
            }
            break;
        }
        else{
            cout << "Invalid evaluation function." << endl;
        }
    }
    if(back){
        return false;
    }
    return true;



}
