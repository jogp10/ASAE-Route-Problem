#include <fstream>
#include <vector>
#include <cmath>

#include "ASAE.h"

using namespace std;

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

void ASAE::readTimeDistances(int n)
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

void ASAE::readEstablishments(int n)
{
    string line, delimiter = ",";
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

    this->graph = Graph(number_of_establishments, true, {0, 0, 0, 9}, {0, 0, 0, 8});

    readEstablishments(number_of_establishments);
    readTimeDistances(number_of_establishments);


    vector<list<int>> solution;
    solution = graph.generate_random_solution();
    //Graph::printSolution(solution);


    //stop execution
    // Plot establishments in a map
    graph.plotGraph();
    cout << "Done building graph." << endl;
}

void ASAE::menu() {
    int option = 0;

    cout << endl << "Welcome to the ASAE!" << endl;
    cout << endl;
    while (option != 5) {
        cout << "1 - Show all establishments" << endl;
        cout << "2 - Show all establishments of a given type" << endl;
        cout << "3 - Show all establishments of a given type and with a given name // tabu search here" << endl;
        cout << "4 - Show all establishments of a given type and with a given name and with a given opening hour // crossover testing" << endl;
        cout << "5 - Exit" << endl;
        cout << "Option: ";
        cin >> option;

        vector<list<int>> solution, solution1, solution2;
        switch (option) {
            case 0:
                return;
            case 1:
                graph.showAllEstablishments();
                break;
            case 2:
                solution = (graph.*(&Graph::hillClimbing))(1000, (&Graph::mutation_solution_6), (&Graph::evaluate_solution), false);
                break;
            case 3:
                solution = (graph.*(&Graph::tabuSearch))(1000, (&Graph::mutation_solution_5), (&Graph::evaluate_solution), 20, false);
                break;
            case 4:
                solution1 = (graph.*(&Graph::hillClimbing))(1000, (&Graph::mutation_solution_6), (&Graph::evaluate_solution), false);
                solution2 = (graph.*(&Graph::tabuSearch))(1000, (&Graph::mutation_solution_5), (&Graph::evaluate_solution), 20, false);
                cout << "step1" << endl;
                graph.crossover_solutions_1(solution1, solution2);
                graph.crossover_solutions_2(solution1, solution2);
                cout << "step2" << endl;

                break;
            case 5:
                break;
            default:
                cout << "Invalid option." << endl;
                break;
        }
        Graph::printSolution(solution);
        cout << graph.check_solution(solution) << endl;
        cout << graph.evaluate_solution(solution) << endl;
        //graph.printDetailedSolution(solution);
    }

}

bool ASAE::hasSubstring(const std::string& str)
{
    return str.find("p_") != std::string::npos;
}
