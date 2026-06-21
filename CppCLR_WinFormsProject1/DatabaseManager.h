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
            msclr::interop::marshal_context context;
            return context.marshal_as<std::string>(managedStr);
        }

        static String^ toManaged(const char* nativeStr) {
            if (nativeStr == nullptr) return "";
            return gcnew String(nativeStr);
        }

    public:
        // Инициализация таблиц
        static void InitializeDatabase() {
            sqlite3* db;
            if (sqlite3_open("bank.db", &db) == SQLITE_OK) {
                const char* sql =
                    "CREATE TABLE IF NOT EXISTS Clients (Id TEXT PRIMARY KEY, FullName TEXT, Phone TEXT);"
                    "CREATE TABLE IF NOT EXISTS Accounts (Number TEXT PRIMARY KEY, ClientId TEXT, Balance REAL, Type TEXT);"
                    "CREATE TABLE IF NOT EXISTS Transactions (AccountNumber TEXT, Date TEXT, Type TEXT, Amount REAL);";
                sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
                sqlite3_close(db);
            }
        }

        // Сохранение всех данных (полная перезапись базы)
        static void SaveAllData(List<Client^>^ clients) {
            sqlite3* db;
            if (sqlite3_open("bank.db", &db) != SQLITE_OK) return;

            sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
            sqlite3_exec(db, "DELETE FROM Clients; DELETE FROM Accounts; DELETE FROM Transactions;", nullptr, nullptr, nullptr);

            sqlite3_stmt* stmtC, * stmtA, * stmtT;
            sqlite3_prepare_v2(db, "INSERT INTO Clients VALUES (?, ?, ?);", -1, &stmtC, nullptr);
            sqlite3_prepare_v2(db, "INSERT INTO Accounts VALUES (?, ?, ?, ?);", -1, &stmtA, nullptr);
            sqlite3_prepare_v2(db, "INSERT INTO Transactions VALUES (?, ?, ?, ?);", -1, &stmtT, nullptr);

            for each(Client ^ client in clients) {
                sqlite3_bind_text(stmtC, 1, toNative(client->Id).c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtC, 2, toNative(client->FullName).c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmtC, 3, toNative(client->Phone).c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(stmtC);
                sqlite3_reset(stmtC);

                for each(BankAccount ^ acc in client->Accounts) {
                    sqlite3_bind_text(stmtA, 1, toNative(acc->Number).c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_text(stmtA, 2, toNative(client->Id).c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_double(stmtA, 3, acc->Balance);

                    // Унифицируем тип перед сохранением в БД, чтобы там всегда был чистый английский текст низким регистром
                    String^ rawType = acc->GetAccountType();
                    String^ dbType = "debit";

                    if (rawType != nullptr) {
                        rawType = rawType->Trim()->ToLower();
                        if (rawType == "savings" || rawType == L"сберегательный") dbType = "savings";
                        else if (rawType == "credit" || rawType == L"кредитный") dbType = "credit";
                    }

                    sqlite3_bind_text(stmtA, 4, toNative(dbType).c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_step(stmtA);
                    sqlite3_reset(stmtA);

                    for each(BankTransaction ^ tx in acc->History) {
                        sqlite3_bind_text(stmtT, 1, toNative(acc->Number).c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmtT, 2, toNative(tx->Date).c_str(), -1, SQLITE_TRANSIENT);
                        sqlite3_bind_text(stmtT, 3, toNative(tx->Type).c_str(), -1, SQLITE_TRANSIENT);
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

        // Загрузка всех данных
        static List<Client^>^ LoadAllData() {
            List<Client^>^ clients = gcnew List<Client^>();
            sqlite3* db;
            if (sqlite3_open("bank.db", &db) != SQLITE_OK) return clients;

            sqlite3_stmt* stmtC, * stmtA, * stmtT;
            sqlite3_prepare_v2(db, "SELECT * FROM Clients;", -1, &stmtC, nullptr);
            sqlite3_prepare_v2(db, "SELECT Number, Balance, Type FROM Accounts WHERE ClientId = ?;", -1, &stmtA, nullptr);
            sqlite3_prepare_v2(db, "SELECT Date, Type, Amount FROM Transactions WHERE AccountNumber = ?;", -1, &stmtT, nullptr);

            while (sqlite3_step(stmtC) == SQLITE_ROW) {
                String^ id = toManaged((const char*)sqlite3_column_text(stmtC, 0));
                String^ name = toManaged((const char*)sqlite3_column_text(stmtC, 1));
                String^ phone = toManaged((const char*)sqlite3_column_text(stmtC, 2));
                Client^ client = gcnew Client(id, name, phone);

                sqlite3_bind_text(stmtA, 1, toNative(id).c_str(), -1, SQLITE_TRANSIENT);
                while (sqlite3_step(stmtA) == SQLITE_ROW) {
                    String^ num = toManaged((const char*)sqlite3_column_text(stmtA, 0));
                    double bal = sqlite3_column_double(stmtA, 1);
                    String^ type = toManaged((const char*)sqlite3_column_text(stmtA, 2));

                    if (type != nullptr) {
                        type = type->Trim()->ToLower();
                    }

                    BankAccount^ acc;
                    // Распознаем любые вариации написания
                    if (type == "debit" || type == L"дебетовый") {
                        acc = gcnew DebitAccount(num, bal);
                    }
                    else if (type == "savings" || type == L"сберегательный") {
                        acc = gcnew SavingsAccount(num, bal);
                    }
                    else if (type == "credit" || type == L"кредитный") {
                        acc = gcnew CreditAccount(num, bal);
                    }
                    else {
                        acc = gcnew DebitAccount(num, bal);
                    }

                    sqlite3_bind_text(stmtT, 1, toNative(num).c_str(), -1, SQLITE_TRANSIENT);
                    while (sqlite3_step(stmtT) == SQLITE_ROW) {
                        acc->History->Add(gcnew BankTransaction(
                            toManaged((const char*)sqlite3_column_text(stmtT, 0)),
                            toManaged((const char*)sqlite3_column_text(stmtT, 1)),
                            sqlite3_column_double(stmtT, 2)
                        ));
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