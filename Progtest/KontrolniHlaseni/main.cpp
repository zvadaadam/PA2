
#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>

using namespace std;

#endif /* __PROGTEST__ */


class CVATRegister {
public:
    CVATRegister(void);
    ~CVATRegister(void);
    
    bool NewCompany(const string & name, const string & addr, const string & taxID);
    
    bool CancelCompany(const string & name, const string & addr);
    bool CancelCompany(const string & taxID);
    
    bool Invoice(const string & taxID, unsigned int amount);
    bool Invoice(const string & name, const string & addr, unsigned int amount);
    
    bool Audit(const string & name, const string & addr, unsigned int & sumIncome ) const;
    bool Audit(const string & taxID, unsigned int & sumIncome ) const;
    
    unsigned int MedianInvoice(void) const;
    
    void print() const;
    
    void printHeap() const;
    
private:
    
    struct Company {
        pair<string, string> nameAndAddr;
        
        string name;
        string addr;
        
        string taxID;
        
        unsigned int sum;
        
        Company(const string name, const string addr, const string taxID);
    };
    
    vector<unsigned int> _invoice;
    vector<unsigned int> _maxInvoice;
    vector<unsigned int> _minInvoice;
    
    vector<Company*> _idTax;
    vector<Company*> _namesCompanies;
    
    unsigned int _numInvoices;
    
    bool _insertByIdTax(Company * company);
    bool _insertByNamesCompanies(Company * company);
    
    bool _keepsItUniqueIdTax(Company * company) const;
    bool _keepsItUniqueNamesCompanies(Company * company) const;
    
    bool _companyExists(const string & taxID) const;
    bool _companyExists(const string & name, const string & addr) const;
    
    bool _addToMaxMinHeap(unsigned int amount);
    
    unsigned int _getMedian() const;
    
    static string toLower(const string & a);
    
    static bool _compareTax(const Company * a, const Company * b);
    static bool _compareNameCompany(const Company * a, const Company * b);
    
    static bool _compareMinHeap(const unsigned int & a, const unsigned int & b);
    
};

CVATRegister::CVATRegister() {
    this->_numInvoices = 0;
}

CVATRegister::~CVATRegister() {
    vector<Company*>::iterator it;
    for (it = _idTax.begin(); it != _idTax.end(); it++) {
        delete (*it);
    }
}

CVATRegister::Company::Company(const string name, const string addr, const string taxID) {
    
    this->taxID = taxID;
    
    this->name = toLower(name);
    this->addr = toLower(addr);;
    
    this->nameAndAddr.first = toLower(name);
    this->nameAndAddr.second = toLower(addr);
    
    this->sum = 0;
}

bool CVATRegister::NewCompany(const string & name, const string & addr, const string & taxID) {
    
    Company * company = new Company(name, addr, taxID);
    
    if (!(_keepsItUniqueIdTax(company) && _keepsItUniqueNamesCompanies(company))) {
        return false;
    }
    
    if(!_insertByIdTax(company)) {
        return false;
    }
    
    if(!_insertByNamesCompanies(company)) {
        return false;
    }
    
    return true;
}

bool CVATRegister::CancelCompany(const string & name, const string & addr) {
    
    if (!_companyExists(name, addr)) {
        return false;
    }
    
    //clear way, make pair of (name,addr) a create new compare func
    Company * tmpCompanyName = new Company(name, addr, "FOO");
    vector<Company*>::iterator itName = lower_bound(_namesCompanies.begin(), _namesCompanies.end(), tmpCompanyName, _compareNameCompany);
    delete tmpCompanyName;
    
    Company * tmpCompanyTax = new Company("FOO", "FOO", (*itName)->taxID);
    vector<Company*>::iterator itTax = lower_bound(_idTax.begin(), _idTax.end(), tmpCompanyTax, _compareTax);
    delete tmpCompanyTax;
    
    
    delete (*itName);
    _namesCompanies.erase(itName);
    _idTax.erase(itTax);
    
    return true;
}


bool CVATRegister::CancelCompany(const string & taxID) {
    
    if (!_companyExists(taxID)) {
        return false;
    }
    
    Company * tmpCompanyTax = new Company("FOO", "FOO", taxID);
    vector<Company*>::iterator itTax = lower_bound(_idTax.begin(), _idTax.end(), tmpCompanyTax, _compareTax);
    delete tmpCompanyTax;
    
    Company * tmpCompanyName = new Company((*itTax)->name, (*itTax)->addr, "FOO");
    vector<Company*>::iterator itName = lower_bound(_namesCompanies.begin(), _namesCompanies.end(), tmpCompanyName, _compareNameCompany);
    delete tmpCompanyName;
    
    delete (*itTax);
    
    _idTax.erase(itTax);
    _namesCompanies.erase(itName);
    
    return true;
}


bool CVATRegister::Invoice(const string & taxID, unsigned int amount) {
    
    if (!_companyExists(taxID)) {
        return false;
    }
    
    Company * tmpCompany = new Company("FOO", "FOO", taxID);
    vector<Company*>::iterator it = lower_bound(_idTax.begin(), _idTax.end(), tmpCompany, _compareTax);
    delete tmpCompany;
    
    (*it)->sum += amount;
    _invoice.push_back(amount);
    
    _addToMaxMinHeap(amount);
    
    
    return true;
}


bool CVATRegister::Invoice(const string & name, const string & addr, unsigned int amount) {
    
    if (!_companyExists(name, addr)) {
        return false;
    }
    
    Company * tmpCompany = new Company(name, addr, "FOO");
    vector<Company*>::iterator it = lower_bound(_namesCompanies.begin(), _namesCompanies.end(), tmpCompany, _compareNameCompany);
    delete tmpCompany;
    
    
    (*it)->sum += amount;
    _invoice.push_back(amount);
    
    _addToMaxMinHeap(amount);
    
    return true;
}


bool CVATRegister::Audit(const string & name, const string & addr, unsigned int & sumIncome ) const {
    
    if (!_companyExists(name, addr)) {
        return false;
    }
    
    //clear way, make pair of (name,addr) a create new compare func
    Company * tmpCompany = new Company(name, addr, "FOO");
    vector<Company*>::const_iterator it = lower_bound(_namesCompanies.cbegin(), _namesCompanies.cend(), tmpCompany, _compareNameCompany);
    delete tmpCompany;
    
    sumIncome = (*it)->sum;
    
    return true;
}


bool CVATRegister::Audit(const string & taxID, unsigned int & sumIncome ) const {
    
    if (!_companyExists(taxID)) {
        return false;
    }
    
    Company * tmpCompany = new Company("FOO", "FOO", taxID);
    vector<Company*>::const_iterator it = lower_bound(_idTax.cbegin(), _idTax.cend(), tmpCompany, _compareTax);
    delete tmpCompany;
    
    sumIncome = (*it)->sum;
    
    return true;
}


unsigned int CVATRegister::MedianInvoice(void) const {
    
    return _getMedian();
}


bool CVATRegister::_keepsItUniqueIdTax(Company * company) const {
    
    if(binary_search(_idTax.begin(), _idTax.end(), company, _compareTax)) {
        return false;
    }
    
    return true;
}

bool CVATRegister::_keepsItUniqueNamesCompanies(Company * company) const {
    
    if(binary_search(_namesCompanies.begin(), _namesCompanies.end(), company, _compareNameCompany)) {
        return false;
    }
    
    return true;
}

bool CVATRegister::_companyExists(const string & taxID) const {
    
    Company * tmpCompany = new Company("FOO", "FOO", taxID);
    if(binary_search(_idTax.begin(), _idTax.end(), tmpCompany, _compareTax)) {
        delete tmpCompany;
        return true;
    }
    delete tmpCompany;
    return false;
}

bool CVATRegister::_companyExists(const string & name, const string & addr) const {
    Company * tmpCompany = new Company(name, addr, "FOO");
    if(binary_search(_namesCompanies.begin(), _namesCompanies.end(), tmpCompany, _compareNameCompany)) {
        delete tmpCompany;
        return true;
    }
    
    delete tmpCompany;
    return false;
}

bool CVATRegister::_compareMinHeap(const unsigned int & a, const unsigned int & b) {
    return a > b;
}

bool CVATRegister::_insertByIdTax(Company * company) {
    
    vector<Company*>::iterator itIdTax;
    
    itIdTax = lower_bound(_idTax.begin(), _idTax.end(), company, _compareTax);
    
    _idTax.insert(itIdTax, company);
    
    return true;
}

bool CVATRegister::_insertByNamesCompanies(Company * company) {
    
    vector<Company*>::iterator itNameCompany;
    
    itNameCompany = lower_bound(_namesCompanies.begin(), _namesCompanies.end(), company, _compareNameCompany);
    
    _namesCompanies.insert(itNameCompany, company);
    
    return true;
}

// Inspired by: https://gist.github.com/Vedrana/3675434
bool CVATRegister::_addToMaxMinHeap(unsigned int amount) {
    
    _maxInvoice.push_back(amount);
    push_heap(_maxInvoice.begin(), _maxInvoice.end());
    
    if (_numInvoices % 2 == 0) {
        if (_minInvoice.empty()) {
            _numInvoices++;
            return true;
        } else if (_maxInvoice.front() > _minInvoice.front()) {
            
            unsigned int maxHeapRoot = _maxInvoice.front();
            pop_heap(_maxInvoice.begin(), _maxInvoice.end());
            _maxInvoice.pop_back();
            
            unsigned int minHeapRoot = _minInvoice.front();
            pop_heap(_minInvoice.begin(), _minInvoice.end(), _compareMinHeap);
            _minInvoice.pop_back();
            
            _maxInvoice.push_back(minHeapRoot);
            push_heap(_maxInvoice.begin(), _maxInvoice.end());
            
            _minInvoice.push_back(maxHeapRoot);
            push_heap(_minInvoice.begin(), _minInvoice.end(), _compareMinHeap);
        }
    } else {
        _minInvoice.push_back(_maxInvoice.front());
        push_heap(_minInvoice.begin(), _minInvoice.end(), _compareMinHeap);
    
        pop_heap(_maxInvoice.begin(), _maxInvoice.end());
        _maxInvoice.pop_back();
    }
    _numInvoices++;

    return true;
}

unsigned int CVATRegister::_getMedian() const {
    if (_numInvoices == 0) return 0;
    if (_numInvoices % 2 != 0) return _maxInvoice.front();
    
    return _minInvoice.front();
}


string CVATRegister::toLower(const string & a) {
    string lowerStr = a;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

bool CVATRegister::_compareTax(const Company * a, const Company * b) {
     return (a->taxID < b->taxID);
}

bool CVATRegister::_compareNameCompany(const Company * a, const Company * b) {
    if(a->name < b->name) return true;
    else if(a->name > b->name) return false;
    else if(a->addr < b->addr) return true;
    
    return false;
}

void CVATRegister::printHeap() const {
    vector<unsigned int>::const_iterator it;
    cout << "MAX : ";
    for (it = _maxInvoice.begin(); it != _maxInvoice.end(); it++) {
        cout << (*it) << " ";
    }
    cout << endl;
    
    cout << "MIN : ";
    for (it = _minInvoice.begin(); it != _minInvoice.end(); it++) {
        cout << (*it) << " ";
    }
    cout << endl;
}


void CVATRegister::print() const {
    
    
    cout << "Sorted by IDs: " << endl;
    for (auto it = _idTax.begin(); it != _idTax.end(); it++) {
        cout << (*it)->taxID << " (" << (*it)->name << "," << (*it)->addr << ")" << endl;
    }
    
    cout << "Sorted by names and companies: " << endl;
    for (auto it = _namesCompanies.begin(); it != _namesCompanies.end(); it++) {
        cout << (*it)->taxID << " (" << (*it)->name << "," << (*it)->addr << ")" << endl;
    }
}


//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------

#ifndef __PROGTEST__
int main ( void )
{
    unsigned int sumIncome;
    
    CVATRegister b1;
    assert ( b1 . NewCompany ( "ACME", "Kolejni", "666/666/666" ) );
    assert ( b1 . NewCompany ( "ACME", "Kolejni", "666/666/666" ) == false);
    assert ( b1 . NewCompany ( "ACME", "Thakurova", "666/666" ) );
    assert ( b1 . NewCompany ( "Dummy", "Thakurova", "123456" ) );
    
    assert ( b1 . Invoice ( "666/666", 2000 ) );
    assert ( b1 . MedianInvoice () == 2000 );
    assert ( b1 . Invoice ( "666/666/666", 3000 ) );
    assert ( b1 . MedianInvoice () == 3000 );
    assert ( b1 . Invoice ( "123456", 4000 ) );
    assert ( b1 . MedianInvoice () == 3000 );
    assert ( b1 . Invoice ( "aCmE", "Kolejni", 5000 ) );
    assert ( b1 . MedianInvoice () == 4000 );
    assert ( b1 . Audit ( "ACME", "Kolejni", sumIncome ) && sumIncome == 8000 );
    assert ( b1 . Audit ( "123456", sumIncome ) && sumIncome == 4000 );
    assert ( b1 . CancelCompany ( "ACME", "KoLeJnI" ) );
    assert ( b1 . MedianInvoice () == 4000 );
    assert ( b1 . CancelCompany ( "666/666" ) );
    assert ( b1 . MedianInvoice () == 4000 );
    assert ( b1 . Invoice ( "123456", 100 ) );
    assert ( b1 . MedianInvoice () == 3000 );
    assert ( b1 . Invoice ( "123456", 300 ) );
    assert ( b1 . MedianInvoice () == 3000 );
    assert ( b1 . Invoice ( "123456", 200 ) );
    assert ( b1 . MedianInvoice () == 2000 );
    assert ( b1 . Invoice ( "123456", 230 ) );
    assert ( b1 . MedianInvoice () == 2000 );
    assert ( b1 . Invoice ( "123456", 830 ) );
    assert ( b1 . MedianInvoice () == 830 );
    assert ( b1 . Invoice ( "123456", 1830 ) );
    assert ( b1 . MedianInvoice () == 1830 );
    assert ( b1 . Invoice ( "123456", 2830 ) );
    assert ( b1 . MedianInvoice () == 1830 );
    assert ( b1 . Invoice ( "123456", 2830 ) );
    assert ( b1 . MedianInvoice () == 2000 );
    assert ( b1 . Invoice ( "123456", 3200 ) );
    assert ( b1 . MedianInvoice () == 2000 );
    
    CVATRegister b2;
    assert ( b2 . NewCompany ( "ACME", "Kolejni", "abcdef" ) );
    assert ( b2 . NewCompany ( "Dummy", "Kolejni", "123456" ) );
    assert ( ! b2 . NewCompany ( "AcMe", "kOlEjNi", "1234" ) );
    assert ( b2 . NewCompany ( "Dummy", "Thakurova", "ABCDEF" ) );
    assert ( b2 . MedianInvoice () == 0 );
    assert ( b2 . Invoice ( "ABCDEF", 1000 ) );
    assert ( b2 . MedianInvoice () == 1000 );
    assert ( b2 . Invoice ( "abcdef", 2000 ) );
    assert ( b2 . MedianInvoice () == 2000 );
    assert ( b2 . Invoice ( "aCMe", "kOlEjNi", 3000 ) );
    assert ( b2 . MedianInvoice () == 2000 );
    assert ( ! b2 . Invoice ( "1234567", 100 ) );
    assert ( ! b2 . Invoice ( "ACE", "Kolejni", 100 ) );
    assert ( ! b2 . Invoice ( "ACME", "Thakurova", 100 ) );
    assert ( ! b2 . Audit ( "1234567", sumIncome ) );
    assert ( ! b2 . Audit ( "ACE", "Kolejni", sumIncome ) );
    assert ( ! b2 . Audit ( "ACME", "Thakurova", sumIncome ) );
    assert ( ! b2 . CancelCompany ( "1234567" ) );
    assert ( ! b2 . CancelCompany ( "ACE", "Kolejni" ) );
    assert ( ! b2 . CancelCompany ( "ACME", "Thakurova" ) );
    assert ( b2 . CancelCompany ( "abcdef" ) );
    assert ( b2 . MedianInvoice () == 2000 );
    assert ( ! b2 . CancelCompany ( "abcdef" ) );
    assert ( b2 . NewCompany ( "ACME", "Kolejni", "abcdef" ) );
    assert ( b2 . CancelCompany ( "ACME", "Kolejni" ) );
    assert ( ! b2 . CancelCompany ( "ACME", "Kolejni" ) );
    
    
    return 0;
}
#endif /* __PROGTEST__ */
