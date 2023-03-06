#include <fstream>
#include <map>
#include <vector>

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

void ASAE::readLines(const string &myFile)
{
    int pos;
    string line, code, name;
    ifstream file(myFile);
    string delimiter = ",";

    if (file.is_open())
    {
        int count = 1;
        getline(file, line);

        while (!file.eof())
        {
            getline(file, line);

            pos = line.find(delimiter);
            code = line.substr(0, pos);
            name = line.substr(pos + 1, line.size() - pos);

            lines.insert(make_pair(code, name));
            readEdges(code);

            count++;
        }
        file.close();
    }
}

void ASAE::readEdges(const string &code)
{
    for (int i = 0; i <= 1; i++)
    {
        string myfile = "../dataset/line_" + code + "_" + to_string(i) + ".csv";
        ifstream file(myfile);

        if (file.is_open())
        {
            int count = 1;
            string line, origin, dest;
            float weight;
            map<int, pair<float, float>> nodes = graph.getNodes();

            getline(file, line);   // trash
            getline(file, origin); // departure node code

            while (!file.eof())
            {
                getline(file, dest); // destination node code

                int originIndex = convertCodeToIndex(origin);
                int destIndex = convertCodeToIndex(dest);

                graph.addEdge(originIndex, destIndex, weight);

                origin = dest;
                count++;
            }
            // cout << count << " number of edges read in file " << myfile << endl;
        }
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

                    while(element[element.size() - 2] != '"') 
                    {
                        opening_hours[i] = stoi(element);
                        getline(str, element, ',');

                        i++;
                    }

                    element.erase(element.size() - 3, 3); 
                   opening_hours[i] = stoi(element);
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
            stops.insert(make_pair(extra[0], count));
            count++;
        }
    
        file.close();
    }
}

ASAE::ASAE()
{
    int nodes = numberOfLines("./dataset/establishments.csv");
    cout << nodes << " number of nodes" << endl;
    Graph g(nodes, true);
    this->graph = g;

    readEstablishments();
    /*
        cout << lines.size() << " number of lines stored " << endl;
        cout << endl <<  stops.size() << " number of stops stored " << endl;
        cout << graph.getNodes().size() << " number of stops stored in getNodes " << endl;
        */
}

void ASAE::toRead()
{
    //readLines("../dataset/distances.csv");
    cout << numberOfLines("./dataset/establishments.csv") << endl;
    cout << "ran toRead" << endl;
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
