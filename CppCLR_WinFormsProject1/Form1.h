#pragma once

// Подключаем БД 
#include "DatabaseManager.h"

#include "BankCore.h"
#include "AddClientForm.h"
#include "AccountOperationForm.h"
#include "sqlite3.h"
#include "StatsManager.h"

namespace CppCLRWinFormsProject {
    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;

    public ref class Form1 : public System::Windows::Forms::Form
    {
    public:
        List<Client^>^ allClients;
    private:
        Client^ currentClient;

        // Базовые панели распределения интерфейса
        Panel^ panelSidebar;
        Panel^ panelContentArea;

        // Общие элементы бокового меню
        Label^ lblMenuTitle;
        Button^ btnDashboard;
        Button^ btnClients;
        Button^ btnStats;
        Button^ btnAbout;

        // ВКЛАДКА 1: ДАШБОРД
        Panel^ panelDashboardView;
        Label^ lblLogoDash;
        TextBox^ txtSearchDash;
        Button^ btnSearchDash;
        Panel^ cardClientInfo;
        Label^ lblInfoTitle;
        Label^ lblClientFIO;
        Label^ lblClientID;
        Label^ lblClientPhone;
        Label^ lblTotalBalance;
        DataGridView^ gridAccounts;
        Panel^ panelHistoryTools;
        ComboBox^ cbSortCriterion;
        ComboBox^ cbFilterType;
        ListBox^ listHistory;
        Button^ btnDeposit;
        Button^ btnWithdraw;
        Button^ btnInterest;
        Button^ btnNewAccount;

        // ВКЛАДКА 2: КЛИЕНТЫ
        Panel^ panelClientsView;
        Label^ lblLogoClients;
        TextBox^ txtSearchClients;
        Button^ btnSearchClients;
        DataGridView^ gridAllClients;
        Button^ btnAddClient;
        Button^ btnDeleteClient;

        // ВКЛАДКА 3: СТАТИСТИКА
        Panel^ panelStatsView;

        // ВКЛАДКА 4: О ПРОГРАММЕ
        Panel^ panelAboutView;
        Panel^ cardAboutInfo;
        Panel^ cardAboutHelp;

    public:
        Form1(void)
        {
            InitializeComponent();
            allClients = gcnew List<Client^>();

            // 1. ИСПРАВЛЕНО: Грузим реальные данные из SQLite
            try {
                DatabaseManager::InitializeDatabase();
                allClients = DatabaseManager::LoadAllData();
            }
            catch (Exception^) {}

            // Если база пуста (первый запуск), создаем мок-данные и сразу сохраняем
            if (allClients == nullptr || allClients->Count == 0) {
                if (allClients == nullptr) allClients = gcnew List<Client^>();
                // CreateMockData();
                DatabaseManager::SaveAllData(allClients);
            }

            // Привязки кликов для навигации
            this->btnDashboard->Click += gcnew System::EventHandler(this, &Form1::OnDashboardClick);
            this->btnClients->Click += gcnew System::EventHandler(this, &Form1::OnClientsClick);
            this->btnStats->Click += gcnew System::EventHandler(this, &Form1::OnStatsClick);
            this->btnAbout->Click += gcnew System::EventHandler(this, &Form1::OnAboutClick);

            // Привязка обработчиков для ПОИСКА
            this->btnSearchDash->Click += gcnew System::EventHandler(this, &Form1::OnSearchDashClick);
            this->btnSearchClients->Click += gcnew System::EventHandler(this, &Form1::OnSearchClientsClick);

            // Регистрация событий кнопок действий
            this->btnAddClient->Click += gcnew System::EventHandler(this, &Form1::OnAddClientClick);
            this->btnDeleteClient->Click += gcnew System::EventHandler(this, &Form1::OnDeleteClientClick);
            this->gridAllClients->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::GridAllClients_CellDoubleClick);

            // Связываем кнопки операций
            this->btnInterest->Click += gcnew System::EventHandler(this, &Form1::btnAddInterest_Click);
            this->btnWithdraw->Click += gcnew System::EventHandler(this, &Form1::btnWithdraw_Click);
            this->btnDeposit->Click += gcnew System::EventHandler(this, &Form1::btnDeposit_Click);
            this->btnNewAccount->Click += gcnew System::EventHandler(this, &Form1::btnOpenAccount_Click);

            // Обработчики изменения критериев сортировки и фильтрации
            this->cbSortCriterion->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::OnHistoryToolsChanged);
            this->cbFilterType->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::OnHistoryToolsChanged);

            // Подписка на ручную отрисовку плавных границ у прозрачных кнопок меню
            this->btnDashboard->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::OnButtonPaint);
            this->btnClients->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::OnButtonPaint);
            this->btnStats->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::OnButtonPaint);
            this->btnAbout->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::OnButtonPaint);

            // Скругление элементов
            this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);

            // Первоначальный вывод данных в таблицы
            if (allClients->Count > 0) {
                currentClient = allClients[0];
                ShowClientData(currentClient);
            }
            LoadAllClientsGrid();

            // Стартовое состояние вкладок
            panelDashboardView->Visible = true;
            panelClientsView->Visible = false;
            panelStatsView->Visible = false;
            panelAboutView->Visible = false;
        }

        static int CompareByDateDesc(BankTransaction^ a, BankTransaction^ b) {
            DateTime dateA, dateB;
            bool parseA = DateTime::TryParse(a->Date, dateA);
            bool parseB = DateTime::TryParse(b->Date, dateB);
            if (parseA && parseB) return dateB.CompareTo(dateA);
            return b->Date->CompareTo(a->Date);
        }
        static int CompareByDateAsc(BankTransaction^ a, BankTransaction^ b) {
            DateTime dateA, dateB;
            bool parseA = DateTime::TryParse(a->Date, dateA);
            bool parseB = DateTime::TryParse(b->Date, dateB);
            if (parseA && parseB) return dateA.CompareTo(dateB);
            return a->Date->CompareTo(b->Date);
        }
        static int CompareByAmountDesc(BankTransaction^ a, BankTransaction^ b) {
            return b->Amount.CompareTo(a->Amount);
        }
        static int CompareByAmountAsc(BankTransaction^ a, BankTransaction^ b) {
            return a->Amount.CompareTo(b->Amount);
        }

    private:
        void OnSearchDashClick(System::Object^ sender, System::EventArgs^ e) {
            String^ searchQuery = txtSearchDash->Text->Trim();
            if (String::IsNullOrEmpty(searchQuery)) {
                MessageBox::Show(L"Введите ID клиента для поиска!", L"Внимание", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            Client^ foundClient = nullptr;
            for each(Client ^ cl in allClients) {
                if (cl->Id->Equals(searchQuery, StringComparison::OrdinalIgnoreCase)) {
                    foundClient = cl;
                    break;
                }
            }

            if (foundClient != nullptr) {
                ShowClientData(foundClient);
            }
            else {
                MessageBox::Show(L"Клиент с ID '" + searchQuery + L"' не найден в системе.", L"Результаты поиска", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
        }

        void OnSearchClientsClick(System::Object^ sender, System::EventArgs^ e) {
            String^ searchQuery = txtSearchClients->Text->Trim();
            LoadAllClientsGrid(searchQuery);
        }

        System::Void btnAddInterest_Click(System::Object^ sender, System::EventArgs^ e) {
            if (currentClient == nullptr) { MessageBox::Show(L"Выберите клиента!", L"Внимание", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
            AccountOperationForm^ form = gcnew AccountOperationForm(AccountOpType::Interest, L"Сберегательный");
            if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                double pct = Double::Parse(form->EnteredAmount);
                bool applied = false;

                String^ selectedType = form->SelectedAccountType;
                if (selectedType != nullptr) selectedType = selectedType->Trim()->ToLower();

                for each(BankAccount ^ acc in currentClient->Accounts) {
                    String^ accType = acc->GetAccountType();
                    if (accType != nullptr) accType = accType->Trim()->ToLower();

                    if (accType == selectedType || (accType == L"сберегательный" && selectedType == "savings")) {
                        double interest = acc->Balance * (pct / 100.0);
                        acc->Balance += interest;
                        acc->History->Add(gcnew BankTransaction(DateTime::Now.ToString("yyyy-MM-dd"), L"Проценты", interest));
                        applied = true;
                    }
                }
                if (applied) {
                    DatabaseManager::SaveAllData(allClients);
                    ShowClientData(currentClient);
                    MessageBox::Show(L"Проценты успешно начислены!", L"Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
                }
                else {
                    MessageBox::Show(L"У клиента нет счетов выбранного типа!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                }
            }
        }

        System::Void btnWithdraw_Click(System::Object^ sender, System::EventArgs^ e) {
            if (currentClient == nullptr) { MessageBox::Show(L"Выберите клиента!", L"Внимание", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
            if (gridAccounts->CurrentRow == nullptr || gridAccounts->CurrentRow->Index < 0) { MessageBox::Show(L"Выберите конкретный счет в таблице!", L"Внимание", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }

            String^ accNum = gridAccounts->CurrentRow->Cells[1]->Value->ToString();
            BankAccount^ targetAcc = nullptr;
            for each(BankAccount ^ acc in currentClient->Accounts) {
                if (acc->Number == accNum) { targetAcc = acc; break; }
            }
            if (targetAcc == nullptr) return;

            AccountOperationForm^ form = gcnew AccountOperationForm(AccountOpType::Withdraw, targetAcc->GetAccountType());
            if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                double amt = Double::Parse(form->EnteredAmount);
                if (targetAcc->Balance >= amt || targetAcc->GetAccountType() == L"Кредитный") {
                    targetAcc->Balance -= amt;
                    targetAcc->History->Add(gcnew BankTransaction(DateTime::Now.ToString("yyyy-MM-dd"), L"Снятие", amt));

                    DatabaseManager::SaveAllData(allClients);
                    ShowClientData(currentClient);
                    MessageBox::Show(L"Сумма успешно списана!", L"Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
                }
                else {
                    MessageBox::Show(L"Недостаточно средств!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }
        }

        System::Void btnDeposit_Click(System::Object^ sender, System::EventArgs^ e) {
            if (currentClient == nullptr) { MessageBox::Show(L"Выберите клиента!", L"Внимание", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
            if (gridAccounts->CurrentRow == nullptr || gridAccounts->CurrentRow->Index < 0) { MessageBox::Show(L"Выберите счет в таблице!", L"Внимание", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }

            String^ accNum = gridAccounts->CurrentRow->Cells[1]->Value->ToString();
            BankAccount^ targetAcc = nullptr;
            for each(BankAccount ^ acc in currentClient->Accounts) {
                if (acc->Number == accNum) { targetAcc = acc; break; }
            }
            if (targetAcc == nullptr) return;

            AccountOperationForm^ form = gcnew AccountOperationForm(AccountOpType::Deposit, nullptr);
            if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                double amt = Double::Parse(form->EnteredAmount);
                targetAcc->Balance += amt;
                targetAcc->History->Add(gcnew BankTransaction(DateTime::Now.ToString("yyyy-MM-dd"), L"Пополнение", amt));

                DatabaseManager::SaveAllData(allClients);
                ShowClientData(currentClient);
                MessageBox::Show(L"Счет пополнен!", L"Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
        }

        System::Void btnOpenAccount_Click(System::Object^ sender, System::EventArgs^ e) {
            if (currentClient == nullptr) {
                MessageBox::Show(L"Выберите клиента!", L"Внимание", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            AccountOperationForm^ form = gcnew AccountOperationForm(AccountOpType::Open, L"Дебетовый");
            if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                String^ type = form->SelectedAccountType;

                // ЗАЩИТА: Очищаем пробелы и переводим в нижний регистр для безопасного сравнения
                if (type != nullptr) {
                    type = type->Trim()->ToLower();
                }
                else {
                    type = "";
                }

                Random^ rand = gcnew Random();
                String^ newNum = "BY" + rand->Next(10, 99) + " 3242 " + rand->Next(1000, 9999);

                BankAccount^ newAcc = nullptr;

                if (type->Contains(L"дебетовый") || type->Contains("debit")) {
                    newAcc = gcnew DebitAccount(newNum, 0.0);
                }
                else if (type->Contains(L"сберегательный") || type->Contains("savings")) {
                    newAcc = gcnew SavingsAccount(newNum, 0.0);
                }
                else if (type->Contains(L"кредитный") || type->Contains("credit")) {
                    newAcc = gcnew CreditAccount(newNum, 0.0);
                }
                else {
                    newAcc = gcnew DebitAccount(newNum, 0.0);
                }

                if (newAcc != nullptr) {
                    currentClient->Accounts->Add(newAcc);

                    DatabaseManager::SaveAllData(allClients); // Сохраняем в БД!
                    ShowClientData(currentClient);
                    LoadAllClientsGrid();
                    MessageBox::Show(L"Счет успешно открыт!", L"Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
                }
            }
        }

        void ShowClientData(Client^ cl) {
            if (cl == nullptr) {
                lblClientFIO->Text = "ФИО: -";
                lblClientID->Text = "ID: -";
                lblClientPhone->Text = "Телефон: -";
                lblTotalBalance->Text = "Общий баланс: 0.00 BYN";
                gridAccounts->Rows->Clear();
                listHistory->Items->Clear();
                return;
            }
            currentClient = cl;

            lblClientFIO->Text = "ФИО: " + cl->FullName;
            lblClientID->Text = "ID: " + cl->Id;
            lblClientPhone->Text = "Телефон: " + cl->Phone;

            double total = 0;
            gridAccounts->Rows->Clear();
            List<BankTransaction^>^ filteredTransactions = gcnew List<BankTransaction^>();

            for each(BankAccount ^ acc in cl->Accounts) {
                total += acc->Balance;
                gridAccounts->Rows->Add(acc->GetAccountType(), acc->Number, acc->Balance.ToString("N2") + " BYN");

                for each(BankTransaction ^ tr in acc->History) {
                    String^ selectedFilter = cbFilterType->SelectedItem->ToString();
                    if (selectedFilter == "Все операции" || tr->Type == selectedFilter) {
                        filteredTransactions->Add(tr);
                    }
                }
            }

            int sortIndex = cbSortCriterion->SelectedIndex;
            if (sortIndex == 0) filteredTransactions->Sort(gcnew Comparison<BankTransaction^>(&Form1::CompareByDateDesc));
            else if (sortIndex == 1) filteredTransactions->Sort(gcnew Comparison<BankTransaction^>(&Form1::CompareByDateAsc));
            else if (sortIndex == 2) filteredTransactions->Sort(gcnew Comparison<BankTransaction^>(&Form1::CompareByAmountDesc));
            else if (sortIndex == 3) filteredTransactions->Sort(gcnew Comparison<BankTransaction^>(&Form1::CompareByAmountAsc));

            listHistory->Items->Clear();
            for each(BankTransaction ^ tr in filteredTransactions) {
                listHistory->Items->Add("Дата: " + tr->Date + " | Тип: " + tr->Type + " | Сумма: " + tr->Amount.ToString("N2") + " BYN");
            }

            lblTotalBalance->Text = "Общий баланс: " + total.ToString("N2") + " BYN";
        }

        void OnHistoryToolsChanged(System::Object^ sender, System::EventArgs^ e) {
            ShowClientData(currentClient);
        }

        void LoadAllClientsGrid() {
            LoadAllClientsGrid(nullptr);
        }

        void LoadAllClientsGrid(String^ filterText) {
            gridAllClients->Rows->Clear();
            for each(Client ^ cl in allClients) {
                if (String::IsNullOrEmpty(filterText) ||
                    cl->FullName->IndexOf(filterText, StringComparison::OrdinalIgnoreCase) >= 0 ||
                    cl->Phone->IndexOf(filterText, StringComparison::OrdinalIgnoreCase) >= 0 ||
                    cl->Id->IndexOf(filterText, StringComparison::OrdinalIgnoreCase) >= 0)
                {
                    gridAllClients->Rows->Add(cl->Id, cl->FullName, cl->Phone, cl->Accounts->Count);
                }
            }
        }

        void OnAddClientClick(System::Object^ sender, System::EventArgs^ e) {
            AddClientForm^ addForm = gcnew AddClientForm();
            if (addForm->btnCancel != nullptr) {
                addForm->btnCancel->FlatStyle = FlatStyle::Flat;
                addForm->btnCancel->FlatAppearance->BorderSize = 0;
                addForm->btnCancel->BackColor = Color::Transparent;
                addForm->btnCancel->ForeColor = Color::FromArgb(11, 0, 163);
                RoundCorners(addForm->btnCancel, 15);
                addForm->btnCancel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::OnButtonPaint);
            }

            if (addForm->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                if (String::IsNullOrWhiteSpace(addForm->txtFIO->Text) || String::IsNullOrWhiteSpace(addForm->txtPhone->Text)) {
                    MessageBox::Show(L"Пожалуйста, заполните все поля ввода!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                    return;
                }

                Random^ rand = gcnew Random();
                String^ generatedId = "TD-2026-" + rand->Next(1000, 9999).ToString();

                Client^ newClient = gcnew Client(generatedId, addForm->txtFIO->Text, addForm->txtPhone->Text);
                newClient->Accounts->Add(gcnew DebitAccount("BY" + rand->Next(10, 99) + " 3242 " + rand->Next(1000, 9999), 0.0));

                allClients->Add(newClient);
                DatabaseManager::SaveAllData(allClients);

                LoadAllClientsGrid();
                MessageBox::Show(L"Новый клиент успешно зарегистрирован!", L"Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
        }

        void OnDeleteClientClick(System::Object^ sender, System::EventArgs^ e) {
            if (gridAllClients->CurrentRow != nullptr && gridAllClients->CurrentRow->Index >= 0) {
                String^ selectedId = gridAllClients->CurrentRow->Cells[0]->Value->ToString();
                String^ selectedFIO = gridAllClients->CurrentRow->Cells[1]->Value->ToString();

                System::Windows::Forms::DialogResult result = MessageBox::Show(
                    L"Вы уверены, что хотите полностью удалить клиента?\n\nФИО: " + selectedFIO + L"\nID: " + selectedId,
                    L"Подтверждение удаления",
                    MessageBoxButtons::YesNo,
                    MessageBoxIcon::Warning
                );

                if (result == System::Windows::Forms::DialogResult::Yes) {
                    for (int i = 0; i < allClients->Count; i++) {
                        if (allClients[i]->Id == selectedId) {
                            allClients->RemoveAt(i);
                            break;
                        }
                    }

                    DatabaseManager::SaveAllData(allClients);
                    LoadAllClientsGrid();

                    if (currentClient != nullptr && currentClient->Id == selectedId) {
                        currentClient = allClients->Count > 0 ? allClients[0] : nullptr;
                        ShowClientData(currentClient);
                    }

                    MessageBox::Show(L"Клиент успешно удален из базы данных.", L"Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
                }
            }
            else {
                MessageBox::Show(L"Пожалуйста, выберите клиента в таблице!", L"Внимание", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            }
        }

        void GridAllClients_CellDoubleClick(System::Object^ sender, System::Windows::Forms::DataGridViewCellEventArgs^ e) {
            if (e->RowIndex >= 0) {
                String^ selectedId = gridAllClients->Rows[e->RowIndex]->Cells[0]->Value->ToString();
                for each(Client ^ cl in allClients) {
                    if (cl->Id == selectedId) {
                        ShowClientData(cl);
                        OnDashboardClick(nullptr, nullptr);
                        break;
                    }
                }
            }
        }

        void ResetMenuButtons() {
            btnDashboard->BackColor = Color::Transparent;
            btnDashboard->ForeColor = Color::FromArgb(11, 0, 163);
            btnClients->BackColor = Color::Transparent;
            btnClients->ForeColor = Color::FromArgb(11, 0, 163);
            btnStats->BackColor = Color::Transparent;
            btnStats->ForeColor = Color::FromArgb(11, 0, 163);
            btnAbout->BackColor = Color::Transparent;
            btnAbout->ForeColor = Color::FromArgb(11, 0, 163);

            btnDashboard->Invalidate();
            btnClients->Invalidate();
            btnStats->Invalidate();
            btnAbout->Invalidate();
        }

        void OnDashboardClick(System::Object^ sender, System::EventArgs^ e) {
            panelDashboardView->Visible = true;
            panelClientsView->Visible = false;
            panelStatsView->Visible = false;
            panelAboutView->Visible = false;
            panelDashboardView->BringToFront();

            ResetMenuButtons();
            btnDashboard->BackColor = Color::FromArgb(11, 0, 163);
            btnDashboard->ForeColor = Color::White;
            btnDashboard->Invalidate();
        }

        void OnClientsClick(System::Object^ sender, System::EventArgs^ e) {
            panelDashboardView->Visible = false;
            panelClientsView->Visible = true;
            panelStatsView->Visible = false;
            panelAboutView->Visible = false;
            panelClientsView->BringToFront();

            ResetMenuButtons();
            btnClients->BackColor = Color::FromArgb(11, 0, 163);
            btnClients->ForeColor = Color::White;
            btnClients->Invalidate();
        }

        void OnStatsClick(System::Object^ sender, System::EventArgs^ e) {
            panelDashboardView->Visible = false;
            panelClientsView->Visible = false;
            panelStatsView->Visible = true;
            panelAboutView->Visible = false;
            panelStatsView->BringToFront();

            ResetMenuButtons();
            btnStats->BackColor = Color::FromArgb(11, 0, 163);
            btnStats->ForeColor = Color::White;
            btnStats->Invalidate();

            StatsManager::RefreshStats(allClients);
        }

        void OnAboutClick(System::Object^ sender, System::EventArgs^ e) {
            panelDashboardView->Visible = false;
            panelClientsView->Visible = false;
            panelStatsView->Visible = false;
            panelAboutView->Visible = true;
            panelAboutView->BringToFront();

            ResetMenuButtons();
            btnAbout->BackColor = Color::FromArgb(11, 0, 163);
            btnAbout->ForeColor = Color::White;
            btnAbout->Invalidate();
        }

        void OnButtonPaint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
            Button^ btn = dynamic_cast<Button^>(sender);
            if (btn == nullptr) return;

            if (btn->BackColor == Color::Transparent) {
                e->Graphics->SmoothingMode = System::Drawing::Drawing2D::SmoothingMode::AntiAlias;
                float radius = 15.0f;
                System::Drawing::Drawing2D::GraphicsPath^ path = gcnew System::Drawing::Drawing2D::GraphicsPath();
                float offset = 0.5f;
                float width = btn->Width - 1.0f;
                float height = btn->Height - 1.0f;

                path->StartFigure();
                path->AddArc(System::Drawing::RectangleF(offset, offset, radius, radius), 180, 90);
                path->AddArc(System::Drawing::RectangleF(width - radius, offset, radius, radius), 270, 90);
                path->AddArc(System::Drawing::RectangleF(width - radius, height - radius, radius, radius), 0, 90);
                path->AddArc(System::Drawing::RectangleF(offset, height - radius, radius, radius), 90, 90);
                path->CloseFigure();

                Pen^ borderPen = gcnew Pen(Color::FromArgb(11, 0, 163), 1.2f);
                e->Graphics->DrawPath(borderPen, path);
                delete borderPen;
                delete path;
            }
        }

        void Form1_Load(System::Object^ sender, System::EventArgs^ e) {
            RoundCorners(this->btnDashboard, 15);
            RoundCorners(this->btnClients, 15);
            RoundCorners(this->btnStats, 15);
            RoundCorners(this->btnAbout, 15);
            RoundCorners(this->btnDeposit, 15);
            RoundCorners(this->btnWithdraw, 15);
            RoundCorners(this->btnInterest, 15);
            RoundCorners(this->btnNewAccount, 15);
            RoundCorners(this->btnAddClient, 15);
            RoundCorners(this->btnDeleteClient, 15);
            RoundCorners(this->cardClientInfo, 18);
            RoundCorners(this->lblTotalBalance, 12);
            RoundCorners(this->btnSearchDash, 10);
            RoundCorners(this->btnSearchClients, 10);
            RoundCorners(this->panelHistoryTools, 10);
            RoundCorners(this->cardAboutInfo, 18);
            RoundCorners(this->cardAboutHelp, 18);
        }

        void RoundCorners(Control^ control, int radius) {
            System::Drawing::Drawing2D::GraphicsPath^ requestPath = gcnew System::Drawing::Drawing2D::GraphicsPath();
            requestPath->StartFigure();
            requestPath->AddArc(0, 0, radius, radius, 180, 90);
            requestPath->AddArc(control->Width - radius, 0, radius, radius, 270, 90);
            requestPath->AddArc(control->Width - radius, control->Height - radius, radius, radius, 0, 90);
            requestPath->AddArc(0, control->Height - radius, radius, radius, 90, 90);
            requestPath->CloseFigure();
            control->Region = gcnew System::Drawing::Region(requestPath);
        }

    protected:
        ~Form1() {
            if (components) delete components;
        }
    private:
        System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
        void InitializeComponent(void)
        {
            Color brandBlue = Color::FromArgb(11, 0, 163);
            Color bgContentGray = Color::FromArgb(226, 232, 240);
            Color brightRed = Color::Crimson;

            this->panelSidebar = (gcnew Panel());
            this->panelContentArea = (gcnew Panel());
            this->lblMenuTitle = (gcnew Label());
            this->btnDashboard = (gcnew Button());
            this->btnClients = (gcnew Button());
            this->btnStats = (gcnew Button());
            this->btnAbout = (gcnew Button());

            this->panelDashboardView = (gcnew Panel());
            this->lblLogoDash = (gcnew Label());
            this->txtSearchDash = (gcnew TextBox());
            this->btnSearchDash = (gcnew Button());
            this->cardClientInfo = (gcnew Panel());
            this->lblInfoTitle = (gcnew Label());
            this->lblClientFIO = (gcnew Label());
            this->lblClientID = (gcnew Label());
            this->lblClientPhone = (gcnew Label());
            this->lblTotalBalance = (gcnew Label());
            this->gridAccounts = (gcnew DataGridView());
            this->panelHistoryTools = (gcnew Panel());
            this->cbSortCriterion = (gcnew ComboBox());
            this->cbFilterType = (gcnew ComboBox());
            this->listHistory = (gcnew ListBox());
            this->btnDeposit = (gcnew Button());
            this->btnWithdraw = (gcnew Button());
            this->btnInterest = (gcnew Button());
            this->btnNewAccount = (gcnew Button());

            this->panelClientsView = (gcnew Panel());
            this->lblLogoClients = (gcnew Label());
            this->txtSearchClients = (gcnew TextBox());
            this->btnSearchClients = (gcnew Button());
            this->gridAllClients = (gcnew DataGridView());
            this->btnAddClient = (gcnew Button());
            this->btnDeleteClient = (gcnew Button());

            this->panelAboutView = (gcnew Panel());
            this->cardAboutInfo = (gcnew Panel());
            this->cardAboutHelp = (gcnew Panel());

            this->SuspendLayout();

            this->Size = System::Drawing::Size(1150, 700);
            this->Text = L"ООО \"БКС\"";
            this->StartPosition = FormStartPosition::CenterScreen;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
            this->MaximizeBox = false;

            this->panelSidebar->Size = System::Drawing::Size(180, 700);
            this->panelSidebar->Dock = DockStyle::Left;
            this->panelSidebar->BackColor = bgContentGray;

            this->lblMenuTitle->Text = L"МЕНЮ";
            this->lblMenuTitle->Font = gcnew System::Drawing::Font("Arial", 14, FontStyle::Bold);
            this->lblMenuTitle->Location = System::Drawing::Point(20, 25);
            this->lblMenuTitle->Size = System::Drawing::Size(140, 30);
            this->lblMenuTitle->TextAlign = ContentAlignment::MiddleCenter;

            this->btnDashboard->Text = L"Дашборд";
            this->btnDashboard->Font = gcnew System::Drawing::Font("Arial", 11, FontStyle::Bold);
            this->btnDashboard->ForeColor = Color::White;
            this->btnDashboard->BackColor = brandBlue;
            this->btnDashboard->FlatStyle = FlatStyle::Flat;
            this->btnDashboard->FlatAppearance->BorderSize = 0;
            this->btnDashboard->Location = System::Drawing::Point(15, 75);
            this->btnDashboard->Size = System::Drawing::Size(150, 40);

            this->btnClients->Text = L"Клиенты";
            this->btnClients->Font = gcnew System::Drawing::Font("Arial", 11, FontStyle::Bold);
            this->btnClients->ForeColor = brandBlue;
            this->btnClients->BackColor = Color::Transparent;
            this->btnClients->FlatStyle = FlatStyle::Flat;
            this->btnClients->FlatAppearance->BorderSize = 0;
            this->btnClients->Location = System::Drawing::Point(15, 125);
            this->btnClients->Size = System::Drawing::Size(150, 40);

            this->btnStats->Text = L"Статистика";
            this->btnStats->Font = gcnew System::Drawing::Font("Arial", 11, FontStyle::Bold);
            this->btnStats->ForeColor = brandBlue;
            this->btnStats->BackColor = Color::Transparent;
            this->btnStats->FlatStyle = FlatStyle::Flat;
            this->btnStats->FlatAppearance->BorderSize = 0;
            this->btnStats->Location = System::Drawing::Point(15, 175);
            this->btnStats->Size = System::Drawing::Size(150, 40);

            this->btnAbout->Text = L"О программе";
            this->btnAbout->Font = gcnew System::Drawing::Font("Arial", 11, FontStyle::Bold);
            this->btnAbout->ForeColor = brandBlue;
            this->btnAbout->BackColor = Color::Transparent;
            this->btnAbout->FlatStyle = FlatStyle::Flat;
            this->btnAbout->FlatAppearance->BorderSize = 0;
            this->btnAbout->Location = System::Drawing::Point(15, 225);
            this->btnAbout->Size = System::Drawing::Size(150, 40);

            this->panelSidebar->Controls->Add(this->lblMenuTitle);
            this->panelSidebar->Controls->Add(this->btnDashboard);
            this->panelSidebar->Controls->Add(this->btnClients);
            this->panelSidebar->Controls->Add(this->btnStats);
            this->panelSidebar->Controls->Add(this->btnAbout);

            this->panelContentArea->Dock = DockStyle::Fill;
            this->panelContentArea->BackColor = bgContentGray;

            this->panelDashboardView->Size = System::Drawing::Size(970, 700);
            this->panelDashboardView->Dock = DockStyle::Fill;
            this->panelDashboardView->BackColor = bgContentGray;

            this->lblLogoDash->Text = L"ООО \"БКС\"";
            this->lblLogoDash->Font = gcnew System::Drawing::Font("Arial", 16, FontStyle::Bold);
            this->lblLogoDash->Location = System::Drawing::Point(25, 20);
            this->lblLogoDash->Size = System::Drawing::Size(300, 35);

            this->txtSearchDash->Location = System::Drawing::Point(400, 23);
            this->txtSearchDash->Size = System::Drawing::Size(280, 26);
            this->txtSearchDash->Font = gcnew System::Drawing::Font("Arial", 11);

            this->btnSearchDash->Text = L"ПОИСК";
            this->btnSearchDash->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
            this->btnSearchDash->ForeColor = Color::White;
            this->btnSearchDash->BackColor = brandBlue;
            this->btnSearchDash->FlatStyle = FlatStyle::Flat;
            this->btnSearchDash->FlatAppearance->BorderSize = 0;
            this->btnSearchDash->Location = System::Drawing::Point(690, 20);
            this->btnSearchDash->Size = System::Drawing::Size(100, 32);

            this->cardClientInfo->Location = System::Drawing::Point(25, 75);
            this->cardClientInfo->Size = System::Drawing::Size(450, 200);
            this->cardClientInfo->BackColor = Color::White;

            this->lblInfoTitle->Text = L"   Информация о клиенте";
            this->lblInfoTitle->BackColor = brandBlue;
            this->lblInfoTitle->ForeColor = Color::White;
            this->lblInfoTitle->Font = gcnew System::Drawing::Font("Arial", 12, FontStyle::Bold);
            this->lblInfoTitle->Dock = DockStyle::Top;
            this->lblInfoTitle->Size = System::Drawing::Size(450, 40);
            this->lblInfoTitle->TextAlign = ContentAlignment::MiddleLeft;

            this->lblClientFIO->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
            this->lblClientFIO->Location = System::Drawing::Point(15, 55);
            this->lblClientFIO->Size = System::Drawing::Size(420, 25);

            this->lblClientID->Font = gcnew System::Drawing::Font("Arial", 10);
            this->lblClientID->Location = System::Drawing::Point(15, 85);
            this->lblClientID->Size = System::Drawing::Size(420, 25);

            this->lblClientPhone->Font = gcnew System::Drawing::Font("Arial", 10);
            this->lblClientPhone->Location = System::Drawing::Point(15, 115);
            this->lblClientPhone->Size = System::Drawing::Size(420, 25);

            this->lblTotalBalance->Font = gcnew System::Drawing::Font("Arial", 13, FontStyle::Bold);
            this->lblTotalBalance->ForeColor = Color::White;
            this->lblTotalBalance->BackColor = Color::FromArgb(0, 102, 255);
            this->lblTotalBalance->Location = System::Drawing::Point(15, 145);
            this->lblTotalBalance->Size = System::Drawing::Size(420, 42);
            this->lblTotalBalance->TextAlign = ContentAlignment::MiddleCenter;

            this->cardClientInfo->Controls->Add(this->lblInfoTitle);
            this->cardClientInfo->Controls->Add(this->lblClientFIO);
            this->cardClientInfo->Controls->Add(this->lblClientID);
            this->cardClientInfo->Controls->Add(this->lblClientPhone);
            this->cardClientInfo->Controls->Add(this->lblTotalBalance);

            this->gridAccounts->ColumnCount = 3;
            this->gridAccounts->Columns[0]->Name = L"Тип";
            this->gridAccounts->Columns[0]->Width = 110;
            this->gridAccounts->Columns[1]->Name = L"Номер";
            this->gridAccounts->Columns[1]->Width = 180;
            this->gridAccounts->Columns[2]->Name = L"Баланс";
            this->gridAccounts->Columns[2]->Width = 135;
            this->gridAccounts->Location = System::Drawing::Point(25, 295);
            this->gridAccounts->Size = System::Drawing::Size(450, 240);
            this->gridAccounts->BackgroundColor = Color::White;
            this->gridAccounts->RowHeadersVisible = false;
            this->gridAccounts->AllowUserToAddRows = false;
            this->gridAccounts->ReadOnly = true;

            this->panelHistoryTools->Location = System::Drawing::Point(500, 75);
            this->panelHistoryTools->Size = System::Drawing::Size(420, 45);
            this->panelHistoryTools->BackColor = Color::White;

            this->cbSortCriterion->DropDownStyle = ComboBoxStyle::DropDownList;
            this->cbSortCriterion->Font = gcnew System::Drawing::Font("Arial", 9);
            this->cbSortCriterion->Items->AddRange(gcnew array<Object^>{
                L"Дата: От новых к старым", L"Дата: От старых к новым",
                    L"Сумма: На возрастание", L"Сумма: На убывание"
            });
            this->cbSortCriterion->SelectedIndex = 0;
            this->cbSortCriterion->Location = System::Drawing::Point(10, 10);
            this->cbSortCriterion->Size = System::Drawing::Size(195, 25);

            this->cbFilterType->DropDownStyle = ComboBoxStyle::DropDownList;
            this->cbFilterType->Font = gcnew System::Drawing::Font("Arial", 9);
            this->cbFilterType->Items->AddRange(gcnew array<Object^>{
                L"Все операции", L"Проценты", L"Снятие", L"Пополнение"
            });
            this->cbFilterType->SelectedIndex = 0;
            this->cbFilterType->Location = System::Drawing::Point(215, 10);
            this->cbFilterType->Size = System::Drawing::Size(195, 25);
            this->panelHistoryTools->Controls->Add(this->cbSortCriterion);
            this->panelHistoryTools->Controls->Add(this->cbFilterType);

            this->listHistory->Location = System::Drawing::Point(500, 125);
            this->listHistory->Size = System::Drawing::Size(420, 410);
            this->listHistory->BackColor = Color::White;
            this->listHistory->Font = gcnew System::Drawing::Font("Arial", 9);

            this->btnDeposit->Text = L"Пополнить счет";
            this->btnDeposit->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
            this->btnDeposit->ForeColor = Color::White;
            this->btnDeposit->BackColor = brandBlue;
            this->btnDeposit->FlatStyle = FlatStyle::Flat;
            this->btnDeposit->FlatAppearance->BorderSize = 0;
            this->btnDeposit->Location = System::Drawing::Point(25, 560);
            this->btnDeposit->Size = System::Drawing::Size(170, 45);

            this->btnWithdraw->Text = L"Снять со счета";
            this->btnWithdraw->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
            this->btnWithdraw->ForeColor = Color::White;
            this->btnWithdraw->BackColor = brandBlue;
            this->btnWithdraw->FlatStyle = FlatStyle::Flat;
            this->btnWithdraw->FlatAppearance->BorderSize = 0;
            this->btnWithdraw->Location = System::Drawing::Point(210, 560);
            this->btnWithdraw->Size = System::Drawing::Size(170, 45);

            this->btnInterest->Text = L"Начислить проценты";
            this->btnInterest->Font = gcnew System::Drawing::Font("Arial", 9, FontStyle::Bold);
            this->btnInterest->ForeColor = Color::White;
            this->btnInterest->BackColor = brandBlue;
            this->btnInterest->FlatStyle = FlatStyle::Flat;
            this->btnInterest->FlatAppearance->BorderSize = 0;
            this->btnInterest->Location = System::Drawing::Point(395, 560);
            this->btnInterest->Size = System::Drawing::Size(170, 45);

            this->btnNewAccount->Text = L"Открыть новый счет";
            this->btnNewAccount->Font = gcnew System::Drawing::Font("Arial", 9, FontStyle::Bold);
            this->btnNewAccount->ForeColor = Color::White;
            this->btnNewAccount->BackColor = brandBlue;
            this->btnNewAccount->FlatStyle = FlatStyle::Flat;
            this->btnNewAccount->FlatAppearance->BorderSize = 0;
            this->btnNewAccount->Location = System::Drawing::Point(580, 560);
            this->btnNewAccount->Size = System::Drawing::Size(180, 45);

            this->panelDashboardView->Controls->Add(this->lblLogoDash);
            this->panelDashboardView->Controls->Add(this->txtSearchDash);
            this->panelDashboardView->Controls->Add(this->btnSearchDash);
            this->panelDashboardView->Controls->Add(this->cardClientInfo);
            this->panelDashboardView->Controls->Add(this->gridAccounts);
            this->panelDashboardView->Controls->Add(this->panelHistoryTools);
            this->panelDashboardView->Controls->Add(this->listHistory);
            this->panelDashboardView->Controls->Add(this->btnDeposit);
            this->panelDashboardView->Controls->Add(this->btnWithdraw);
            this->panelDashboardView->Controls->Add(this->btnInterest);
            this->panelDashboardView->Controls->Add(this->btnNewAccount);

            this->panelClientsView->Size = System::Drawing::Size(970, 700);
            this->panelClientsView->Dock = DockStyle::Fill;
            this->panelClientsView->BackColor = bgContentGray;

            this->lblLogoClients->Text = L"ООО \"БКС\"";
            this->lblLogoClients->Font = gcnew System::Drawing::Font("Arial", 16, FontStyle::Bold);
            this->lblLogoClients->Location = System::Drawing::Point(25, 20);
            this->lblLogoClients->Size = System::Drawing::Size(300, 35);

            this->txtSearchClients->Location = System::Drawing::Point(400, 23);
            this->txtSearchClients->Size = System::Drawing::Size(280, 26);
            this->txtSearchClients->Font = gcnew System::Drawing::Font("Arial", 11);

            this->btnSearchClients->Text = L"ПОИСК";
            this->btnSearchClients->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
            this->btnSearchClients->ForeColor = Color::White;
            this->btnSearchClients->BackColor = brandBlue;
            this->btnSearchClients->FlatStyle = FlatStyle::Flat;
            this->btnSearchClients->FlatAppearance->BorderSize = 0;
            this->btnSearchClients->Location = System::Drawing::Point(690, 20);
            this->btnSearchClients->Size = System::Drawing::Size(100, 32);

            this->gridAllClients->ColumnCount = 4;
            this->gridAllClients->Columns[0]->Name = L"ID";
            this->gridAllClients->Columns[0]->Width = 140;
            this->gridAllClients->Columns[1]->Name = L"ФИО";
            this->gridAllClients->Columns[1]->Width = 300;
            this->gridAllClients->Columns[2]->Name = L"Телефон";
            this->gridAllClients->Columns[2]->Width = 180;
            this->gridAllClients->Columns[3]->Name = L"Кол-во счетов";
            this->gridAllClients->Columns[3]->Width = 130;
            this->gridAllClients->Location = System::Drawing::Point(25, 75);
            this->gridAllClients->Size = System::Drawing::Size(895, 460);
            this->gridAllClients->BackgroundColor = Color::White;
            this->gridAllClients->RowHeadersVisible = false;
            this->gridAllClients->AllowUserToAddRows = false;
            this->gridAllClients->ReadOnly = true;

            this->btnAddClient->Text = L"Регистрация клиента";
            this->btnAddClient->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
            this->btnAddClient->ForeColor = Color::White;
            this->btnAddClient->BackColor = brandBlue;
            this->btnAddClient->FlatStyle = FlatStyle::Flat;
            this->btnAddClient->FlatAppearance->BorderSize = 0;
            this->btnAddClient->Location = System::Drawing::Point(265, 560);
            this->btnAddClient->Size = System::Drawing::Size(210, 45);

            this->btnDeleteClient->Text = L"Удалить клиента";
            this->btnDeleteClient->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
            this->btnDeleteClient->ForeColor = Color::White;
            this->btnDeleteClient->BackColor = brightRed;
            this->btnDeleteClient->FlatStyle = FlatStyle::Flat;
            this->btnDeleteClient->FlatAppearance->BorderSize = 0;
            this->btnDeleteClient->Location = System::Drawing::Point(495, 560);
            this->btnDeleteClient->Size = System::Drawing::Size(180, 45);

            this->panelClientsView->Controls->Add(this->lblLogoClients);
            this->panelClientsView->Controls->Add(this->txtSearchClients);
            this->panelClientsView->Controls->Add(this->btnSearchClients);
            this->panelClientsView->Controls->Add(this->gridAllClients);
            this->panelClientsView->Controls->Add(this->btnAddClient);
            this->panelClientsView->Controls->Add(this->btnDeleteClient);

            this->panelAboutView->Size = System::Drawing::Size(970, 700);
            this->panelAboutView->Dock = DockStyle::Fill;
            this->panelAboutView->BackColor = bgContentGray;

            Label^ lblAboutTitle = gcnew Label();
            lblAboutTitle->Text = L"О программе и Справка";
            lblAboutTitle->Font = gcnew System::Drawing::Font("Arial", 16, FontStyle::Bold);
            lblAboutTitle->Location = System::Drawing::Point(25, 20);
            lblAboutTitle->Size = System::Drawing::Size(400, 35);
            this->panelAboutView->Controls->Add(lblAboutTitle);

            this->cardAboutInfo->Location = System::Drawing::Point(25, 75);
            this->cardAboutInfo->Size = System::Drawing::Size(895, 140);
            this->cardAboutInfo->BackColor = Color::White;

            Label^ lblSystemName = gcnew Label();
            lblSystemName->Text = L"Автоматизированная Банковская Система «ООО Банк Кредитных Систем»";
            lblSystemName->Font = gcnew System::Drawing::Font("Arial", 12, FontStyle::Bold);
            lblSystemName->ForeColor = brandBlue;
            lblSystemName->Location = System::Drawing::Point(20, 15);
            lblSystemName->Size = System::Drawing::Size(800, 25);
            lblSystemName->BackColor = Color::Transparent;
            this->cardAboutInfo->Controls->Add(lblSystemName);

            Label^ lblAuthorInfo = gcnew Label();
            lblAuthorInfo->Text = L"Разработчик: Студент Мазнев Артем Александрович\n" +
                L"Версия ПО: 3.0.0 \n" +
                L"Стек технологий: C++/CLI, Windows Forms, SQLite 3, GDI+ Graphics Engine";
            lblAuthorInfo->Font = gcnew System::Drawing::Font("Arial", 10);
            lblAuthorInfo->Location = System::Drawing::Point(20, 50);
            lblAuthorInfo->Size = System::Drawing::Size(800, 70);
            lblAuthorInfo->BackColor = Color::Transparent;
            this->cardAboutInfo->Controls->Add(lblAuthorInfo);
            this->panelAboutView->Controls->Add(this->cardAboutInfo);

            this->cardAboutHelp->Location = System::Drawing::Point(25, 235);
            this->cardAboutHelp->Size = System::Drawing::Size(895, 310);
            this->cardAboutHelp->BackColor = Color::White;

            Label^ lblHelpTitle = gcnew Label();
            lblHelpTitle->Text = L"Инструкция по использованию поисковой подсистемы";
            lblHelpTitle->Font = gcnew System::Drawing::Font("Arial", 12, FontStyle::Bold);
            lblHelpTitle->Location = System::Drawing::Point(20, 15);
            lblHelpTitle->Size = System::Drawing::Size(800, 25);
            lblHelpTitle->BackColor = Color::Transparent;
            this->cardAboutHelp->Controls->Add(lblHelpTitle);

            Label^ lblHelpText = gcnew Label();
            lblHelpText->Text = L"• На Дашборде:\n" +
                L"  Поиск работает строго по ID клиента (Регистронезависимый).\n" +
                L"  Если клиент найден — программа тут же подгружает его данные, счета и историю операций.\n" +
                L"  Если нет — выводит аккуратное информационное уведомление.\n\n" +
                L"• В разделе Клиенты:\n" +
                L"  Поиск сделан «умным» и гибким. При нажатии кнопки «ПОИСК» таблица автоматически\n" +
                L"  фильтруется по любому частичному совпадению: можно свободно искать по ФИО,\n" +
                L"  по номеру мобильного телефона или по уникальному ID.\n\n" +
                L"• Сброс фильтрации:\n" +
                L"  Чтобы сбросить результаты поиска и вернуть полный список всех клиентов,\n" +
                L"  достаточно очистить текстовое поле ввода и нажать кнопку «ПОИСК».";
            lblHelpText->Font = gcnew System::Drawing::Font("Arial", 10);
            lblHelpText->Location = System::Drawing::Point(20, 55);
            lblHelpText->AutoSize = true;
            lblHelpText->BackColor = Color::Transparent;
            this->cardAboutHelp->Controls->Add(lblHelpText);
            this->panelAboutView->Controls->Add(this->cardAboutHelp);

            this->panelStatsView = StatsManager::CreateStatsPanel(970, 700);

            this->panelContentArea->Controls->Add(this->panelDashboardView);
            this->panelContentArea->Controls->Add(this->panelClientsView);
            this->panelContentArea->Controls->Add(this->panelStatsView);
            this->panelContentArea->Controls->Add(this->panelAboutView);

            this->panelDashboardView->BringToFront();

            this->Controls->Add(this->panelContentArea);
            this->Controls->Add(this->panelSidebar);

            this->ResumeLayout(false);
            this->panelSidebar->ResumeLayout(false);
            this->panelDashboardView->ResumeLayout(false);
            this->panelDashboardView->PerformLayout();
            this->panelClientsView->ResumeLayout(false);
            this->panelClientsView->PerformLayout();
            this->panelContentArea->ResumeLayout(false);
        }
#pragma endregion
    };
}