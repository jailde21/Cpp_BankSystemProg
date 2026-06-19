#pragma once

// Нативные инклуды строго на самом верху (до System)!
#include "sqlite3.h"
#include <string>
#include <msclr/marshal_cppstd.h>

// Подключаем классы нашего банка (Client, Account и т.д.)
#include "BankCore.h"

namespace CppCLRWinFormsProject {

    using namespace System;
    using namespace System::Collections::Generic;

    public ref class DatabaseManager {
    private:
        static std::string toNativeString(String^ managedStr) {
            if (managedStr == nullptr) return "";
            msclr::interop::marshal_context context;
            return context.marshal_as<std::string>(managedStr);
        }

        static String^ toManagedString(const char* nativeStr) {
            if (nativeStr == nullptr) return "";
            return gcnew String(nativeStr);
        }

    public:
        static void InitializeDatabase() {
            sqlite3* db;
            if (sqlite3_open("bank.db", &db) == SQLITE_OK) {
                const char* createClients =
                    "CREATE TABLE IF NOT EXISTS Clients ("
                    "Id TEXT PRIMARY KEY, "
                    "FullName TEXT, "
                    "Phone TEXT);";
                sqlite3_exec(db, createClients, nullptr, nullptr, nullptr);

                const char* createAccounts =
                    "CREATE TABLE IF NOT EXISTS Accounts ("
                    "Number TEXT PRIMARY KEY, "
                    "ClientId TEXT, "
                    "Balance REAL, "
                    "Type TEXT, "
                    "FOREIGN KEY(ClientId) REFERENCES Clients(Id));";
                sqlite3_exec(db, createAccounts, nullptr, nullptr, nullptr);

                const char* createTransactions =
                    "CREATE TABLE IF NOT EXISTS Transactions ("
                    "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "AccountNumber TEXT, "
                    "Date TEXT, "
                    "Type TEXT, "
                    "Amount REAL, "
                    "FOREIGN KEY(AccountNumber) REFERENCES Accounts(Number));";
                sqlite3_exec(db, createTransactions, nullptr, nullptr, nullptr);
            }
            sqlite3_close(db);
        }

        static void SaveAllData(List<Client^>^ clients) {
            sqlite3* db;
            if (sqlite3_open("bank.db", &db) != SQLITE_OK) {
                sqlite3_close(db);
                return;
            }

            sqlite3_exec(db, "DELETE FROM Transactions;", nullptr, nullptr, nullptr);
            sqlite3_exec(db, "DELETE FROM Accounts;", nullptr, nullptr, nullptr);
            sqlite3_exec(db, "DELETE FROM Clients;", nullptr, nullptr, nullptr);

            sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

            sqlite3_stmt* stmtClient;
            sqlite3_prepare_v2(db, "INSERT INTO Clients (Id, FullName, Phone) VALUES (?, ?, ?);", -1, &stmtClient, nullptr);

            sqlite3_stmt* stmtAcc;
            sqlite3_prepare_v2(db, "INSERT INTO Accounts (Number, ClientId, Balance, Type) VALUES (?, ?, ?, ?);", -1, &stmtAcc, nullptr);

            sqlite3_stmt* stmtTx;
            sqlite3_prepare_v2(db, "INSERT INTO Transactions (AccountNumber, Date, Type, Amount) VALUES (?, ?, ?, ?);", -1, &stmtTx, nullptr);

            for each (Client ^ client in clients) {
                std::string cId = toNativeString(client->Id);
                std::string cName = toNativeString(client->FullName);
                std::string cPhone = toNativeString(client->Phone);

                sqlite3_bind_text(stmtClient, 1, cId.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtClient, 2, cName.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtClient, 3, cPhone.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(stmtClient);
                sqlite3_reset(stmtClient);

                for each (BankAccount ^ acc in client->Accounts) {
                    std::string aNum = toNativeString(acc->Number);
                    std::string aType = toNativeString(acc->GetAccountType());

                    sqlite3_bind_text(stmtAcc, 1, aNum.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(stmtAcc, 2, cId.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_double(stmtAcc, 3, acc->Balance);
                    sqlite3_bind_text(stmtAcc, 4, aType.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_step(stmtAcc);
                    sqlite3_reset(stmtAcc);

                    for each (BankTransaction ^ tx in acc->History) {
                        std::string tDate = toNativeString(tx->Date);
                        std::string tType = toNativeString(tx->Type);

                        sqlite3_bind_text(stmtTx, 1, aNum.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmtTx, 2, tDate.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmtTx, 3, tType.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_double(stmtTx, 4, tx->Amount);
                        sqlite3_step(stmtTx);
                        sqlite3_reset(stmtTx);
                    }
                }
            }

            sqlite3_finalize(stmtClient);
            sqlite3_finalize(stmtAcc);
            sqlite3_finalize(stmtTx);

            sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
            sqlite3_close(db);
        }

        static List<Client^>^ LoadAllData() {
            List<Client^>^ clients = gcnew List<Client^>();
            sqlite3* db;

            if (sqlite3_open("bank.db", &db) != SQLITE_OK) {
                sqlite3_close(db);
                return clients;
            }

            sqlite3_stmt* stmtClients;
            if (sqlite3_prepare_v2(db, "SELECT Id, FullName, Phone FROM Clients;", -1, &stmtClients, nullptr) == SQLITE_OK) {
                while (sqlite3_step(stmtClients) == SQLITE_ROW) {
                    String^ id = toManagedString((const char*)sqlite3_column_text(stmtClients, 0));
                    String^ name = toManagedString((const char*)sqlite3_column_text(stmtClients, 1));
                    String^ phone = toManagedString((const char*)sqlite3_column_text(stmtClients, 2));

                    clients->Add(gcnew Client(id, name, phone));
                }
            }
            sqlite3_finalize(stmtClients);

            sqlite3_stmt* stmtAcc;
            sqlite3_stmt* stmtTx;

            sqlite3_prepare_v2(db, "SELECT Number, Balance, Type FROM Accounts WHERE ClientId = ?;", -1, &stmtAcc, nullptr);
            sqlite3_prepare_v2(db, "SELECT Date, Type, Amount FROM Transactions WHERE AccountNumber = ?;", -1, &stmtTx, nullptr);

            for each (Client ^ client in clients) {
                std::string cId = toNativeString(client->Id);
                sqlite3_bind_text(stmtAcc, 1, cId.c_str(), -1, SQLITE_TRANSIENT);

                while (sqlite3_step(stmtAcc) == SQLITE_ROW) {
                    String^ num = toManagedString((const char*)sqlite3_column_text(stmtAcc, 0));
                    double bal = sqlite3_column_double(stmtAcc, 1);
                    String^ type = toManagedString((const char*)sqlite3_column_text(stmtAcc, 2));

                    BankAccount^ acc;
                    if (type == L"Дебетовый") acc = gcnew DebitAccount(num, bal);
                    else if (type == L"Сберегательный") acc = gcnew SavingsAccount(num, bal);
                    else acc = gcnew CreditAccount(num, bal);

                    std::string aNum = toNativeString(acc->Number);
                    sqlite3_bind_text(stmtTx, 1, aNum.c_str(), -1, SQLITE_TRANSIENT);

                    while (sqlite3_step(stmtTx) == SQLITE_ROW) {
                        String^ date = toManagedString((const char*)sqlite3_column_text(stmtTx, 0));
                        String^ tType = toManagedString((const char*)sqlite3_column_text(stmtTx, 1));
                        double amount = sqlite3_column_double(stmtTx, 2);

                        acc->History->Add(gcnew BankTransaction(date, tType, amount));
                    }
                    sqlite3_reset(stmtTx);

                    client->Accounts->Add(acc);
                }
                sqlite3_reset(stmtAcc);
            }

            sqlite3_finalize(stmtAcc);
            sqlite3_finalize(stmtTx);
            sqlite3_close(db);

            return clients;
        }
    };
}