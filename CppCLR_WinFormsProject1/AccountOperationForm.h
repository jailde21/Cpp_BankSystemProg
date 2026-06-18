#pragma once

namespace CppCLRWinFormsProject {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    // Перечисление для типов операций
    public enum class AccountOpType {
        Interest,   // Начисление процента
        Withdraw,   // Снять со счета
        Deposit,    // Пополнение счета
        Open        // Открытие счета
    };

    public ref class AccountOperationForm : public System::Windows::Forms::Form
    {
    private:
        AccountOpType currentOp;

    public:
        // Конструктор
        AccountOperationForm(AccountOpType op, String^ accountType)
        {
            InitializeComponent();
            currentOp = op;
            SetupUI(accountType);
        }

    protected:
        ~AccountOperationForm()
        {
            if (components) delete components;
        }

    private:
        System::Windows::Forms::Label^ lblTitle;
    public:
        System::Windows::Forms::ComboBox^ cmbAccountType;
        System::Windows::Forms::TextBox^ txtAmount;
        System::Windows::Forms::Button^ btnAction;
        System::Windows::Forms::Button^ btnCancel;
    private:
        System::ComponentModel::Container^ components;

        void InitializeComponent(void)
        {
            this->lblTitle = (gcnew System::Windows::Forms::Label());
            this->cmbAccountType = (gcnew System::Windows::Forms::ComboBox());
            this->txtAmount = (gcnew System::Windows::Forms::TextBox());
            this->btnAction = (gcnew System::Windows::Forms::Button());
            this->btnCancel = (gcnew System::Windows::Forms::Button());
            this->SuspendLayout();

            // Настройка главного окна
            this->BackColor = System::Drawing::Color::FromArgb(226, 232, 240); // Твой цвет bgContentGray
            this->ClientSize = System::Drawing::Size(340, 190);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox = false;
            this->MinimizeBox = false;
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;

            // Заголовок операции
            this->lblTitle->Font = (gcnew System::Drawing::Font(L"Arial", 12, System::Drawing::FontStyle::Bold));
            this->lblTitle->Location = System::Drawing::Point(12, 15);
            this->lblTitle->Size = System::Drawing::Size(316, 25);
            this->lblTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->lblTitle->ForeColor = System::Drawing::Color::Black;

            // Выбор типа счета
            this->cmbAccountType->Font = (gcnew System::Drawing::Font(L"Arial", 11));
            this->cmbAccountType->Location = System::Drawing::Point(20, 50);
            this->cmbAccountType->Size = System::Drawing::Size(300, 28);
            this->cmbAccountType->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->cmbAccountType->Items->AddRange(gcnew array<System::Object^>(3) { L"Дебетовый", L"Сберегательный", L"Кредитный" });
            this->cmbAccountType->SelectedIndex = 0;

            // Поле ввода суммы
            this->txtAmount->Font = (gcnew System::Drawing::Font(L"Arial", 11));
            this->txtAmount->Location = System::Drawing::Point(20, 90);
            this->txtAmount->Size = System::Drawing::Size(300, 28);

            // Кнопка Действия
            this->btnAction->BackColor = System::Drawing::Color::FromArgb(11, 0, 163); // Твой brandBlue
            this->btnAction->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnAction->FlatAppearance->BorderSize = 0;
            this->btnAction->Font = (gcnew System::Drawing::Font(L"Arial", 10, System::Drawing::FontStyle::Bold));
            this->btnAction->ForeColor = System::Drawing::Color::White;
            this->btnAction->Location = System::Drawing::Point(20, 135);
            this->btnAction->Size = System::Drawing::Size(140, 38);
            this->btnAction->UseVisualStyleBackColor = false;
            this->btnAction->Click += gcnew System::EventHandler(this, &AccountOperationForm::btnAction_Click);

            // Кнопка Отмена (Прозрачная с синей рамкой)
            this->btnCancel->Text = L"Отмена";
            this->btnCancel->BackColor = System::Drawing::Color::Transparent;
            this->btnCancel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnCancel->FlatAppearance->BorderSize = 0;
            this->btnCancel->Font = (gcnew System::Drawing::Font(L"Arial", 10, System::Drawing::FontStyle::Bold));
            this->btnCancel->ForeColor = System::Drawing::Color::FromArgb(11, 0, 163); // Синий текст
            this->btnCancel->Location = System::Drawing::Point(180, 135);
            this->btnCancel->Size = System::Drawing::Size(140, 38);
            this->btnCancel->UseVisualStyleBackColor = false;
            this->btnCancel->Click += gcnew System::EventHandler(this, &AccountOperationForm::btnCancel_Click);

            // Подписка на красивую отрисовку обводки и текста
            this->btnCancel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &AccountOperationForm::OnButtonPaint);

            // Добавление элементов на форму
            this->Controls->Add(this->lblTitle);
            this->Controls->Add(this->cmbAccountType);
            this->Controls->Add(this->txtAmount);
            this->Controls->Add(this->btnAction);
            this->Controls->Add(this->btnCancel);

            this->Load += gcnew System::EventHandler(this, &AccountOperationForm::AccountOperationForm_Load);
            this->ResumeLayout(false);
            this->PerformLayout();
        }

        void SetupUI(String^ accountType)
        {
            if (!String::IsNullOrEmpty(accountType)) {
                cmbAccountType->SelectedItem = accountType;
            }

            switch (currentOp)
            {
            case AccountOpType::Interest:
                this->Text = L"Начисление процента";
                lblTitle->Text = L"Начисление процента";
                btnAction->Text = L"Начислить";
                break;

            case AccountOpType::Withdraw:
                this->Text = L"Снять со счета";
                lblTitle->Text = L"Снять со счета";
                btnAction->Text = L"Снять";
                break;

            case AccountOpType::Deposit:
                this->Text = L"Пополнение счета";
                lblTitle->Text = L"Пополнение счета";
                btnAction->Text = L"Пополнить";
                // ВЕРНУЛИ ВЫБОР СЧЕТА НА ЭКРАН ДЛЯ ПОПОЛНЕНИЯ:
                cmbAccountType->Visible = true;
                txtAmount->Location = System::Drawing::Point(20, 90);
                break;

            case AccountOpType::Open:
                this->Text = L"Открытие счета";
                lblTitle->Text = L"Открытие счета";
                btnAction->Text = L"Открыть";
                txtAmount->Visible = false;
                break;
            }
        }

        void btnAction_Click(System::Object^ sender, System::EventArgs^ e)
        {
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }

        void btnCancel_Click(System::Object^ sender, System::EventArgs^ e)
        {
            this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->Close();
        }

        void AccountOperationForm_Load(System::Object^ sender, System::EventArgs^ e)
        {
            RoundCorners(this->btnAction, 15);
            RoundCorners(this->btnCancel, 15);
        }

        void RoundCorners(Control^ control, int radius)
        {
            System::Drawing::Drawing2D::GraphicsPath^ path = gcnew System::Drawing::Drawing2D::GraphicsPath();
            path->StartFigure();
            path->AddArc(0, 0, radius, radius, 180, 90);
            path->AddArc(control->Width - radius, 0, radius, radius, 270, 90);
            path->AddArc(control->Width - radius, control->Height - radius, radius, radius, 0, 90);
            path->AddArc(0, control->Height - radius, radius, radius, 90, 90);
            path->CloseFigure();
            control->Region = gcnew System::Drawing::Region(path);
        }

        // Исправленный метод отрисовки (текст теперь центрируется идеально ровно)
        void OnButtonPaint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e)
        {
            Button^ btn = dynamic_cast<Button^>(sender);
            if (btn == nullptr) return;

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

            // Рисуем красивую рамку
            Pen^ borderPen = gcnew Pen(System::Drawing::Color::FromArgb(11, 0, 163), 1.5f);
            e->Graphics->DrawPath(borderPen, path);

            // Идеальное центрирование текста без уплывания
            StringFormat^ sf = gcnew StringFormat();
            sf->Alignment = StringAlignment::Center;
            sf->LineAlignment = StringAlignment::Center;

            SolidBrush^ textBrush = gcnew SolidBrush(btn->ForeColor);
            e->Graphics->DrawString(btn->Text, btn->Font, textBrush, RectangleF(0, 0, btn->Width, btn->Height), sf);

            delete borderPen;
            delete textBrush;
            delete path;
            delete sf;
        }

    public:
        property String^ SelectedAccountType {
            String^ get() {
                return cmbAccountType->SelectedItem ? cmbAccountType->SelectedItem->ToString() : L"Дебетовый";
            }
        }
        property String^ EnteredAmount {
            String^ get() { return txtAmount->Text; }
        }
    };
}