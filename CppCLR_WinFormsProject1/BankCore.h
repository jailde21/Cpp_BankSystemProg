#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace CppCLRWinFormsProject {

    public ref class BankTransaction {
    public:
        String^ Date;
        String^ Type;
        double Amount;

        BankTransaction(String^ date, String^ type, double amount) {
            Date = date;
            Type = type;
            Amount = amount;
        }
    };

    public ref class BankAccount abstract {
    public:
        String^ Number;
        double Balance;
        List<BankTransaction^>^ History;

        BankAccount(String^ num, double bal) {
            Number = num;
            Balance = bal;
            History = gcnew List<BankTransaction^>();
        }

        // Этот метод используется для логики и сохранения в БД (всегда lowercase english)
        virtual String^ GetAccountType() abstract;

        // Добавим свойство для красивого отображения на форме (UI)
        virtual property String^ DisplayName{
            String ^ get() abstract;
        }
    };

    public ref class DebitAccount : public BankAccount {
    public:
        DebitAccount(String^ num, double bal) : BankAccount(num, bal) {}
        virtual String^ GetAccountType() override { return "debit"; }

        virtual property String^ DisplayName{
            String ^ get() override { return L"Дебетовый"; }
        }
    };

    public ref class SavingsAccount : public BankAccount {
    public:
        SavingsAccount(String^ num, double bal) : BankAccount(num, bal) {}
        virtual String^ GetAccountType() override { return "savings"; }

        virtual property String^ DisplayName{
            String ^ get() override { return L"Сберегательный"; }
        }
    };

    public ref class CreditAccount : public BankAccount {
    public:
        CreditAccount(String^ num, double bal) : BankAccount(num, bal) {}
        virtual String^ GetAccountType() override { return "credit"; }

        virtual property String^ DisplayName{
            String ^ get() override { return L"Кредитный"; }
        }
    };

    public ref class Client {
    public:
        String^ Id;
        String^ FullName;
        String^ Phone;
        List<BankAccount^>^ Accounts;

        Client(String^ id, String^ name, String^ phone) {
            Id = id;
            FullName = name;
            Phone = phone;
            Accounts = gcnew List<BankAccount^>();
        }
    };
}