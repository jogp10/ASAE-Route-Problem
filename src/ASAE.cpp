#include <fstream>
#include <map>
#include <vector>
#include <math.h>

#include "ASAE.h"

using namespace std;

int ASAE::numberOfLines(const string &myfile)
{
    int number_of_lines = 0;

    string line;
    fstream file;
    file.open(myfile, fstream::in);

    if (file.is_open())
    {
        getline(file, line);

        while (!file.eof())
        {
            getline(file, line);
            number_of_lines++;
        }
        file.close();
    }
    return number_of_lines - 1;
}

void ASAE::readEdges()
{
        string myfile = "./dataset/distances.csv";

        ifstream file(myfile);

        if (file.is_open()) {
            int origin = 0;
            string line;
            char sep = ',';

            getline(file, line);   // trash
            getline(file, line, sep); // first origin


            while (!file.eof()) {
                string weight;

                int destino = 0;

                while(getline(file, weight, sep) && weight[0] != 'p') {
                    graph.addEdge(origin, destino, stof(weight));
                    destino++;
                }
                origin++;
            }
            // cout << count << " number of edges read in file " << myfile << endl;
        }
}

void ASAE::readEstablishments()
{
    string line;
    string delimiter = ",";
    size_t pos;
    ifstream file("./dataset/establishments.csv");
    int count = 1;

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

                    while(element[element.size() - 1] != '"')
                    {
                        opening_hours[i] = stoi(element);
                        getline(str, element, ',');

                        i++;
                    }

                    element.erase(element.size() - 2, 2);
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
                        //cout << temp << endl; 
                    }

                    element.erase(element.size() - 1);
                }

                extra.push_back(element);  
            }
            graph.setNode(stoi(extra[0]), extra[4], stof(extra[5]), stof(extra[6]), stof(extra[7]), stoi(extra[8]), opening_hours);
            count++;
        }
    
        file.close();
    }
}

ASAE::ASAE()
{
    int nodes = numberOfLines("./dataset/establishments.csv");
    //cout << nodes << " number of nodes" << endl;
    this->graph = Graph(nodes, true) ;

    readEstablishments();

    setNumberOfVans(nodes);

    readEdges();
    auto a = this->graph.getNode(4).adj;
    cout << "Done" << endl;


    for (auto a : this->graph.getNode(5).adj) {
        cout << a.dest << " " << a.weight << endl;
    }
    /*
        cout << lines.size() << " number of lines stored " << endl;
        cout << endl <<  stops.size() << " number of stops stored " << endl;
        cout << graph.getNodes().size() << " number of stops stored in getNodes " << endl;
        */
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

int ASAE::convertCodeToIndex(string a)
{
    return stoi(a.erase(0, 2));
}

