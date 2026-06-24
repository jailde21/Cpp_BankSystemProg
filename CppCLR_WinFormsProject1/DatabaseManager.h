#pragma once
#include "sqlite3.h"
#include <string>
#include <msclr/marshal_cppstd.h>
#include "BankCore.h"

namespace CppCLRWinFormsProject {

    using namespace System;
    using namespace System::Collections::Generic;

    public ref class DatabaseManager {
    private:
        
        static std::string toNative(String^ managedStr) {
            if (managedStr == nullptr) return "";
            return msclr::interop::marshal_as<std::string>(managedStr);
        }

        static String^ toManaged(const char* nativeStr) {
            if (nativeStr == nullptr) return "";
            return gcnew String(nativeStr);
        }

    public:
        // Инициализация базы данных и создание индексов для ускорения выборок
        static void InitializeDatabase() {
            sqlite3* db;
            if (sqlite3_open("bank.db", &db) == SQLITE_OK) {
                const char* sql =
                    "CREATE TABLE IF NOT EXISTS Clients (Id TEXT PRIMARY KEY, FullName TEXT, Phone TEXT);"
                    "CREATE TABLE IF NOT EXISTS Accounts (Number TEXT PRIMARY KEY, ClientId TEXT, Balance REAL, Type TEXT);"
                    "CREATE TABLE IF NOT EXISTS Transactions (AccountNumber TEXT, Date TEXT, Type TEXT, Amount REAL);"
                    "CREATE INDEX IF NOT EXISTS idx_accounts_client ON Accounts (ClientId);"
                    "CREATE INDEX IF NOT EXISTS idx_transactions_account ON Transactions (AccountNumber);";

                sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
                sqlite3_close(db);
            }
        }

        // Сохранение всех данных 
        static void SaveAllData(List<Client^>^ clients) {
            sqlite3* db;
            if (sqlite3_open("bank.db", &db) != SQLITE_OK) return;

            sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
            sqlite3_exec(db, "DELETE FROM Clients; DELETE FROM Accounts; DELETE FROM Transactions;", nullptr, nullptr, nullptr);

            sqlite3_stmt* stmtC = nullptr, * stmtA = nullptr, * stmtT = nullptr;

            if (sqlite3_prepare_v2(db, "INSERT INTO Clients VALUES (?, ?, ?);", -1, &stmtC, nullptr) != SQLITE_OK ||
                sqlite3_prepare_v2(db, "INSERT INTO Accounts VALUES (?, ?, ?, ?);", -1, &stmtA, nullptr) != SQLITE_OK ||
                sqlite3_prepare_v2(db, "INSERT INTO Transactions VALUES (?, ?, ?, ?);", -1, &stmtT, nullptr) != SQLITE_OK) {

                if (stmtC) sqlite3_finalize(stmtC);
                if (stmtA) sqlite3_finalize(stmtA);
                if (stmtT) sqlite3_finalize(stmtT);
                sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
                sqlite3_close(db);
                return;
            }

            for each(Client ^ client in clients) {
                std::string cId = toNative(client->Id);
                std::string cName = toNative(client->FullName);
                std::string cPhone = toNative(client->Phone);

                sqlite3_bind_text(stmtC, 1, cId.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtC, 2, cName.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtC, 3, cPhone.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(stmtC);
                sqlite3_reset(stmtC);

                for each(BankAccount ^ acc in client->Accounts) {
                    std::string aNum = toNative(acc->Number);
                    std::string aType = toNative(acc->GetAccountType());

                    sqlite3_bind_text(stmtA, 1, aNum.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(stmtA, 2, cId.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_double(stmtA, 3, acc->Balance);
                    sqlite3_bind_text(stmtA, 4, aType.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_step(stmtA);
                    sqlite3_reset(stmtA);

                    for each(BankTransaction ^ tx in acc->History) {
                        std::string tDate = toNative(tx->Date);
                        std::string tType = toNative(tx->Type);

                        sqlite3_bind_text(stmtT, 1, aNum.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmtT, 2, tDate.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmtT, 3, tType.c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_double(stmtT, 4, tx->Amount);
                        sqlite3_step(stmtT);
                        sqlite3_reset(stmtT);
                    }
                }
            }

            sqlite3_finalize(stmtC);
            sqlite3_finalize(stmtA);
            sqlite3_finalize(stmtT);
            sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
            sqlite3_close(db);
        }

        // Загрузка всех данных из базы
        static List<Client^>^ LoadAllData() {
            List<Client^>^ clients = gcnew List<Client^>();
            sqlite3* db;
            if (sqlite3_open("bank.db", &db) != SQLITE_OK) return clients;

            sqlite3_stmt* stmtC = nullptr, * stmtA = nullptr, * stmtT = nullptr;

            if (sqlite3_prepare_v2(db, "SELECT * FROM Clients;", -1, &stmtC, nullptr) != SQLITE_OK ||
                sqlite3_prepare_v2(db, "SELECT Number, Balance, Type FROM Accounts WHERE ClientId = ?;", -1, &stmtA, nullptr) != SQLITE_OK ||
                sqlite3_prepare_v2(db, "SELECT Date, Type, Amount FROM Transactions WHERE AccountNumber = ?;", -1, &stmtT, nullptr) != SQLITE_OK) {

                if (stmtC) sqlite3_finalize(stmtC);
                if (stmtA) sqlite3_finalize(stmtA);
                if (stmtT) sqlite3_finalize(stmtT);
                sqlite3_close(db);
                return clients;
            }

            while (sqlite3_step(stmtC) == SQLITE_ROW) {
                String^ id = toManaged((const char*)sqlite3_column_text(stmtC, 0));
                String^ name = toManaged((const char*)sqlite3_column_text(stmtC, 1));
                String^ phone = toManaged((const char*)sqlite3_column_text(stmtC, 2));
                Client^ client = gcnew Client(id, name, phone);

                std::string nativeId = toNative(id);
                sqlite3_bind_text(stmtA, 1, nativeId.c_str(), -1, SQLITE_TRANSIENT);

                while (sqlite3_step(stmtA) == SQLITE_ROW) {
                    String^ num = toManaged((const char*)sqlite3_column_text(stmtA, 0));
                    double bal = sqlite3_column_double(stmtA, 1);
                    String^ type = toManaged((const char*)sqlite3_column_text(stmtA, 2));

                    if (type != nullptr) {
                        type = type->Trim()->ToLower();
                    }

                    // Четкое разделение типов
                    BankAccount^ acc;
                    if (type == "savings") {
                        acc = gcnew SavingsAccount(num, bal);
                    }
                    else if (type == "credit") {
                        acc = gcnew CreditAccount(num, bal);
                    }
                    else {
                        
                        acc = gcnew DebitAccount(num, bal);
                    }

                    std::string nativeNum = toNative(num);
                    sqlite3_bind_text(stmtT, 1, nativeNum.c_str(), -1, SQLITE_TRANSIENT);

                    while (sqlite3_step(stmtT) == SQLITE_ROW) {
                        
                        String^ tDate = toManaged((const char*)sqlite3_column_text(stmtT, 0));
                        String^ tType = toManaged((const char*)sqlite3_column_text(stmtT, 1));
                        double tAmount = sqlite3_column_double(stmtT, 2);

                        acc->History->Add(gcnew BankTransaction(tDate, tType, tAmount));
                    }
                    sqlite3_reset(stmtT);
                    client->Accounts->Add(acc);
                }
                sqlite3_reset(stmtA);
                clients->Add(client);
            }

            sqlite3_finalize(stmtC);
            sqlite3_finalize(stmtA);
            sqlite3_finalize(stmtT);
            sqlite3_close(db);
            return clients;
        }
    };
}