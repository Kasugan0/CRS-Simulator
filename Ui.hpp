#pragma once

#include <cmath>
#include <cctype>
#include <limits>
#include <iostream>
#include <algorithm>
#include <string>
#include <functional>

#include "Account.hpp"




namespace Ui
{
    static constexpr size_t CHOICES_OF_MENU = 5;
    static constexpr size_t MAX_LENGTH_OF_CHOICE = static_cast<int>(std::floor(std::log10(CHOICES_OF_MENU))) + 1;
    static std::string tempInput;


    void waitInput();
    void clearData();
    void clearScreen();
    void tillValid(std::function<void()> dispaly, std::function<bool()> isValid, std::string& result);
    void tillValid(std::function<void()> dispaly, std::function<bool()> isValid);
    void tillValid(std::function<void(const char*)> display, const char* message, std::function<bool()> isValid);
    void tillValid(std::function<void(const char*)> display, const char* message, std::function<bool()> isValid, std::string& result);

    void doubleCheckPassword(Password& result);
    bool authenticateAccount(Account* account);

    bool deposit(Account* validAccPtr);
    bool withdraw(Account* validAccPtr);
    bool transfer(Account* fromAccPtr, Pool& pool);

    void welcome();
    void menu();
    void pause();
    void fullScreenMsg(const char* message);
    void wrongInput();
    void passwordWrongOrRetry();
    void accountNotFoundOrRetry(const std::string toAccNum);
    void accountIsSelfOrRetry();
    void amountNotMultipleOf100OrRetry();
    void exceedDailyRestrictOrRetry();
    void exceedSingleOpRestrictOrRetry();
    void exceedAccountBalanceOrRetry();

    void accountCreating(const std::string& number);
    void accountCreated(const Account& account);
    void accountUnavailable(const Account* accountPtr);
    void showAccountInfo(const Account* accountPtr);

    bool isValidNumber();
    bool isValidYesOrNo();
    bool isYes();
    bool isValidName();
    bool isValidPassword();
    bool isValidId();
    bool isValidChoice();
    bool isValidAmount();
}