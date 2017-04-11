//
//  main.cpp
//  CPolynom
//
//  Created by Adam Zvada on 26.03.17.
//  Copyright © 2017 Adam Zvada. All rights reserved.
//

#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <complex>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

#endif /* __PROGTEST__ */

#define EPSILON 1e-6

ios_base & dummy_polynomial_manipulator ( ios_base & x )
{
    return x;
}

ios_base & ( * ( polynomial_variable ( const string & varName ) ) ) ( ios_base & x )
{
    return dummy_polynomial_manipulator;
}


class CPolynomial {
public:
    CPolynomial();
    CPolynomial(const CPolynomial & obj);
    
    double & operator [] (const unsigned int index);
    const double operator [] (const unsigned int index) const;
    
    const double operator()(const double x) const;
    
    bool operator == (const CPolynomial & p) const;
    bool operator != (const CPolynomial & p) const;
    
    //void operator = (const int index);
    
    CPolynomial operator + (const CPolynomial & b) const;
    CPolynomial operator - (const CPolynomial & b) const;
    
    CPolynomial operator * (const CPolynomial & b) const;
    CPolynomial operator * (const double c) const;

    unsigned long Degree() const;
    
    friend ostream & operator << (ostream & os, const CPolynomial & p);
private:
    
    vector<double> _coefficients;
    
    bool _areDoublesEqual(double a, double b) const;
    
    unsigned long _getMax(const unsigned long a, const unsigned long b) const;
    unsigned long _getMin(const unsigned long a, const unsigned long b) const;
};

CPolynomial::CPolynomial() {
    _coefficients.resize(10, 0);
}

CPolynomial::CPolynomial(const CPolynomial & obj){
    for (unsigned int i = 0; i < obj._coefficients.size(); i++) {
        this->_coefficients.push_back(obj._coefficients[i]);
    }
}

double & CPolynomial::operator [] (const unsigned int index) {
    
    if (index >= _coefficients.size()) {
        cout << "Resize on " << index + 1 << endl;
        _coefficients.resize(index + 1, 0);
    }
    
    return _coefficients[index];
}

const double CPolynomial::operator [] (const unsigned int index) const {
    
    try {
        _coefficients.at(index);
    } catch (...) {
        return 0;
    }
    
    return _coefficients.at(index);
}

const double CPolynomial::operator()(const double x) const {
    
    double funcVal = 0;
    
    for (unsigned int i = 0; i <= (*this).Degree(); i++) {
        funcVal += (*this)[i] * pow(x, i);
    }
 
    return funcVal;
}

//void CPolynomial::operator = (int coef) {
//    
//}

bool CPolynomial::operator == (const CPolynomial & p) const {
    
    for (unsigned int i = 0; i < _coefficients.size(); i++) {
        if (!_areDoublesEqual(_coefficients[i], p._coefficients[i])) {
            return false;
        }
    }
    
    return true;
}

bool CPolynomial::operator != (const CPolynomial & p) const {
    
    for (unsigned int i = 0; i < _coefficients.size(); i++) {
        if (!_areDoublesEqual(_coefficients[i], p._coefficients[i])) {
            return true;
        }
    }

    return false;
}

CPolynomial CPolynomial::operator + (const  CPolynomial &  b) const {
    CPolynomial newPolynomail;
    
    unsigned long maxDegree = _getMax(this->Degree(), b.Degree());
    
    for (unsigned int i = 0; i <= maxDegree; i++) {
        newPolynomail[i] = (*this)[i] + b[i];
    }
    
    return newPolynomail;
}


CPolynomial CPolynomial::operator - (const  CPolynomial &  b) const {
    CPolynomial newPol;
    
    unsigned long maxDegree = _getMax(this->Degree(), b.Degree());
    
    for (unsigned int i = 0; i <= maxDegree; i++) {
        newPol[i] = (*this)[i] - b[i];
    }
    
    return newPol;
}

CPolynomial CPolynomial::operator * (const CPolynomial & b) const {
    CPolynomial newPol;
    
    for (unsigned int i = 0; i <= (*this).Degree(); i++) {
        for (unsigned int j = 0; j <= b.Degree(); j++) {
            newPol[i + j] += (*this)[i] * b[j];
        }
    }
    
    return newPol;
}

CPolynomial CPolynomial::operator * (const double c) const {
    CPolynomial newPol;
    for (unsigned int i = 0; i <= this->Degree(); i++) {
        //(*this)[i] = (*this)[i] * c;
        newPol[i] = (*this)[i] * c;
    }
    
    return newPol;;
}


unsigned long CPolynomial::Degree() const {
    for (unsigned long i = _coefficients.size() - 1; i > 0; i--) {
        if (_coefficients[i] != 0 ) {
            return i;
        }
    }
    return 0;
}

ostream & operator << (ostream & os, const CPolynomial & p) {
    
    unsigned long curDegree = p._coefficients.size() - 1;
    bool flagWrite = false;
    for (auto it = p._coefficients.rbegin(); it != p._coefficients.rend(); it++) {
       
        if ((*it) != 0) {
            
            if (*it > 0 && flagWrite) {
                os << " + ";
            } else if (*it < 0){
                if (!flagWrite) {
                    os << "- ";
                } else {
                    os << " - ";
                }
            }
            
            if(*it > 1 ) {
                if(curDegree >= 1) {
                    os << (*it) << "*" << "x^" << curDegree;
                } else {
                    os << (*it);
                }
            } else if(*it < -1) {
                if(curDegree >= 1) {
                    os << fabs((*it)) << "*" << "x^" << curDegree;
                } else {
                    os << fabs((*it));
                }

            } else if(*it == 1 || *it == -1 ) {
                if(curDegree >= 1) {
                    os << "x^" << curDegree;
                } else {
                    os << fabs((*it));
                }

            }
            flagWrite = true;
        }
        
        curDegree--;
    }
    
    if (!flagWrite) {
        os << "0";
    }
    
    return os;
}

bool CPolynomial::_areDoublesEqual(double a, double b) const {
    return fabs(a - b) < EPSILON;
}

unsigned long CPolynomial::_getMax(const unsigned long a, const unsigned long b) const {
    return a > b ? a : b;
}

unsigned long CPolynomial::_getMin(const unsigned long a, const unsigned long b) const {
    return a > b ? b : a;
}

#ifndef __PROGTEST__
bool smallDiff(double a, double b) {
    return fabs(a - b) < EPSILON;
}

bool dumpMatch(const CPolynomial & x, const vector<double> & ref) {
 
    //TODO
    
    return true;
}

int main(void) {
    CPolynomial a, b, c;
    ostringstream out;
    
    a[0] = -10;
    a[1] = 3.5;
    a[3] = 1;
    
    cout << a(2) << endl;
    assert ( smallDiff ( a ( 2 ), 5 ) );
    out . str ("");
    out << a;
    assert ( out . str () == "x^3 + 3.5*x^1 - 10" );
    a = a * -2;
    cout << a.Degree() << endl;
    assert ( a . Degree () == 3
            && dumpMatch ( a, vector<double>{ 20.0, -7.0, -0.0, -2.0 } ) );
    
    out . str ("");
    out << a;
    cout << out.str() << endl;
    assert ( out . str () == "- 2*x^3 - 7*x^1 + 20" );
    out . str ("");
    out << b;
    cout << out.str() << endl;
    assert ( out . str () == "0" );
    b[5] = -1;
    out . str ("");
    out << b;
    cout << out.str() << endl;
    assert ( out . str () == "- x^5" );
    c = a + b;
    cout << c.Degree() << endl;
    assert ( c . Degree () == 5
            && dumpMatch ( c, vector<double>{ 20.0, -7.0, 0.0, -2.0, 0.0, -1.0 } ) );
    
    out . str ("");
    out << c;
    cout << out.str() << endl;
    assert ( out . str () == "- x^5 - 2*x^3 - 7*x^1 + 20" );
    c = a - b;
    assert ( c . Degree () == 5
            && dumpMatch ( c, vector<double>{ 20.0, -7.0, -0.0, -2.0, -0.0, 1.0 } ) );
    
    out . str ("");
    out << c;
    cout << out.str() << endl;
    assert ( out . str () == "x^5 - 2*x^3 - 7*x^1 + 20" );
    c = a * b;
    cout << c.Degree() << endl;
    assert ( c . Degree () == 8
            && dumpMatch ( c, vector<double>{ 0.0, -0.0, 0.0, -0.0, -0.0, -20.0, 7.0, 0.0, 2.0 } ) );
    
    out . str ("");
    out << c;
    cout << out.str() << endl;
    assert ( out . str () == "2*x^8 + 7*x^6 - 20*x^5" );
    assert ( a != b );
    b[5] = 0;
    assert ( !(a == b) );
    a = a * 0;
    assert ( a . Degree () == 0
            && dumpMatch ( a, vector<double>{ 0.0 } ) );
    
    assert ( a == b );
    
    CPolynomial p;
    p[1000] = 120000;
    p[0] = 1;
    out . str();
    cout << p << endl;
    
    //bonus
    a[2] = 4;
    a[1] = -3;
    b[3] = 7;
    out . str ("");
    out << polynomial_variable ( "y" ) << "a=" << a << ", b=" << b;
    assert ( out . str () == "a=4*y^2 - 3*y^1, b=7*y^3" );
    
    out . str ("");
    out << polynomial_variable ( "test" ) << c;
    assert ( out . str () == "2*test^8 + 7*test^6 - 20*test^5" );
    
    return 0;
}
#endif /* __PROGTEST__ */
