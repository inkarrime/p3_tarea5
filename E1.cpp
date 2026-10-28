#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class TransactionManager;

class Account {
private:
    std::string id;
    double balance;

    void credit(double amount) { balance += amount; }
    void debit(double amount) { balance -= amount; }

    friend class TransactionManager;

public:
    Account(const std::string& id, double initialBalance)
        : id(id), balance(initialBalance) {}

    const std::string& getId() const { return id; }
    double getBalance() const { return balance; }
};

class Transaction {
private:
    std::string type;
    std::string accountId;
    double amount;
    bool successful;

public:
    Transaction(const std::string& type, const std::string& accountId,
                double amount, bool successful)
        : type(type), accountId(accountId), amount(amount), successful(successful) {}

    std::string toString() const {
        return type + " | " + accountId + " | " + std::to_string(amount) +
               (successful ? " | Aceptada" : " | Rechazada");
    }
};

class TransactionManager {
private:
    static TransactionManager* instance;
    static std::mutex instanceMutex;

    std::mutex operationMutex;
    std::vector<Transaction> history;

    TransactionManager() {}

public:
    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;

    static TransactionManager& getInstance() {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (instance == nullptr) {
            instance = new TransactionManager();
        }
        return *instance;
    }

    bool deposit(Account* account, double amount) {
        if (account == nullptr) return false;
        std::lock_guard<std::mutex> lock(operationMutex);
        bool ok = amount > 0;
        if (ok) {
            account->credit(amount);
        }
        history.push_back(Transaction("Deposito", account->getId(), amount, ok));
        return ok;
    }

    bool withdraw(Account* account, double amount) {
        if (account == nullptr) return false;
        std::lock_guard<std::mutex> lock(operationMutex);
        bool ok = amount > 0 && account->getBalance() >= amount;
        if (ok) {
            account->debit(amount);
        }
        history.push_back(Transaction("Retiro", account->getId(), amount, ok));
        return ok;
    }
};

TransactionManager* TransactionManager::instance = nullptr;
std::mutex TransactionManager::instanceMutex;

int main() {
    auto& manager = TransactionManager::getInstance();
    Account account("A1", 1000);
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            manager.deposit(&account, 100);
            manager.withdraw(&account, 50);
        });
    }
    for (auto& t : threads) t.join();
    std::cout << "Balance final: "
              << account.getBalance()
              << std::endl;
    std::cout << "Direccion manager: "
              << &TransactionManager::getInstance()
              << std::endl;
}
