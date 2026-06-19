#pragma once
// Обрати внимание: здесь СТРОГО НЕТ #include "Form1.h"

namespace CppCLRWinFormsProject {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    // Перечисление, из-за потери которого лезли 162 ошибки
    public enum class AccountOpType {
        Deposit,
        Withdraw,
        Interest,
        Open
    };

    public ref class AccountOperationForm : public System::Windows::Forms::Form
    {
    public:
        // Свойства, через которые Form1 забирает результаты
        property String^ SelectedAccountType;
        property String^ EnteredAmount;

    private:
        AccountOpType currentOpType;

        // UI Элементы
        Label^ lblTitle;
        Label^ lblAccountType;
        ComboBox^ cbAccountType;
        Label^ lblAmount;
        TextBox^ txtAmount;
        Button^ btnConfirm;
        Button^ btnCancel;

    public:
        AccountOperationForm(AccountOpType opType, String^ defaultAccType)
        {
            InitializeComponent();
            currentOpType = opType;

            // Динамическая подстройка интерфейса в зависимости от типа операции
            if (opType == AccountOpType::Deposit) {
                lblTitle->Text = L"Пополнение счета";
                lblAmount->Text = L"Сумма пополнения (BYN):";
                cbAccountType->Visible = false;
                lblAccountType->Visible = false;
            }
            else if (opType == AccountOpType::Withdraw) {
                lblTitle->Text = L"Снятие со счета";
                lblAmount->Text = L"Сумма снятия (BYN):";
                cbAccountType->Visible = false;
                lblAccountType->Visible = false;
            }
            else if (opType == AccountOpType::Interest) {
                lblTitle->Text = L"Начисление процентов";
                lblAmount->Text = L"Процентная ставка (%):";
                if (defaultAccType != nullptr) cbAccountType->SelectedItem = defaultAccType;
            }
            else if (opType == AccountOpType::Open) {
                lblTitle->Text = L"Открытие нового счета";
                txtAmount->Visible = false; // Сумму при открытии вводить не нужно
                lblAmount->Visible = false;
                if (defaultAccType != nullptr) cbAccountType->SelectedItem = defaultAccType;
            }
        }

    protected:
        ~AccountOperationForm()
        {
            if (components) {
                delete components;
            }
        }

    private:
        System::ComponentModel::Container^ components;

        void InitializeComponent(void)
        {
            Color brandBlue = Color::FromArgb(11, 0, 163);

            this->lblTitle = (gcnew System::Windows::Forms::Label());
            this->lblAccountType = (gcnew System::Windows::Forms::Label());
            this->cbAccountType = (gcnew System::Windows::Forms::ComboBox());
            this->lblAmount = (gcnew System::Windows::Forms::Label());
            this->txtAmount = (gcnew System::Windows::Forms::TextBox());
            this->btnConfirm = (gcnew System::Windows::Forms::Button());
            this->btnCancel = (gcnew System::Windows::Forms::Button());
            this->SuspendLayout();

            // lblTitle
            this->lblTitle->Font = (gcnew System::Drawing::Font(L"Arial", 14, System::Drawing::FontStyle::Bold));
            this->lblTitle->ForeColor = brandBlue;
            this->lblTitle->Location = System::Drawing::Point(20, 20);
            this->lblTitle->Size = System::Drawing::Size(340, 30);
            this->lblTitle->Text = L"Операция по счету";
            this->lblTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;

            // lblAccountType
            this->lblAccountType->AutoSize = true;
            this->lblAccountType->Font = (gcnew System::Drawing::Font(L"Arial", 10));
            this->lblAccountType->Location = System::Drawing::Point(30, 70);
            this->lblAccountType->Size = System::Drawing::Size(80, 16);
            this->lblAccountType->Text = L"Тип счета:";

            // cbAccountType
            this->cbAccountType->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->cbAccountType->Font = (gcnew System::Drawing::Font(L"Arial", 11));
            this->cbAccountType->FormattingEnabled = true;
            this->cbAccountType->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Дебетовый", L"Сберегательный", L"Кредитный" });
            this->cbAccountType->Location = System::Drawing::Point(30, 90);
            this->cbAccountType->Size = System::Drawing::Size(320, 25);
            this->cbAccountType->SelectedIndex = 0;

            // lblAmount
            this->lblAmount->AutoSize = true;
            this->lblAmount->Font = (gcnew System::Drawing::Font(L"Arial", 10));
            this->lblAmount->Location = System::Drawing::Point(30, 130);
            this->lblAmount->Size = System::Drawing::Size(54, 16);
            this->lblAmount->Text = L"Сумма:";

            // txtAmount
            this->txtAmount->Font = (gcnew System::Drawing::Font(L"Arial", 12));
            this->txtAmount->Location = System::Drawing::Point(30, 150);
            this->txtAmount->Size = System::Drawing::Size(320, 26);

            // btnConfirm
            this->btnConfirm->BackColor = brandBlue;
            this->btnConfirm->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnConfirm->FlatAppearance->BorderSize = 0;
            this->btnConfirm->Font = (gcnew System::Drawing::Font(L"Arial", 10, System::Drawing::FontStyle::Bold));
            this->btnConfirm->ForeColor = System::Drawing::Color::White;
            this->btnConfirm->Location = System::Drawing::Point(30, 210);
            this->btnConfirm->Size = System::Drawing::Size(150, 40);
            this->btnConfirm->Text = L"Подтвердить";
            this->btnConfirm->UseVisualStyleBackColor = false;
            this->btnConfirm->Click += gcnew System::EventHandler(this, &AccountOperationForm::btnConfirm_Click);

            // btnCancel
            this->btnCancel->BackColor = System::Drawing::Color::White;
            this->btnCancel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnCancel->Font = (gcnew System::Drawing::Font(L"Arial", 10, System::Drawing::FontStyle::Bold));
            this->btnCancel->ForeColor = brandBlue;
            this->btnCancel->Location = System::Drawing::Point(200, 210);
            this->btnCancel->Size = System::Drawing::Size(150, 40);
            this->btnCancel->Text = L"Отмена";
            this->btnCancel->UseVisualStyleBackColor = false;
            this->btnCancel->Click += gcnew System::EventHandler(this, &AccountOperationForm::btnCancel_Click);

            // Настройка самой формы (AccountOperationForm)
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackColor = System::Drawing::Color::FromArgb(245, 247, 250);
            this->ClientSize = System::Drawing::Size(380, 280);
            this->Controls->Add(this->btnCancel);
            this->Controls->Add(this->btnConfirm);
            this->Controls->Add(this->txtAmount);
            this->Controls->Add(this->lblAmount);
            this->Controls->Add(this->cbAccountType);
            this->Controls->Add(this->lblAccountType);
            this->Controls->Add(this->lblTitle);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox = false;
            this->MinimizeBox = false;
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
            this->Text = L"Действие со счетом";
            this->ResumeLayout(false);
            this->PerformLayout();
        }

        void btnConfirm_Click(System::Object^ sender, System::EventArgs^ e) {
            // Защита от дурака: если не открытие счета, проверяем чтобы сумма была корректной
            if (currentOpType != AccountOpType::Open) {
                if (String::IsNullOrWhiteSpace(txtAmount->Text)) {
                    MessageBox::Show(L"Пожалуйста, введите сумму!", L"Внимание", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                    return;
                }
                double val = 0;
                // Проверяем, что введено именно число и оно больше нуля (меняем запятую на точку, если нужно)
                String^ amountText = txtAmount->Text->Replace(".", ",");
                if (!Double::TryParse(amountText, val) || val <= 0) {
                    MessageBox::Show(L"Значение должно быть положительным числом!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
                    return;
                }
            }

            // Передаем данные в публичные свойства
            this->SelectedAccountType = cbAccountType->SelectedItem->ToString();
            this->EnteredAmount = txtAmount->Text->Replace(".", ",");

            // Возвращаем сигнал об успехе и закрываемся
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }

        void btnCancel_Click(System::Object^ sender, System::EventArgs^ e) {
            this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->Close();
        }
    };
}