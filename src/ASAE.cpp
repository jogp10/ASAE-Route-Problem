#include <fstream>
#include <map>
#include <vector>
#include <math.h>

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

void ASAE::readTimeDistances()
{
        string myFile = "./dataset/distances.csv";

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
                    if(weight[weight.size()-3] == 'p' | weight[weight.size()-3] == '_') { break; }
                    graph.addEdge(origin, destin, stof(weight));
                    destin++;
                }
                origin++;
            }
        }
}

void ASAE::readEstablishments()
{
    string line, delimiter = ",";
    ifstream file("./dataset/establishments.csv");

    if (file.is_open())
    {
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
            graph.setNode(stoi(extra[0]), extra[4], stof(extra[5]), stof(extra[6]), stof(extra[7]), stoi(extra[8]), opening_hours);
        }
    
        file.close();
    }
}

ASAE::ASAE()
{
    int nodes = numberOfLines("./dataset/establishments.csv");
    this->graph = Graph(nodes, true);

    readEstablishments();
    readTimeDistances();

    setNumberOfVans(nodes);

    cout << "Done building graph." << endl;
}

void ASAE::setNumberOfVans(int n)
{
    this->number_of_vans = floor(n * 0.1);
}



void ASAE::setTime(int seconds, int minutes, int hours)
{
    this->seconds = seconds;
    this->minutes = minutes;
    this->hours = hours;
}

void ASAE::addTime(int seconds, int minutes, int hours)
{
    this->seconds += seconds;
    this->minutes += minutes;
    this->hours += hours;

    if (this->seconds >= 60)
    {
        this->minutes += this->seconds / 60;
        this->seconds = this->seconds % 60;
    }

    if (this->minutes >= 60)
    {
        this->hours += this->minutes / 60;
        this->minutes = this->minutes % 60;
    }

    if (this->hours >= 24)
    {
        this->hours = this->hours % 24;
    }
}

int ASAE::getNumberOfVans(int numberOfVans) {
    return this->number_of_vans;
}

