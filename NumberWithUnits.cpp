#include "NumberWithUnits.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <cstdlib>
using namespace std;

namespace ariel{

    // EPS for operator == between doubles.
    const double EPS=0.0001;

    // declaring static members of class.
    vector<vector<double>> NumberWithUnits::unit_graph;
    vector<string> NumberWithUnits::units;
    unordered_map<string,unsigned int> NumberWithUnits::mat_indexes;

    NumberWithUnits::NumberWithUnits(double num,const string& unit)
    {
        if(mat_indexes.find(unit)==mat_indexes.end()){
            throw out_of_range{"NumberWithUnits Class Error: Initialized with unrecognized unit variable."};
        }
        this->num=num;
        this->unit=unit;
    }

    NumberWithUnits::~NumberWithUnits(){}

    unsigned int skip_spaces(string line,unsigned int index)
    {
        while(index<(unsigned int)line.length() &&(line.at(index)==' ' || line.at(index)=='\r' || line.at(index)=='\n' 
               || line.at(index)=='\t')){ (index)++; }
        return index;
    }

    string copy_unit(string line,unsigned int index)
    {
        string unit;
        while(index<(unsigned int)line.length() && line.at(index)!=' ' && line.at(index)!='\r' && line.at(index)!='\n' 
              && line.at(index)!='\t' && line.at(index)!='=')
            {
             unit+=line.at(index++);
             }
        return unit;
    }

    string copy_ratio(string line,unsigned int index)
    {
        string unit;
        while((line.at(index)>='0' && line.at(index)<='9' && index<(unsigned int)line.length()) || line.at(index)=='.')
            {
             unit+=line.at(index++);
             }
        return unit;
    }
    void NumberWithUnits::update(ifstream& file){
        string line;
        while(getline(file,line))
        {
            if (line.length() < 1){ return; }
            string first_unit;
            string second_unit;
            string ratio_str;
            double ratio=0;
            unsigned int index=0;
            index=skip_spaces(line,index);
            if(line.at(index++)!='1')
            {
                throw out_of_range{"Text format is wrong!\n"};
            }
            index=skip_spaces(line,index);
            first_unit=copy_unit(line,index);
            index+=(unsigned int)first_unit.length();
            index=skip_spaces(line,index);
            if(line.at(index++)!='=')
            {                        
                throw out_of_range{"Text format is wrong!\n"};
            }
            index=skip_spaces(line,index);
            ratio_str=copy_ratio(line,index);
            index+=(unsigned int)ratio_str.length();
            std::string::size_type sz=0;
            ratio = stod (ratio_str,&sz);
            index=skip_spaces(line,index);
            second_unit=copy_unit(line,index);
            // unit_table[first_unit][second_unit]=ratio;
            // unit_table[second_unit][first_unit]=1/ratio;
            if(mat_indexes.find(first_unit)==mat_indexes.end()&&mat_indexes.find(second_unit)==mat_indexes.end())
            {
                vector<double> v_first;
                vector<double> v_second;
                unit_graph.push_back(v_first);
                unit_graph.push_back(v_second);
                mat_indexes[first_unit]=units.size();
                units.push_back(first_unit);
                mat_indexes[second_unit]=units.size();
                units.push_back(second_unit);
                for (unsigned int i = 0; i < units.size()-2; i++)
                {
                    unit_graph.at(i).push_back(0);
                    unit_graph.at(i).push_back(0);
                }
                for (unsigned int i = 0; i < units.size(); i++)
                {
                    unit_graph.at(units.size()-2).push_back(0);
                    unit_graph.at(units.size()-1).push_back(0);
                }
                unit_graph.at(units.size()-2).at(units.size()-2)=1;
                unit_graph.at(units.size()-1).at(units.size()-1)=1;
                unit_graph.at(units.size()-2).at(units.size()-1)=ratio;
                unit_graph.at(units.size()-1).at(units.size()-2)=1/ratio;
            }
            else if(mat_indexes.find(first_unit)==mat_indexes.end())
            {
                vector<double> v_first;
                unit_graph.push_back(v_first);
                mat_indexes[first_unit]=units.size();
                units.push_back(first_unit);
                for (unsigned int i = 0; i < units.size()-1; i++)
                {
                    unit_graph.at(i).push_back(0);
                }
                for (unsigned int i = 0; i < units.size(); i++)
                {
                    unit_graph.at(units.size()-1).push_back(0);
                }
                unit_graph.at(units.size()-1).at(units.size()-1)=1;
                unit_graph.at(units.size()-1).at(mat_indexes[second_unit])=ratio;
                unit_graph.at(mat_indexes[second_unit]).at(units.size()-1)=1/ratio;
            }
            else if(mat_indexes.find(second_unit)==mat_indexes.end())
            {
                vector<double> v_second;
                unit_graph.push_back(v_second);
                mat_indexes[second_unit]=units.size();
                units.push_back(second_unit);
                for (unsigned int i = 0; i < units.size()-1; i++)
                {
                    unit_graph.at(i).push_back(0);
                }
                for (unsigned int i = 0; i < units.size(); i++)
                {
                     unit_graph.at(units.size()-1).push_back(0);
                }
                unit_graph.at(units.size()-1).at(units.size()-1)=1;
                unit_graph.at(mat_indexes[first_unit]).at(units.size()-1)=ratio;
                unit_graph.at(units.size()-1).at(mat_indexes[first_unit])=1/ratio;
            }
            else
            {
                unit_graph.at(mat_indexes[first_unit]).at(mat_indexes[second_unit])=ratio;
                unit_graph.at(mat_indexes[second_unit]).at(mat_indexes[first_unit])=1/ratio;
            }
        }
        file.close();
        // filling mat according to FW graph algorithm.
        for (unsigned int k = 0;  k < units.size(); k++)
        {
            for (unsigned int i = 0; i < units.size(); i++)
            {   
                for (unsigned int j = i+1; j < units.size(); j++)
                {
                    if(unit_graph.at(i).at(j)==0 && unit_graph.at(i).at(k)!=0 &&
                       unit_graph.at(k).at(j)!=0 && k!=i && k!=j)
                    {
                       unit_graph.at(i).at(j)=unit_graph.at(i).at(k)*unit_graph.at(k).at(j);
                       unit_graph.at(j).at(i)=1/unit_graph.at(i).at(j);
                    }
                }
            }
        }
    }
    void NumberWithUnits::read_units(ifstream& file)
    {
        if(units.size()>1){
            update(file);
            return;
        }        
        string line;
        unordered_map<string,unordered_map<string,double>> unit_table;
        while(getline(file,line))
        {
            if (line.length() < 1){ return; }
            string first_unit;
            string second_unit;
            string ratio_str;
            double ratio=0;
            unsigned int index=0;
            index=skip_spaces(line,index);
            if(line.at(index++)!='1')
            {
                throw out_of_range{"Text format is wrong!\n"};
            }
            index=skip_spaces(line,index);
            first_unit=copy_unit(line,index);
            index+=(unsigned int)first_unit.length();
            index=skip_spaces(line,index);
            if(line.at(index++)!='=')
            {                        
                throw out_of_range{"Text format is wrong!\n"};
            }
            index=skip_spaces(line,index);
            ratio_str=copy_ratio(line,index);
            index+=(unsigned int)ratio_str.length();
            std::string::size_type sz=0;
            ratio = stod (ratio_str,&sz);
            index=skip_spaces(line,index);
            second_unit=copy_unit(line,index);
            unit_table[first_unit][second_unit]=ratio;
            unit_table[second_unit][first_unit]=1/ratio;
        }
        file.close();
        unordered_map<string,unordered_map<string,double>>:: iterator itr;
        unsigned int size=0;
        for (itr = unit_table.begin(); itr != unit_table.end(); itr++)
        {
            vector<double> v;
            units.push_back(itr->first);
            unit_graph.push_back(v);
            mat_indexes[itr->first]=size;
            size++;
        } 
        // filling mat with 0's and 1's at the main diagonal.
        for (unsigned int i = 0; i < size; i++)
        {  
            for (unsigned int j = 0; j < size; j++)
            {
                if(i==j){
                    unit_graph.at(i).push_back(1);
                }
                else
                {
                    unit_graph.at(i).push_back(0);
                } 
            }
        }
        // updating mat according to neighbors.
        for (unsigned int i = 0; i < size; i++)
        {   
            for (unsigned int j = i+1; j < size; j++)
            {
                if(unit_table[units.at(i)].find(units.at(j))!=unit_table[units.at(i)].end())
                {
                    unit_graph.at(i).at(j)=unit_table[units.at(i)][units.at(j)];
                    unit_graph.at(j).at(i)=1/unit_graph.at(i).at(j);
                }
            }
        }
        // filling mat according to FW graph algorithm.
        for (unsigned int k = 0;  k < size; k++)
        {
            for (unsigned int i = 0; i < size; i++)
            {   
                for (unsigned int j = i+1; j < size; j++)
                {
                    if(unit_graph.at(i).at(j)==0 && unit_graph.at(i).at(k)!=0 &&
                       unit_graph.at(k).at(j)!=0 && k!=i && k!=j)
                    {
                       unit_graph.at(i).at(j)=unit_graph.at(i).at(k)*unit_graph.at(k).at(j);
                       unit_graph.at(j).at(i)=1/unit_graph.at(i).at(j);
                    }
                }
            }
        }
    }

    NumberWithUnits NumberWithUnits::operator+(const NumberWithUnits& other) const
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        return NumberWithUnits{this->num+other_size_this_unit,this->unit};
    }

    NumberWithUnits& NumberWithUnits::operator+=(const NumberWithUnits& other)
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};       
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        this->num+=other_size_this_unit;
        return *this;
    }

    NumberWithUnits NumberWithUnits::operator+() const
    {
        return NumberWithUnits{this->num,this->unit};        
    }

    NumberWithUnits NumberWithUnits::operator-() const
    {
        return NumberWithUnits{-(this->num),this->unit};        
    }

    NumberWithUnits NumberWithUnits::operator-(const NumberWithUnits& other) const
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        return NumberWithUnits{this->num-other_size_this_unit,this->unit};
    }

    NumberWithUnits& NumberWithUnits::operator-=(const NumberWithUnits& other)
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        this->num-=other_size_this_unit;
        return *this;
    }

    bool NumberWithUnits::operator==(const NumberWithUnits& other) const
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        return abs(this->num-other_size_this_unit)<EPS;
    }

    bool NumberWithUnits::operator>=(const NumberWithUnits& other) const
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        return this->num>=other_size_this_unit;
    }

    bool NumberWithUnits::operator<=(const NumberWithUnits& other) const
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        return this->num<=other_size_this_unit;
    }

    bool NumberWithUnits::operator!=(const NumberWithUnits& other) const
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        return abs(this->num-other_size_this_unit)>EPS;
    }

    bool NumberWithUnits::operator<(const NumberWithUnits& other) const
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        return this->num < other_size_this_unit;
    }

    bool NumberWithUnits::operator>(const NumberWithUnits& other) const
    {
        if(unit_graph.at(mat_indexes[this->unit]).at(mat_indexes[other.unit])==0)
        {
            throw out_of_range{"Units do not match - ["+other.unit+"] cannot be converted to ["+this->unit+"] "};
        }
        double other_size_this_unit=unit_graph.at(mat_indexes[other.unit]).at(mat_indexes[this->unit])*other.num;
        return this->num > other_size_this_unit;
    }

    NumberWithUnits& NumberWithUnits::operator++()
    {
        (this->num)++;
        return *this;
    }

    NumberWithUnits NumberWithUnits::operator++(int dummy_flag_for_postfix_increment)
    {
        NumberWithUnits temp{this->num,this->unit};
        (this->num)++;
        return temp;
    }

    NumberWithUnits& NumberWithUnits::operator--()
    {
        (this->num)--;       
        return *this;
    }

    NumberWithUnits NumberWithUnits::operator--(int dummy_flag_for_postfix_increment)
    {
        NumberWithUnits temp{this->num,this->unit};
        (this->num)--;
        return temp;
    }

    NumberWithUnits operator*(double r, const NumberWithUnits& n)
    {
        return NumberWithUnits{n.num*r,n.unit};
    }
    NumberWithUnits operator*(const NumberWithUnits& n,double r)
    {
        return NumberWithUnits{n.num*r,n.unit};
    }
    ostream& operator<< (std::ostream& output, const NumberWithUnits& n)
    {
        return (output << n.num << '[' << n.unit << ']');
    }

    static istream& getAndCheckNextCharIs(istream& input, char expectedChar)
    {
        char actualChar=' ';
        input >> actualChar;
        if (!input)
        {
            return input;
        }
        if (actualChar!=expectedChar) 
        {
            input.setstate(ios::failbit);
        }
        return input;
    }
    istream& operator>> (std::istream& input , NumberWithUnits& n)
    {
        double num=0;
        string unit;
        ios::pos_type startPosition = input.tellg();    
        if ( (!(input >> num))                    ||
            (!getAndCheckNextCharIs(input,'['))   ||
            (!(input >> unit))                    ||
            (unit.at(unit.length()-1)!=']'        &&
            (!getAndCheckNextCharIs(input,']'))))
            {
            // rewind on error
            auto errorState = input.rdstate(); // remember error state
            input.clear(); // clear error so seekg will work
            input.seekg(startPosition); // rewind
            input.clear(errorState); // set back the error flag  
        }
        else
        {
            if (unit.at(unit.length()-1)==']')
            {
                unit=unit.substr(0,unit.length()-1);
            }
            if(NumberWithUnits::mat_indexes.find(unit)==NumberWithUnits::mat_indexes.end()){
                throw out_of_range{"This unit does not exist!"};
            }
            n.num=num;
            n.unit=unit;
        }
        return input;
    }
};