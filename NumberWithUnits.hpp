#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
namespace ariel
{

    class NumberWithUnits
    {

        static vector<vector<double>> unit_graph;
        static vector<string> units;
        static unordered_map<string,unsigned int> mat_indexes;
        double num;
        string unit;

        public:
        
            NumberWithUnits(double num,const string& unit);
            ~NumberWithUnits();
            static void update(ifstream& file);
            static void read_units(ifstream& file);
            NumberWithUnits operator+(const NumberWithUnits& other) const;
            NumberWithUnits& operator+=(const NumberWithUnits& other);
            NumberWithUnits operator+() const;
            NumberWithUnits operator-() const;
            NumberWithUnits operator-(const NumberWithUnits& other) const;
            NumberWithUnits& operator-=(const NumberWithUnits& other);
            bool operator==(const NumberWithUnits& other) const;
            bool operator>=(const NumberWithUnits& other) const;
            bool operator<=(const NumberWithUnits& other) const;
            bool operator!=(const NumberWithUnits& other) const;
            bool operator<(const NumberWithUnits& other) const;
            bool operator>(const NumberWithUnits& other) const;
            friend std::ostream& operator<< (std::ostream& output, const NumberWithUnits& n);
            friend std::istream& operator>> (std::istream& input , NumberWithUnits& n);
            NumberWithUnits& operator++();
            NumberWithUnits operator++(int dummy_flag_for_postfix_increment);
            NumberWithUnits& operator--();
            NumberWithUnits operator--(int dummy_flag_for_postfix_increment);
            friend NumberWithUnits operator*(const NumberWithUnits& n,double r);
            friend NumberWithUnits operator*(double r,const NumberWithUnits& n);
    };
};