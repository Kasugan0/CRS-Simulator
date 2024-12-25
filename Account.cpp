#include <iomanip>
#include "Account.hpp"




Id::Id(const std::string& s) : id(s) {}


bool Id::verify(const std::string& s)
{
    if (s.length() != LENGTH_OF_ID) return false;

    int sum = 0;
    for (size_t i = 0; i < LENGTH_OF_ID - 1; i++)
    {
        if (!isdigit(s[i])) return false;
        sum += (s[i] - '0') * WEIGH[i];
    }
    if (s[LENGTH_OF_ID - 1] != EXPECT[sum % 11]) return false;

    return true;
}


std::string Id::getId() const
{
    return id;
}



std::hash<std::string> Password::hasher;


Password::Password(const std::string& plaintext) : hashed(hasher(plaintext + HASH_SALT)) {}


bool Password::authenticate(const std::string& plaintext) const
{
    return hashed == hasher(plaintext + HASH_SALT);
}


void Password::changePassword(const std::string& plaintext)
{
    hashed = hasher(plaintext + HASH_SALT);
}



Account::Account
(
    const std::string& number, const std::string& name, const Id& id, const Password& password,
    const bool available, size_t wrongPasswordAttempted, const long long balance, const long long usedBalanceToday
)
: number(number), name(name), id(id), password(password),
    available(available), wrongPasswordAttempted(wrongPasswordAttempted), balance(balance), usedBalanceToday(usedBalanceToday) {}


bool Account::isAvailable()
{
    return available;
}


void Account::setAvailable(bool flag)
{
    available = flag;
}


std::string Account::getNumber() const
{
    return number;
}


std::string Account::getName() const
{
    return name;
}


Id Account::getId() const
{
    return id;
}


long long Account::getBalance() const
{
    return balance;
}


long long Account::getUsedBalanceToday() const
{
    return usedBalanceToday;
}


size_t Account::getWrongPasswordAttempted()
{
    return wrongPasswordAttempted;
}


bool Account::authenticate(const Password& givenPassword)
{
    if (!isAvailable()) return false;
    if (givenPassword.hashed == password.hashed) return true;
    if (++wrongPasswordAttempted >= MAX_WRONG_PASSWORD_ATTEMPT) setAvailable(false);
    return false;
}


void Account::deposit(long long amount)
{
    if (amount <= 0) throw std::invalid_argument("Cannot deposit invaild amount of cash.");
    else if (amount + balance < 0) throw std::runtime_error("I'd rather believe it's system error than admit a rich guy's wealth just made long long overflow!");
    balance += amount;
}


void Account::withdraw(long long amount)
{
    if (amount <= 0) throw std::invalid_argument("Cannot withdraw invaild amount of cash.");
    if (amount > balance) throw std::invalid_argument("Not enough balance for withdrawal.");
    if (amount + usedBalanceToday > Account::RESTRICT_PER_DAY) throw std::invalid_argument("Beyond RESTRICT_PER_DAY for withdrawal.");
    balance -= amount, usedBalanceToday += amount;
}


void Account::transferTo(std::string toAccNum, long long amount, Pool& pool)
{
    if (amount <= 0) throw std::invalid_argument("Cannot transfer invaild amount of cash.");
    if (amount > balance) throw std::invalid_argument("Not enough balance for transfer.");
    if (amount + usedBalanceToday > Account::RESTRICT_PER_DAY) throw std::invalid_argument("Beyond RESTRICT_PER_DAY for transfer.");
    if (!pool.contains(toAccNum)) throw std::invalid_argument("Cannot transfer to an invaild account.");

    Account* ptr = pool.getAccount(toAccNum);
    if (ptr == nullptr) throw std::invalid_argument("Account not found for transfer: " + toAccNum);

    balance -= amount, usedBalanceToday += amount;
    ptr->balance += amount;
}


void Account::changePassword(const Password& newPassword)
{
    password = newPassword;
}


void Pool::newAccount(Account* accountPtr)
{
    if (accountPtr == nullptr) throw std::invalid_argument("Cannot add a nullptr Account.");
    accounts[accountPtr->getNumber()] = accountPtr;
}


void Pool::deleteAccount(const std::string& accountNumber)
{
    auto it = accounts.find(accountNumber);
    if (it == accounts.end()) throw std::invalid_argument("Account not found for deletion: " + accountNumber);
    delete it -> second;
    accounts.erase(it);
}


size_t Pool::count() const
{
    return accounts.size();
}


bool Pool::contains(const std::string& accountNumber) const
{
    return accounts.find(accountNumber) != accounts.end();
}


Account* Pool::getAccount(const std::string& accountNumber)
{
    return accounts[accountNumber];
}


void Pool::save() const
{
    std::ofstream outputFile(OUTPUT_FILE_NAME, std::ios::app);
    std::ios_base::fmtflags orig = outputFile.flags();
    if (!outputFile) throw std::runtime_error("Failed to open file "+ OUTPUT_FILE_NAME);
    for (const auto& i : accounts)
    {
        auto saving = i.second;
        outputFile << "Account:\t"  << saving->getNumber() << std::endl;
        outputFile << "Name:\t\t"   << saving->getName() << std::endl;
        outputFile << "ID:\t\t\t"   << saving->getId().getId() << std::endl;
        outputFile << "Password:\t" << std::hex << std::setw(sizeof(size_t) * 2) << std::setfill('0') << saving->password.hashed << std::endl;
        outputFile.flags(orig);
        outputFile << "Balance\t\t" << saving->getBalance() << std::endl;
        outputFile << std::endl;
    }
}


Pool::~Pool()
{
    for (auto& i : accounts) delete i.second;
}