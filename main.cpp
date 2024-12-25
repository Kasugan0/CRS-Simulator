#include "Account.hpp"
#include "Ui.hpp"



int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    Pool pool;
    

    while (true)
    {
        std::string accountNumber;
        Ui::tillValid(Ui::welcome, Ui::isValidNumber, accountNumber);
        // if (accountNumber == "0000000000000000000") break; // Use this to debug.
        if (!pool.contains(accountNumber))
        {
            Ui::tillValid(Ui::fullScreenMsg, "未找到此账户，是否使用此账号开户？[y/n]", Ui::isValidYesOrNo);
            if (!Ui::isYes())
            {
                Ui::clearData();
                Ui::clearScreen();
                continue;
            }
            Ui::accountCreating(accountNumber);
            Ui::clearData();
            Ui::pause();

            std::string newName;
            Ui::tillValid(Ui::fullScreenMsg, "请输入您的真实姓名：", Ui::isValidName, newName);

            std::string idstring;
            Ui::tillValid(Ui::fullScreenMsg, "请输入您的身份证号： ", Ui::isValidId, idstring);
            Id newId(idstring);

            Password newPassword;
            Ui::doubleCheckPassword(newPassword);

            Account newAccount(accountNumber, newName, newId, newPassword);
            pool.newAccount(&newAccount);
            Ui::accountCreated(newAccount);

            Ui::clearData();
            Ui::pause();
        }
        else
        {
            Account* currentAccount = pool.getAccount(accountNumber);
            if (!currentAccount->isAvailable())
            {
                Ui::accountUnavailable(currentAccount);
                Ui::pause();
                Ui::clearData();
                Ui::clearScreen();
                continue;
            }
            if (!Ui::authenticateAccount(currentAccount))
            {
                Ui::fullScreenMsg("登录失败！");
                Ui::pause();
                Ui::clearData();
                Ui::clearScreen();
                continue;
            }
            bool quit = false;
            while (!quit)
            {
                std::string userChoice;
                Ui::tillValid(Ui::menu, Ui::isValidChoice, userChoice);
                size_t choice = std::stoul(userChoice);
                switch (choice)
                {
                case 0:
                    quit = true;
                    Ui::fullScreenMsg("感谢使用！");
                    break;
                case 1:
                    Ui::showAccountInfo(currentAccount);
                    break;
                case 2:
                    if (!Ui::deposit(currentAccount)) Ui::fullScreenMsg("存款失败！");
                    break;
                case 3:
                    if (!Ui::withdraw(currentAccount)) Ui::fullScreenMsg("取款失败！");
                    break;
                case 4:
                    if (!Ui::transfer(currentAccount, pool)) Ui::fullScreenMsg("转账失败！");
                    break;
                case 5:
                    Password newPassword;
                    Ui::doubleCheckPassword(newPassword);
                    currentAccount->changePassword(newPassword);
                default:
                    break;
                }
                Ui::pause();
            }
            Ui::clearData();
            Ui::clearScreen();
        }
    }
    pool.save();
    return 0;
}