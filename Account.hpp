#pragma once

#include <cctype>
#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>





class Id;
class Password;
class Account;
class Pool;




class Id
{
private:
    std::string id;
    static constexpr int WEIGH[] = {7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2};
    static constexpr char EXPECT[] = {'1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2', '!'};


public:
    static constexpr size_t LENGTH_OF_ID = 18;
    
    Id() = default;
    Id(const Id&) = default;
    Id& operator=(const Id&) = default;
    Id(const std::string& s);
    static bool verify(const std::string& s);
    std::string getId() const;
};



class Password
{
private:
    static std::hash<std::string> hasher;
    inline static const std::string HASH_SALT = "Kasugano_0";
    size_t hashed;


public:
    static constexpr size_t LENGTH_OF_PASSWORD = 6;
    Password() = default;
    Password(const std::string& plaintext);
    Password(const Password&) = default;
    Password& operator=(const Password&) = default;
    bool authenticate(const std::string& plaintext) const;
    void changePassword(const std::string& plaintext);

    friend class Account;
    friend class Pool;
};



class Account
{
private:
    std::string number;
    std::string name;
    Id id;
    Password password;
    bool available;
    size_t wrongPasswordAttempted;
    long long balance;
    long long usedBalanceToday;
    

public:
    static constexpr size_t LENGTH_OF_ACCOUNT_NUMBER = 19;
    static constexpr size_t MAX_LENGTH_OF_NAME = 50;
    static constexpr long long MAX_AMOUNT_VALUE = std::numeric_limits<long long>::max();
    static constexpr size_t MAX_LENGTH_OF_AMOUNT = static_cast<int>(std::floor(std::log10(MAX_AMOUNT_VALUE))) + 1;
    static constexpr size_t MAX_WRONG_PASSWORD_ATTEMPT = 3;
    static constexpr long long DEFAULT_BALANCE = 10000;
    static constexpr long long RESTRICT_PER_DAY = 500000;
    static constexpr long long RESTRICT_PER_OPERATION = 100000;
    

    Account() = default;
    Account(const std::string& number, const std::string& name, const Id& id, const Password& password, const bool available = true, size_t wrongPasswordAttempted = 0, long long balance = DEFAULT_BALANCE, long long usedBalanceToday = 0);
    Account(const Account&) = default;
    Account& operator=(const Account&) = default;

    bool isAvailable();
    void setAvailable(bool flag);
    std::string getNumber() const;
    std::string getName() const;
    Id getId() const;
    long long getBalance() const;
    long long getUsedBalanceToday() const;
    size_t getWrongPasswordAttempted();

    bool authenticate(const Password& givenPassword);
    void deposit(long long amount);
    void withdraw(long long amount);
    void transferTo(std::string num, long long amount, Pool& pool);
    void changePassword(const Password& newPassword);
    friend class Pool;
};



class Pool
{
private:
    inline static const std::string OUTPUT_FILE_NAME = "AccountInfo.txt";
    std::unordered_map<std::string, Account*> accounts;


public:
    Pool() = default;
    Pool(const Pool&) = default;
    Pool& operator=(const Pool&) = default;
    ~Pool();
    void newAccount(Account* p);
    void deleteAccount(const std::string& account);
    size_t count() const;
    bool contains(const std::string& account) const;
    Account* getAccount(const std::string& number);
    void save() const;
};