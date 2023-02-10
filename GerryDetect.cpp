/*-------------------------------------------

Gerrymandering Detection App

System: VSCode on Windows 10

Author: Alan Michalek

------------------------------------------- */
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <tgmath.h>

using namespace std;

// sructure to hold district number, dem/rep votes in addition to wasted votes for either party.
struct district
{
    int number;
    int democrat = 0;
    int republican = 0;

    int wastedDem = 0;
    int wastedRep = 0;
};

// struct to hold info about each state including name, a vector of the ditrict struct representing
// each district, total dem/rep wasted votes, total votes in general,
// efficiency gap, whehter or not the state is gerrymandered, and more! Fun for the whole family!
struct state
{
    string name;

    vector<district> districts;

    int totalDemWaste = 0;
    int totalRepWaste = 0;

    int totalDem = 0;
    int totalRep = 0;
    int totalVotes = 0;

    float effGap;
    string gerrymandered = "No";
    string against;

    int eligVoters;
};

// Determines who won the district.
// Takes district struct as parameter.
// Is bool type, returns true if democrat, false if republican.
bool won(district District)
{
    if (District.democrat > District.republican)
        return 1;
    return 0;
}

// Determines if state is gerrymandered, and fills in relevant info 
// for that state like wasted votes.
// Takes state struct for parameter.
void wasted(state &State)
{

    int totalDemWaste = 0, totalRepWaste = 0;
    
    // Loop iterates through districts vector. 
    for (auto district : State.districts)
    {
        
        // Overhalf number for the state. 
        int overHalf = (district.democrat + district.republican) / 2 + 1;

        // If democrats won that district
        if (won(district))
        {
            district.wastedRep = district.republican;
            district.wastedDem = abs(district.democrat - overHalf);
        }

        // If reps win that district
        else
        {
            district.wastedRep = abs(district.republican - overHalf);
            district.wastedDem = district.democrat;
        }

        // total wasted votes FOR ALL THE DISTRICTS are summed and stored into state struct.
        totalDemWaste += district.wastedDem;
        totalRepWaste += district.wastedRep;

        // Total dem/rep votes for the state stored into state struct. 
        State.totalDem += district.democrat;
        State.totalRep += district.republican;
    }
    State.totalDemWaste = totalDemWaste;
    State.totalRepWaste = totalRepWaste;
    State.totalVotes = State.totalDem + State.totalRep;

    // efficency Gap calculation.
    State.effGap = 100 * float(abs(State.totalDemWaste - State.totalRepWaste)) / float(State.totalVotes);

    // if effGap above 7, gerrymandered. And determines which state against. 
    if (State.effGap > 7 && State.districts.size() > 2)
    {
        State.gerrymandered = "Yes";
        if (State.totalDem > State.totalRep)
            State.against = "Republicans";
        else
            State.against = "Democrats";
    }
}

// Outputs gerrymandering stats.
// Takes state struct for parameter.
// Returns nothing, just cout statements for relevant gerry statistics.
void stats(state State)
{
    wasted(State);
    cout << endl;
    cout << "Gerrymandered: " << State.gerrymandered << endl;
    if (State.gerrymandered == "Yes")
    {
        cout << "Gerrymandered against: " << State.against << endl;

        // setprecision so it matches zybooks testcases.
        cout << "Efficiency Factor: " << setprecision(6) << State.effGap << "%" << endl;
    }
    cout << "Wasted Democratic votes: " << State.totalDemWaste << endl;
    cout << "Wasted Republican votes: " << State.totalRepWaste << endl;
    cout << "Eligible Voters: " << State.eligVoters << endl
         << endl;
}

// Outputs histogram of 100 characters. D's, and R's, each represeting the parties. 
// A visual representation of party share of votes for the whole state.
// Takes State struct, returns nothing as it's void. 
void plot(state State)
{
    cout << endl;
    for (auto district : State.districts)
    {
        // Floor to round down interger. D precent is percentage of Dem votes. 
        int Dpercent = floor(100 * float(district.democrat) / (district.democrat + district.republican));
        int Rpercent = 100 - Dpercent;

        cout << "District: " << district.number << endl;
        for (int i = 0; i < Dpercent; i++)
        {
            cout << "D";
        }
        for (int i = 0; i < Rpercent; i++)
        {
            cout << "R";
        }
        cout << endl;
    }
    cout << endl;
}

// Lowercases whole line with tolower string method iterating a whole string literal.
// Takes string, returns string that is lowercased.
string lowerLine(string line)
{
    string newLine;
    for (auto c : line)
    {
        newLine += tolower(c);
    }
    return newLine;
}

// Ensures that search function isn't case sensitive.
// Takes a string, Uppercases first letter, lowercases the rest, and returns that string. 
string caseSense(string line)
{
    line = line.substr(1);
    string newString = "";
    string newLine;
    while (line.find(' ') != string::npos)
    {
        char up = toupper(line[0]);
        newLine = up + lowerLine(line.substr(1, line.find(' ')));
        newString += newLine;
        line = line.substr(line.find(' ') + 1);
    }

    // if there is only one word, but also ensures that the last word of a multiword line is read and case corrected.
    if (line.size())
    {
        char up = toupper(line[0]);
        newLine = up + lowerLine(line.substr(1, line.find(' ')));
        newString += newLine;
    }

    return newString;
}

// Searches state in list vector, and returns that state struct to be used for stats/plot commands. 
// Parameters:
//  searchState is a string representing state to be serached.
//  aState is passbyref state struct that will be filled by searched state's info. 
//  list is the list vector taken from the run function that contains relevant data.
//  name is a string. Is the name of state. 
void search(string searchState, state &aState, vector<state> &list, string &name)
{

    searchState = caseSense(searchState);
    
    for (auto state : list)
    {
        if (state.name == searchState)
        {
            name = state.name;
            aState = state;
            return;
        }
    }
    cout << "\nState does not exist, search again.\n\n";
}

// Add's eligible voters number to state struct.
// Takes list vector from run function as passbyref and line read from eligible voters file.
// Returns void.
void addElig(vector<state> &list, string line)
{
    for (int i = 0; i < list.size(); i++)
    {
        if (list.at(i).name == line.substr(0, line.find(',')))
        {
            list.at(i).eligVoters = stoi(line.substr(line.find(',') + 1));

            cout << "..." << list.at(i).name << "..."
                 << list.at(i).eligVoters << " eligible voters\n";
        }
    }
}

// Reads a single line from districts voters file and returns state struct to be added to list vector. 
// Takes string line, returns state struct.
state read(string line)
{
    state aState;
    district aDistrict;
    aState.name = line.substr(0, line.find(','));
    line = line.substr(line.find(',') + 1);

    // ct is used to determine if the function is reading the district number, the rep votes, or the dem votes.
    // if ct 1, its the district num. if 2, the democrat votes, if 3, the repulican votes. Then it resets back to 1 and repeats.
    int ct = 1;

    // Reads every word using comma positions. 
    while (line.find(',') != string::npos)
    {
        if (ct == 1)
        {
            if (line.substr(0, line.find(',')) == "AL")
            {
                aDistrict.number = 1;
            }
            else
                aDistrict.number = stoi(line.substr(0, line.find(',')));
        }
        if (ct == 2)
        {
            aDistrict.democrat = stoi(line.substr(0, line.find(',')));
        }
        if (ct == 3)
        {
            aDistrict.republican = stoi(line.substr(0, line.find(',')));
            aState.districts.push_back(aDistrict);
            ct = 0;
        }
        line = line.substr(line.find(',') + 1);
        ct++;
    }

    aDistrict.republican = stoi(line);
    aState.districts.push_back(aDistrict);

    return aState;
}

// Loads and reads both files. Takes name of files as strings, string loaded to display whether or not
// the files are loaded in the input loop, and bool firslLoaded to determine wheter or not JUST the first
// file was loaded. 
// Returns vector of states, the outermost vector for everything needed for this program. 
vector<state> load(string distFile, string eligFile, string &loaded, bool &firstLoaded)
{
    string line;
    vector<state> list;

    ifstream district(distFile);
    if (!district.is_open())
    {
        cout << "Invalid first file, try again.\n\n";
        return list;
    }
    cout << "Reading: " << distFile << endl;
    while (getline(district, line))
    {
        list.push_back(read(line));
        cout << "..." << list.at(list.size() - 1).name << "..." << list.at(list.size() - 1).districts.size() << " districts total\n";
    }
    cout << endl;
    district.close();
    ifstream eligible(eligFile);
    if (!eligible.is_open())
    {
        cout << "Invalid second file, try again.\n\n";
        firstLoaded = true;
        return list;
    }
    cout << "\nReading: " << eligFile << endl;

    while (getline(eligible, line))
    {
        addElig(list, line);
    }
    eligible.close();
    cout << endl;
    loaded = "Yes";
    return list;
}

// This is where the user sees outputs and inputs. Is void. 
void run()
{
    cout << "Welcome to the Gerrymandering app!\n\n";
    vector<state> list;
    state searchedState;
    string stateName = "N/A";
    string cmd1;
    string cmd2;
    string cmd3;
    string eFile;
    string loaded = "No";
    bool firstLoaded;
    while (true)
    {
        cout << "Data loaded? " << loaded << endl;
        cout << "State: " << stateName << endl
             << endl;
        cout << "Enter command: ";
        cin >> cmd1;
        cout << "\n-----------------------------\n\n";
        if (cmd1 == "load")
        {
            cin >> cmd2 >> cmd3;
            // cmd2 = "districts.txt", cmd3 = "eligible_voters.txt";
            if (loaded != "Yes")
                list = load(cmd2, cmd3, loaded, firstLoaded);
            else
                cout << "Already read data in, exit and start over.\n\n";
        }
        if (cmd1 == "search")
        {
            cout << endl;
            getline(cin, cmd2);
            if (!firstLoaded && loaded == "No")
                cout << "No data loaded, please load data first.\n\n";
            else
                search(cmd2, searchedState, list, stateName);
        }
        if (cmd1 == "stats")
        {
            if (stateName == "N/A" && loaded == "Yes")
                cout << "No state indicated, please search for state first.\n\n";
            if (loaded == "Yes" && stateName != "N/A")
                stats(searchedState);
            if (loaded == "No")
                cout << "No data loaded, please load data first.\n\n";
        }
        if (cmd1 == "plot")
        {
            if (loaded == "No")
                cout << "No data loaded, please load data first.\n\n";
            else
                plot(searchedState);
        }
        if (cmd1 == "exit")
        {
            break;
        }
    }
}

int main()
{
    run();
}