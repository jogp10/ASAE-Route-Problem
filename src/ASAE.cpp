#include <fstream>
#include <map>
#include <vector>

#include "ASAE.h"

int ASAE::numberOfLines(const string &myfile)
{
    int number_of_lines = 0;

    string line;
    ifstream file(myfile);

    if (file.is_open())
    {
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
    string token;
    ifstream file("../dataset/establishments.csv");
    int count = 1;

    if (file.is_open())
    {
        getline(file, line);
        while (!file.eof())
        {

            vector<string> extra;
            getline(file, line);

            while ((pos = line.find(delimiter)) != std::string::npos)
            {
                token = line.substr(0, pos);
                extra.push_back(token);
                line.erase(0, pos + delimiter.length());
            }
            vector<int> opening_hours(24, 0);
            graph.setNode(stoi(extra[0]), extra[1], stof(extra[2]), stof(extra[3]), stof(extra[4]), stoi(extra[5]), opening_hours);
            stops.insert(make_pair(extra[0], count));
            count++;
        }
        // cout << count << "number of stops read" << endl;
        file.close();
    }
}

ASAE::ASAE()
{
    int nodes = numberOfLines("../dataset/establishments.csv");
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
    readLines("../dataset/distances.csv");
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
