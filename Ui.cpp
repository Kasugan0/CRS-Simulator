#include "Ui.hpp"



namespace Ui
{
    void waitInput()
    {
        std::cin >> tempInput;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }


    void clearData()
    {
        tempInput.clear();
    }


    void clearScreen()
    {
        #ifdef _WIN32
            system("cls");
        #elif defined(__unix__) || defined(__linux__) || defined(_POSIX_VERSION)
            system("clear");
        #elif defined(__APPLE__) && defined(__MACH__)
            system("clear");
        #endif
    }


    void tillValid(std::function<void()> display, std::function<bool()> isValid, std::string& result)
    {
        do
        {
            display();
            waitInput();
            if (!isValid()) wrongInput();
        } while (!isValid());
        result = tempInput;
        Ui::clearData();
    }


    void tillValid(std::function<void()> display, std::function<bool()> isValid)
    {
        do
        {
            display();
            waitInput();
            if (!isValid()) wrongInput();
        } while (!isValid());
    }


    void tillValid(std::function<void(const char*)> display, const char* message, std::function<bool()> isValid)
    {
        do
        {
            display(message);
            waitInput();
            if (!isValid()) wrongInput();
        } while (!isValid());
    }


    void tillValid(std::function<void(const char*)> display, const char* message, std::function<bool()> isValid, std::string& result)
    {
        do
        {
            display(message);
            waitInput();
            if (!isValid()) wrongInput();
        } while (!isValid());
        result = tempInput;
        Ui::clearData();
    }


    void doubleCheckPassword(Password& result)
    {
        std::string first, second;
        do
        {
            tillValid(fullScreenMsg, "请输入密码：", isValidPassword, first);
            tillValid(fullScreenMsg, "请确认刚刚输入的密码：", isValidPassword, second);
        } while (first != second);
        result.changePassword(first);
        clearData();
    }


    bool authenticateAccount(Account* validAccPtr)
    {
        bool blocked = false, passed = false;
        std::string userInput;
        while (!blocked && !passed)
        {
            tillValid(fullScreenMsg, "请输入密码：", isValidPassword, userInput);
            Password givenPassword(userInput);
            if (validAccPtr->authenticate(givenPassword)) passed = true;
            else
            {
                tillValid(passwordWrongOrRetry, isValidYesOrNo);
                if (!isYes())
                {
                    clearData();
                    clearScreen();
                    return false;
                }
                else
                {
                    if (!validAccPtr->isAvailable())
                    {
                        blocked = true;
                        continue;
                    }
                    clearScreen();
                    std::cout << "还可尝试 "
                        << Account::MAX_WRONG_PASSWORD_ATTEMPT - validAccPtr->getWrongPasswordAttempted() 
                        << " 次，尝试次数用完将锁定账户" << std::endl;
                    pause();
                    clearData();
                    clearScreen();
                    continue;
                }
            }
        }
        if (blocked)
        {
            accountUnavailable(validAccPtr);
            pause();
            return false;
        }
        if (passed) return true;
        return false;
    }


    bool deposit(Account* validAccPtr)
    {
        long long amount;
        do
        {
            tillValid(fullScreenMsg, "请输入金额：", isValidAmount);
            amount = std::stoll(tempInput);
            if (amount % 100 != 0) tillValid(amountNotMultipleOf100OrRetry, isValidYesOrNo);
            else break;

            if (!isYes()) return false;
            else continue;
        } while (true);
        validAccPtr->deposit(amount);
        return true;
    }


    bool withdraw(Account* validAccPtr)
    {
        long long amount;
        do
        {
            tillValid(fullScreenMsg, "请输入金额：", isValidAmount);
            amount = std::stoll(tempInput);
            if (amount % 100 != 0) tillValid(amountNotMultipleOf100OrRetry, isValidYesOrNo);
            else if (amount > Account::RESTRICT_PER_OPERATION) tillValid(exceedSingleOpRestrictOrRetry, isValidYesOrNo);
            else if (validAccPtr->getUsedBalanceToday() + amount > Account::RESTRICT_PER_DAY) tillValid(exceedDailyRestrictOrRetry, isValidYesOrNo);
            else if (amount > validAccPtr->getBalance()) tillValid(exceedAccountBalanceOrRetry, isValidYesOrNo);
            else break;

            if (!isYes()) return false;
            else continue;
        } while (true);
        validAccPtr->withdraw(amount);
        return true;
    }


    bool transfer(Account* fromAccPtr, Pool& pool)
    {
        size_t validInputTimes = 0;
        std::string first, second;
        do
        {
            if (validInputTimes < 1) fullScreenMsg("请输入收款方账号：");
            else fullScreenMsg("请确认收款方账号：");

            waitInput();
            if (!isValidNumber())
            {
                wrongInput();
                continue;
            }
            else if (fromAccPtr->getNumber() == tempInput) tillValid(accountIsSelfOrRetry, isValidYesOrNo);
            else if (!pool.contains(tempInput))
                do
                {
                    accountNotFoundOrRetry(tempInput);
                    waitInput();
                    if (!isValidYesOrNo()) wrongInput();
                } while (!isValidYesOrNo());
            else
            {
                if (!validInputTimes++) first = tempInput;
                else second = tempInput;
                continue;
            }

            if (!isYes()) return false;
            else continue;
        } while (first != second || first.empty() || second.empty());
        

        long long amount;
        do
        {
            tillValid(fullScreenMsg, "请输入金额：", isValidAmount);
            amount = std::stoll(tempInput);
            if (amount > Account::RESTRICT_PER_OPERATION) tillValid(exceedSingleOpRestrictOrRetry, isValidYesOrNo);
            else if (fromAccPtr->getUsedBalanceToday() + amount > Account::RESTRICT_PER_DAY) tillValid(exceedDailyRestrictOrRetry, isValidYesOrNo);
            else if (amount > fromAccPtr->getBalance()) tillValid(exceedAccountBalanceOrRetry, isValidYesOrNo);
            else break;

            if (!isYes()) return false;
            else continue;
        } while (true);

        fromAccPtr->transferTo(first, amount, pool);
        return true;
    }


    void welcome()
    {
        clearScreen();
        std::cout << "***************************************" << std::endl;
        std::cout << "***************************************" << std::endl;
        std::cout << "******   欢迎使用 ATM 自助服务   ******" << std::endl;
        std::cout << "***************************************" << std::endl;
        std::cout << "***************************************" << std::endl;
        std::cout << "请输入卡号：" << std::endl;
    }


    void menu()
    {
        clearScreen();
        std::cout << "***************************************" << std::endl;
        std::cout << "******          主菜单           ******" << std::endl;
        std::cout << "***************************************" << std::endl;
        std::cout << "1 - 查询余额" << std::endl;
        std::cout << "2 - 存款" << std::endl;
        std::cout << "3 - 取款" << std::endl;
        std::cout << "4 - 转账" << std::endl;
        std::cout << "5 - 修改密码" << std::endl;
        std::cout << "0 - 退卡" << std::endl;
        std::cout << "***************************************" << std::endl;
        std::cout << "请输入你的选择：" << std::endl;
    }


    void pause()
    {
        std::cout << "请按回车键继续……" << std::endl;
        std::cin.get();
    }


    void fullScreenMsg(const char* message)
    {
        clearScreen();
        std::cout << message << std::endl;
    }


    void wrongInput()
    {
        fullScreenMsg("输入有误，请核实后重试！");
        pause();
    }


    void passwordWrongOrRetry()
    {
        fullScreenMsg("密码错误！");
        std::cout << "一天内密码输入错误次数超过 " << Account::MAX_WRONG_PASSWORD_ATTEMPT << " 次将锁定账户" << std::endl;
        std::cout << "是否重试[y/n]" << std::endl;
    }


    void accountNotFoundOrRetry(const std::string toAccNum)
    {
        clearScreen();
        std::cout << "未找到账号 " << toAccNum << " ，请核对。" << std::endl;
        std::cout << "是否重新输入收款方账号？[y/n]" << std::endl;
    }


    void accountIsSelfOrRetry()
    {
        fullScreenMsg("汇款方和收款方账号不能相同");
        std::cout << "是否重新输入收款方账号？[y/n]" << std::endl;
    }


    void transferOrCancel(const std::string toAccNum, size_t amount)
    {
        clearScreen();
        std::cout << "确认是否向 " << toAccNum << " 转账" << amount << " ？[y/n]" << std::endl;
    }


    void amountNotMultipleOf100OrRetry()
    {
        fullScreenMsg("存/取 金额必须是 100 的整数倍！");
        std::cout << "是否重新输入金额？[y/n]" << std::endl;
    }


    void exceedDailyRestrictOrRetry()
    {
        fullScreenMsg("超出单日交易限额！");
        std::cout << "当前单日交易限额为：" << Account::RESTRICT_PER_DAY << std::endl;
        std::cout << "是否重新输入金额？[y/n]" << std::endl;
    }


    void exceedSingleOpRestrictOrRetry()
    {
        fullScreenMsg("超出单次交易限额！");
        std::cout << "当前单次交易限额为：" << Account::RESTRICT_PER_OPERATION << std::endl;
        std::cout << "是否重新输入金额？[y/n]" << std::endl;
    }


    void exceedAccountBalanceOrRetry()
    {
        fullScreenMsg("余额不足！");
        std::cout << "是否重新输入金额？[y/n]" << std::endl;
    }


    void accountCreating(const std::string& number)
    {
        clearScreen();
        std::cout << "正在为账户 " << number << " 开户……" << std::endl;
    }


    void accountCreated(const Account& account)
    {
        clearScreen();
        std::cout << "***************************************" << std::endl;
        std::cout << "账户 " << account.getNumber() << " 开户成功！" << std::endl;
        std::cout << "***************************************" << std::endl;
        std::cout << "基本信息如下：" << std::endl;
        std::cout << "姓名: " << account.getName() << std::endl;
        std::cout << "身份证号: " << account.getId().getId() << std::endl;
        std::cout << "余额： " << account.getBalance() << std::endl;
        std::cout << "单次交易限额： " << Account::RESTRICT_PER_OPERATION << std::endl;
        std::cout << "单日交易限额： " << Account::RESTRICT_PER_DAY << std::endl;
        std::cout << "***************************************" << std::endl;
        std::cout << "请妥善保管好您的密码！" << std::endl;
        std::cout << "如有需要，请按回车键后重新登录。" << std::endl;
        std::cout << "***************************************" << std::endl;
    }
    

    void accountUnavailable(const Account* accountPtr)
    {
        clearScreen();
        std::cout << "账户 " << accountPtr->getNumber() << " 暂不可用，请联系工作人员……" << std::endl;
    }


    void showAccountInfo(const Account* accountPtr)
    {
        clearScreen();
        std::cout << "***************************************" << std::endl;
        std::cout << "账户 " << accountPtr->getNumber() << std::endl;
        std::cout << "***************************************" << std::endl;
        std::cout << "余额： " << accountPtr->getBalance() << std::endl;
        std::cout << "可用单日交易额度： " << Account::RESTRICT_PER_DAY - accountPtr->getUsedBalanceToday() << std::endl;
        std::cout << "***************************************" << std::endl;
    }


    bool isValidNumber()
    {
        if (tempInput.length() != Account::LENGTH_OF_ACCOUNT_NUMBER) return false;
        if (std::any_of(tempInput.begin(), tempInput.end(), [](char c) {return !std::isdigit(static_cast<unsigned char>(c));})) return false;
        return true;
    }


    bool isValidYesOrNo()
    {
        if (tempInput == "y" || tempInput == "Y" || tempInput == "n" || tempInput == "N") return true;
        return false;
    }


    bool isYes()
    {
        if (tempInput == "y" || tempInput == "Y") return true;
        if (tempInput == "n" || tempInput == "N") return false;
        return false;
    }


    bool isValidName()
    {
        if (tempInput.empty()) return false;
        if (tempInput.length() > Account::MAX_LENGTH_OF_NAME) return false;
        if (std::all_of(tempInput.begin(), tempInput.end(), [](char c) {return std::isspace(static_cast<unsigned char>(c));})) return false;
        return true;
    }


    bool isValidPassword()
    {
        if (tempInput.length() != Password::LENGTH_OF_PASSWORD) return false;
        if (std::any_of(tempInput.begin(), tempInput.end(), [](char c) {return !std::isdigit(static_cast<unsigned char>(c));})) return false;
        return true;
    }


    bool isValidId()
    {
        return Id::verify(Ui::tempInput);
    }


    bool isValidChoice()
    {
        if (tempInput.length() < 1 || tempInput.length() > MAX_LENGTH_OF_CHOICE) return false;
        if (std::any_of(tempInput.begin(), tempInput.end(), [](char c) {return !std::isdigit(static_cast<unsigned char>(c));})) return false;
        if (std::stoul(tempInput) > CHOICES_OF_MENU) return false;
        return true;
    }


    bool isValidAmount()
    {
        if (tempInput.length() < 1 || tempInput.length() > Account::MAX_LENGTH_OF_AMOUNT) return false;
        if (std::any_of(tempInput.begin(), tempInput.end(), [](char c) {return !std::isdigit(static_cast<unsigned char>(c));})) return false;
        long long userInput = std::stoll(tempInput);
        if (userInput <= 0 || userInput >= Account::MAX_AMOUNT_VALUE) return false;
        return true;
    }
}