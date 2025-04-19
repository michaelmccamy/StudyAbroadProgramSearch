using namespace std;

#ifndef FINALPROJECTDSA_PROGRAMS_H
#define FINALPROJECTDSA_PROGRAMS_H

// this class allows us to store all the information needed for each study abroad program
class studyAbroadProgram{
public:
    string name;
    string city;
    string country;
    string region;
    string term;
    string gpa;
    string advisor;
    string college;
    string credits;
    string cost;
    string sapa;
    string about;

    // constructor
    studyAbroadProgram(string name, string city, string country, string region, string term, string gpa, string advisor, string college, string credits, string cost, string sapa, string about){
        this->name = name;
        this->city = city;
        this->country = country;
        this->region = region;
        this->term = term;
        this->gpa = gpa;
        this->advisor = advisor;
        this->college = college;
        this->credits = credits;
        this->cost = cost;
        this->sapa = sapa;
        this->about = about;
    }

};
#endif
