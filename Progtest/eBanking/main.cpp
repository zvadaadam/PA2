//
//  main.cpp
//  eBanking
//
//  Created by Adam Zvada on 05.04.17.
//  Copyright © 2017 Adam Zvada. All rights reserved.
//

#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

#endif /* __PROGTEST__ */


const char * copyStr(const char * str){
    char * newStr = new char[strlen(str)+1];
    strcpy(newStr, str);
    return newStr;
}


//--------------------LIST-----------------------------

template<typename T> class List; //forward decleration

template<typename T>
ostream & operator <<(ostream &, const List<T> &);

template <typename T>
class List {
    public:
    List();
    List(List const &);
    List(const T *);
    ~List();
    
    void push_back(const T &);
    void pop_back();
    
    bool isEmpty() const;
    
    unsigned int size() const;
    
    List<T> operator = (const T *);
    
    const T operator [] (unsigned int) const;
    T & operator [] (unsigned int);
    
    friend ostream & operator << <>(ostream &, const List &);
    
    private:
    T * _data;
    unsigned int _maxSize;
    unsigned int _curSize;
    
    void _realloc();
};

template<class T>
List<T>::List() {
    _maxSize = 5;
    _curSize = 0;
    _data = new T[_maxSize];
}

template<class T>
List<T>::List(List const & list) {
    //TODO
    
}

template<class T>
List<T>::List(const T * data) {
    //TODO
}

template<class T>
List<T>::~List() {
    delete [] _data;
    _curSize = 0;
    _maxSize = 0;
}

template<class T>
void List<T>::push_back(const T & element) {
    
    if (_curSize + 1 >= _maxSize) {
        _realloc();
    }
    _data[_curSize++] = element;
}

template<class T>
void List<T>::pop_back() {
    if (isEmpty()) {
        return;
    }
    _curSize--;
}

template<class T>
bool List<T>::isEmpty() const {
    if (_curSize == 0) {
        return true;
    }
    
    return false;
}

template<class T>
unsigned int List<T>::size() const {
    return this->_curSize;
}

template<class T>
List<T> List<T>::operator = (const T *) {
  //TODO
}

template<class T>
const T List<T>::operator[](unsigned int index) const {
    return _data[index];
}

template<class T>
T & List<T>::operator[](unsigned int index) {
    return _data[index];
}

template<class T>
void List<T>::_realloc() {
    T * newData = new T[(_maxSize*=2)];
    
    for (unsigned int i = 0; i < _curSize; i++) {
        newData[i] = _data[i];
    }
    
    delete [] _data;
    _data = newData;
}

template<class T>
ostream & operator << (ostream & out, const List<T> & list) {
    for (unsigned int i = 0; i < list._curSize; i++) {
        out << list._data[i] << " ";
    }
    return out;
}


//---------------Transaction-------------------------------


class CTransaction {
public:
    CTransaction(const char * senderID, const char * reciverID, int amnout, const char * signature);
    ~CTransaction();
    
    const char * _senderID;
    const char * _reciverID;
    const char * _signature;
 
    int _amount;
    
    int _refCount;
};

CTransaction::CTransaction(const char * senderID, const char * reciverID, int amnout, const char * signature) {
    _senderID = copyStr(senderID);
    _reciverID = copyStr(reciverID);
    _amount = amnout;
    _signature = copyStr(signature);
    _refCount = 0;
}

CTransaction::~CTransaction() {
    delete _senderID;
    delete _reciverID;
    delete _signature;
}

//---------------UserAccount-------------------------------


class CAccount {
public:
    
    CAccount(const char * accID, int initBalance);
    CAccount(const CAccount * acc);
    ~CAccount();
    
    long Balance() const;
    
    const char * getAccID() const;
    int getInitBalance();
    
    List<CTransaction*> getTransactions();
    
    void setBalance(int balance);
    
    void decreaseBalBy(int amount);
    void increaseBalBy(int amount);
    
    void addTransaction(CTransaction * transaction);
    
    void removeTransactions();
    
    void changeInitBalance();
    
    friend ostream & operator<<(ostream & os, const CAccount & account);
    
    int refCount;
private:
    
    List<CTransaction*> _transactions;
    
    const char * _accID;
    int _initBalance;
    int _balance;
};


CAccount::CAccount(const char * accID, int initBalance) {
    _accID = copyStr(accID);
    _initBalance = initBalance;
    _balance = initBalance;
    
    refCount = 0;
}

CAccount::CAccount(const CAccount * acc) {
    _accID = copyStr(acc->_accID);
    _initBalance = acc->_initBalance;
    _balance = acc->_balance;
    refCount = 0;
    
//    for (unsigned int i = 0; i < acc->_transactions.size(); i++) {
//        CTransaction * trans = acc->_transactions[i];
//        CTransaction * newTrans = new CTransaction(trans->_senderID, trans->_reciverID, trans->_amount, trans->_signature);
//        this->addTransaction(newTrans);
//    }
//
    for (unsigned int i = 0; i < acc->_transactions.size(); i++) {
        CTransaction * trans = acc->_transactions[i];
        addTransaction(trans);
    }
    
}
CAccount::~CAccount() {
    delete _accID;
    for (unsigned int i = 0; i < _transactions.size(); i++) {
        if (_transactions[i]->_refCount == 1) {
            delete _transactions[i];
        } else {
            _transactions[i]->_refCount--;
        }
        
    }
}

long CAccount::Balance() const {
    return _balance;
}

int CAccount::getInitBalance() {
    return _initBalance;
}

const char * CAccount::getAccID() const {
    return _accID;
}

List<CTransaction*> CAccount::getTransactions() {
    return _transactions;
}

void CAccount::decreaseBalBy(int amount) {
    _balance -= amount;
}

void CAccount::increaseBalBy(int amount) {
    _balance += amount;
}

void CAccount::addTransaction(CTransaction * transaction) {
    _transactions.push_back(transaction);
    _transactions[_transactions.size()-1]->_refCount++;
}

void CAccount::removeTransactions() {
    int size = _transactions.size();
    for (int i = 0; i < size; i++) {
        if (_transactions[i]->_refCount == 1) {
            delete _transactions[i];
        } else {
            _transactions[i]->_refCount--;
        }
        _transactions.pop_back();
    }
}

void CAccount::changeInitBalance() {
    _initBalance = _balance;
}


ostream & operator<<(ostream & os, const CAccount & account) {
    
    os << account._accID << ":\n   " << account._initBalance << endl;
    
    for (unsigned int i = 0; i < account._transactions.size(); i++) {
        CTransaction * transaction = account._transactions[i];
        if (strcmp(transaction->_senderID, account.getAccID()) == 0) {
            os << " - " << transaction->_amount << ", to: " << transaction->_reciverID << ", sign: " << transaction->_signature << endl;
        } else {
            os << " + " << transaction->_amount << ", from: " << transaction->_senderID << ", sign: " << transaction->_signature << endl;
        }
    }
    
    os << " = " << account._balance << endl;
    
    return os;
}


//----------------------CBank-------------------------------


class CBank {
public:
    
    CBank();
    ~CBank();
    CBank(const CBank & cBank);
    CBank & operator=(const CBank & cBank);
    
    bool NewAccount(const char * accID, int initialBalance);
    bool Transaction(const char * debAccID, const char * credAccID, int amount, const char * signature);
    bool TrimAccount(const char * accID);
    
    CAccount & Account(const char * accID) const;
    
private:
    
    List<CAccount*> _accounts;
    
    CAccount * _findAcc(const char * accID) const;
    
    void _addAccount(CAccount * acc);
};

CBank::CBank() {

}

CBank::~CBank() {
    for (unsigned int i = 0; i < _accounts.size(); i++) {
        delete _accounts[i];
    }
}

CBank::CBank(const CBank & cBank) {
    
    for (unsigned int i = 0; i < cBank._accounts.size(); i++) {
        CAccount * newAcc = new CAccount(cBank._accounts[i]);
        _accounts.push_back(newAcc);
    }
    
//    for (unsigned int i = 0; i < cBank._accounts.size(); i++) {
//        _addAccount(cBank._accounts[i]);
//    }

}

CBank & CBank::operator=(const CBank & cBank) {
    
    if (&cBank == this) return *this;
    
    int prevAcc = _accounts.size();
    for (int i = 0; i < prevAcc; i++) {
        _accounts.pop_back();
        delete _accounts[i];
    }
        
    List<CAccount*> acc;
    for (unsigned int i = 0; i < cBank._accounts.size(); i++) {
        CAccount * newAcc = new CAccount(cBank._accounts[i]);
        _accounts.push_back(newAcc);
    }
    
    
    return *this;
}

bool CBank::NewAccount(const char * accID, int initialBalance) {

    if (_findAcc(accID) != nullptr) {
        return false;
    }
    
    CAccount * account = new CAccount(accID, initialBalance);
    
    _accounts.push_back(account);
    
    return true;
}

bool CBank::Transaction(const char * debAccID, const char * credAccID, int amount, const char * signature) {
    
    CAccount * debAcc = _findAcc(debAccID);
    CAccount * credAcc = _findAcc(credAccID);
    
    if (debAcc == nullptr || credAcc == nullptr || debAcc == credAcc) {
        return false;
    }
    
    CTransaction * transaction = new CTransaction(debAccID, credAccID, amount, signature);
    
    debAcc->decreaseBalBy(amount);
    debAcc->addTransaction(transaction);
    
    credAcc->increaseBalBy(amount);
    credAcc->addTransaction(transaction);
    
    return true;
}

bool CBank::TrimAccount(const char * accID) {
    
    CAccount * acc = _findAcc(accID);
    
    if (acc == nullptr) {
        return false;
    }
    
    acc->removeTransactions();
    
    acc->changeInitBalance();
    
    return true;
}

CAccount & CBank::Account(const char * accID) const {
    CAccount * acc = _findAcc(accID);
    if (acc == nullptr) {
        throw 222;
    }
    return * acc;
}

CAccount *  CBank::_findAcc(const char * accID) const {
    
    for (unsigned int i = 0; i < _accounts.size(); i++) {
        if (strcmp(accID, _accounts[i]->getAccID()) == 0) {
            return _accounts[i];
        }
    }
    
    return nullptr;
}

void CBank::_addAccount(CAccount * acc) {
    _accounts.push_back(acc);
    _accounts[_accounts.size()-1]->refCount++;
}

#ifndef __PROGTEST__
int main ( void )
{
    ostringstream os;
    char accCpy[100], debCpy[100], credCpy[100], signCpy[100];
    
    CBank x0;
    assert ( x0 . NewAccount ( "123456", 1000 ) );
    assert ( x0 . NewAccount ( "987654", -500 ) );
    assert ( x0 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
    assert ( x0 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
    assert ( x0 . NewAccount ( "111111", 5000 ) );
    assert ( x0 . Transaction ( "111111", "987654", 290, "Okh6e+8rAiuT5=" ) );
    assert ( x0 . Account ( "123456" ). Balance ( ) ==  -2190 );
    assert ( x0 . Account ( "987654" ). Balance ( ) ==  2980 );
    assert ( x0 . Account ( "111111" ). Balance ( ) ==  4710 );

    
   os . str ( "" );
   os << x0 . Account ( "123456" );
    
    cout << os.str() << endl;
    assert ( ! strcmp ( os . str () . c_str (), "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n" ) );
    os . str ( "" );
    os << x0 . Account ( "987654" );
    assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 290, from: 111111, sign: Okh6e+8rAiuT5=\n = 2980\n" ) );
    os . str ( "" );
    os << x0 . Account ( "111111" );
    assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 290, to: 987654, sign: Okh6e+8rAiuT5=\n = 4710\n" ) );
    assert ( x0 . TrimAccount ( "987654" ) );
    assert ( x0 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
    os . str ( "" );
    os << x0 . Account ( "987654" );
    cout << os.str() << endl;
    assert ( ! strcmp ( os . str () . c_str (), "987654:\n   2980\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 3103\n" ) );
    
    CBank x2;
    strncpy ( accCpy, "123456", sizeof ( accCpy ) );
    assert ( x2 . NewAccount ( accCpy, 1000 ));
    strncpy ( accCpy, "987654", sizeof ( accCpy ) );
    assert ( x2 . NewAccount ( accCpy, -500 ));
    strncpy ( debCpy, "123456", sizeof ( debCpy ) );
    strncpy ( credCpy, "987654", sizeof ( credCpy ) );
    strncpy ( signCpy, "XAbG5uKz6E=", sizeof ( signCpy ) );
    assert ( x2 . Transaction ( debCpy, credCpy, 300, signCpy ) );
    strncpy ( debCpy, "123456", sizeof ( debCpy ) );
    strncpy ( credCpy, "987654", sizeof ( credCpy ) );
    strncpy ( signCpy, "AbG5uKz6E=", sizeof ( signCpy ) );
    assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) );
    strncpy ( accCpy, "111111", sizeof ( accCpy ) );
    assert ( x2 . NewAccount ( accCpy, 5000 ));
    strncpy ( debCpy, "111111", sizeof ( debCpy ) );
    strncpy ( credCpy, "987654", sizeof ( credCpy ) );
    strncpy ( signCpy, "Okh6e+8rAiuT5=", sizeof ( signCpy ) );
    assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) );
    assert ( x2 . Account ( "123456" ). Balance ( ) ==  -2190 );
    assert ( x2 . Account ( "987654" ). Balance ( ) ==  5580 );
    assert ( x2 . Account ( "111111" ). Balance ( ) ==  2110 );
    os . str ( "" );
    os << x2 . Account ( "123456" );
    assert ( ! strcmp ( os . str () . c_str (), "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n" ) );
    os . str ( "" );
    os << x2 . Account ( "987654" );
    assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n = 5580\n" ) );
    os . str ( "" );
    os << x2 . Account ( "111111" );
    assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n = 2110\n" ) );
    assert ( x2 . TrimAccount ( "987654" ) );
    strncpy ( debCpy, "111111", sizeof ( debCpy ) );
    strncpy ( credCpy, "987654", sizeof ( credCpy ) );
    strncpy ( signCpy, "asdf78wrnASDT3W", sizeof ( signCpy ) );
    assert ( x2 . Transaction ( debCpy, credCpy, 123, signCpy ) );
    os . str ( "" );
    os << x2 . Account ( "987654" );
    assert ( ! strcmp ( os . str () . c_str (), "987654:\n   5580\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) );
    
    CBank x4;
    assert ( x4 . NewAccount ( "123456", 1000 ) );
    assert ( x4 . NewAccount ( "987654", -500 ) );
    assert ( !x4 . NewAccount ( "123456", 3000 ) );
    assert ( !x4 . Transaction ( "123456", "666", 100, "123nr6dfqkwbv5" ) );
    assert ( !x4 . Transaction ( "666", "123456", 100, "34dGD74JsdfKGH" ) );
    assert ( !x4 . Transaction ( "123456", "123456", 100, "Juaw7Jasdkjb5" ) );
    try
    {
        x4 . Account ( "666" ). Balance ( );
        assert ( "Missing exception !!" == NULL );
    }
    catch ( ... )
    {
    }
    try
    {
        os << x4 . Account ( "666" ). Balance ( );
        assert ( "Missing exception !!" == NULL );
    }
    catch ( ... )
    {
    }
    
    assert ( !x4 . TrimAccount ( "666" ) );
    
    CBank x6;
    assert ( x6 . NewAccount ( "123456", 1000 ) );
    assert ( x6 . NewAccount ( "987654", -500 ) );
    assert ( x6 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
    assert ( x6 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
    assert ( x6 . NewAccount ( "111111", 5000 ) );
    assert ( x6 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
    CBank x7 ( x6 );
    
    os. str("");
    
    os << x7.Account("111111");
    
    assert ( x6 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
    assert ( x7 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
    assert ( x6 . NewAccount ( "99999999", 7000 ) );
    assert ( x6 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
    assert ( x6 . TrimAccount ( "111111" ) );
    assert ( x6 . Transaction ( "123456", "111111", 221, "Q23wr234ER==" ) );
    os . str ( "" );
    os << x6 . Account ( "111111" );
    assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n + 221, from: 123456, sign: Q23wr234ER==\n = -1581\n" ) );
    os . str ( "" );
    os << x6 . Account ( "99999999" );
    assert ( ! strcmp ( os . str () . c_str (), "99999999:\n   7000\n + 3789, from: 111111, sign: aher5asdVsAD\n = 10789\n" ) );
    os . str ( "" );
    os << x6 . Account ( "987654" );
    assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) );
    os . str ( "" );
    os << x7 . Account ( "111111" );
    assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );
    try
    {
        os << x7 . Account ( "99999999" ). Balance ( );
        assert ( "Missing exception !!" == NULL );
    }
    catch ( ... )
    {
    }
    os . str ( "" );
    os << x7 . Account ( "987654" );
    assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 789, from: 111111, sign: SGDFTYE3sdfsd3W\n = 6369\n" ) );

    CBank x8;
    CBank x9;
    x9.NewAccount("123456", 1000);
    assert ( x8 . NewAccount ( "123456", 1000 ) );
    assert ( x8 . NewAccount ( "987654", -500 ) );
    assert ( x8 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
    assert ( x8 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
    assert ( x8 . NewAccount ( "111111", 5000 ) );
    assert ( x8 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
    x9 = x8;
    assert ( x8 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
    assert ( x9 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
    assert ( x8 . NewAccount ( "99999999", 7000 ) );
    assert ( x8 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
    assert ( x8 . TrimAccount ( "111111" ) );
    os . str ( "" );
    os << x8 . Account ( "111111" );
    assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n = -1802\n" ) );
    os . str ( "" );
    os << x9 . Account ( "111111" );
    assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );
    
    return 0;
}
#endif /* __PROGTEST__ */
