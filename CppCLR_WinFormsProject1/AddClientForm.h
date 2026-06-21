#pragma once

namespace CppCLRWinFormsProject {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class AddClientForm : public System::Windows::Forms::Form
    {
    private:
        System::ComponentModel::Container^ components;
        Label^ lblHeader;
        Label^ lblFIO;
        Label^ lblPhone;

    public:
        TextBox^ txtFIO;
        TextBox^ txtPhone;
        Button^ btnSave;
        Button^ btnCancel;

        AddClientForm(void)
        {
            InitializeComponent();
            this->Load += gcnew System::EventHandler(this, &AddClientForm::AddClientForm_Load);
        }

    protected:
        ~AddClientForm()
        {
            if (components) delete components;
        }

    private:
        void InitializeComponent(void)
        {
            Color brandBlue = Color::FromArgb(11, 0, 163);
            Color bgGray = Color::FromArgb(226, 232, 240);

            this->lblHeader = (gcnew System::Windows::Forms::Label());
            this->lblFIO = (gcnew System::Windows::Forms::Label());
            this->txtFIO = (gcnew System::Windows::Forms::TextBox());
            this->lblPhone = (gcnew System::Windows::Forms::Label());
            this->txtPhone = (gcnew System::Windows::Forms::TextBox());
            this->btnSave = (gcnew System::Windows::Forms::Button());
            this->btnCancel = (gcnew System::Windows::Forms::Button());
            this->SuspendLayout();

            // lblHeader
            this->lblHeader->Text = L"Регистрация нового клиента";
            this->lblHeader->Font = gcnew System::Drawing::Font("Arial", 14, FontStyle::Bold);
            this->lblHeader->Location = System::Drawing::Point(20, 15);
            this->lblHeader->Size = System::Drawing::Size(350, 30);

            // lblFIO
            this->lblFIO->Text = L"ФИО Клиента:";
            this->lblFIO->Font = gcnew System::Drawing::Font("Arial", 10);
            this->lblFIO->Location = System::Drawing::Point(20, 65);
            this->lblFIO->Size = System::Drawing::Size(120, 20);

            // txtFIO
            this->txtFIO->Location = System::Drawing::Point(20, 90);
            this->txtFIO->Size = System::Drawing::Size(350, 25);
            this->txtFIO->Font = gcnew System::Drawing::Font("Arial", 10);

            // lblPhone
            this->lblPhone->Text = L"Номер телефона:";
            this->lblPhone->Font = gcnew System::Drawing::Font("Arial", 10);
            this->lblPhone->Location = System::Drawing::Point(20, 135);
            this->lblPhone->Size = System::Drawing::Size(150, 20);

            // txtPhone
            this->txtPhone->Location = System::Drawing::Point(20, 160);
            this->txtPhone->Size = System::Drawing::Size(350, 25);
            this->txtPhone->Font = gcnew System::Drawing::Font("Arial", 10);

            // btnSave
            this->btnSave->Text = L"Сохранить";
            this->btnSave->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
            this->btnSave->ForeColor = Color::White;
            this->btnSave->BackColor = brandBlue;
            this->btnSave->Location = System::Drawing::Point(20, 220);
            this->btnSave->Size = System::Drawing::Size(160, 40);
            this->btnSave->FlatStyle = FlatStyle::Flat;
            this->btnSave->Click += gcnew System::EventHandler(this, &AddClientForm::btnSave_Click);

            // btnCancel
            this->btnCancel->Text = L"Отмена";
            this->btnCancel->Font = gcnew System::Drawing::Font("Arial", 10);
            this->btnCancel->ForeColor = brandBlue;
            this->btnCancel->BackColor = Color::Transparent;
            this->btnCancel->Location = System::Drawing::Point(210, 220);
            this->btnCancel->Size = System::Drawing::Size(160, 40);
            this->btnCancel->FlatStyle = FlatStyle::Flat;
            this->btnCancel->Click += gcnew System::EventHandler(this, &AddClientForm::btnCancel_Click);

            // AddClientForm конфигурация окна
            this->ClientSize = System::Drawing::Size(390, 290);
            this->Controls->Add(this->lblHeader);
            this->Controls->Add(this->lblFIO);
            this->Controls->Add(this->txtFIO);
            this->Controls->Add(this->lblPhone);
            this->Controls->Add(this->txtPhone);
            this->Controls->Add(this->btnSave);
            this->Controls->Add(this->btnCancel);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox = false;
            this->MinimizeBox = false;
            this->StartPosition = FormStartPosition::CenterParent;
            this->Text = L"Новый клиент";
            this->ResumeLayout(false);
            this->PerformLayout();
        }

    private:
        void AddClientForm_Load(System::Object^ sender, System::EventArgs^ e) {
            RoundCorners(this->btnSave, 15);
            RoundCorners(this->btnCancel, 15);
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

        // ОБРАБОТЧИК КНОПКИ СОХРАНЕНИЯ С ВАЛИДАЦИЕЙ ВВОДА
        void btnSave_Click(System::Object^ sender, System::EventArgs^ e) {
            String^ fio = txtFIO->Text->Trim();
            String^ phone = txtPhone->Text->Trim();

            // 1. Проверка ФИО на пустоту и длину
            if (String::IsNullOrEmpty(fio) || fio->Length < 2) {
                MessageBox::Show(L"Ошибка: Поле ФИО должно содержать не менее 2 символов!",
                    L"Ошибка валидации", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            // Проверка каждого символа ФИО (разрешаем только буквы, пробелы и дефис)
            for (int i = 0; i < fio->Length; i++) {
                wchar_t ch = fio[i];
                if (!Char::IsLetter(ch) && ch != L' ' && ch != L'-') {
                    MessageBox::Show(L"Ошибка: ФИО может содержать только буквы, пробелы или дефисы. Никаких цифр и спецсимволов!",
                        L"Ошибка валидации", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                    return;
                }
            }

            // 2. Проверка телефона на пустоту
            if (String::IsNullOrEmpty(phone)) {
                MessageBox::Show(L"Ошибка: Номер телефона не может быть пустым!",
                    L"Ошибка валидации", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            // Проверка телефона на корректность символов и подсчет реальных цифр
            int digitCount = 0;
            for (int i = 0; i < phone->Length; i++) {
                wchar_t ch = phone[i];
                if (Char::IsDigit(ch)) {
                    digitCount++;
                }
                else if (ch != L'+' && ch != L'-' && ch != L' ' && ch != L'(' && ch != L')') {
                    MessageBox::Show(L"Ошибка: Недопустимый символ в номере телефона!\nРазрешены: цифры, '+', '-', пробелы и скобки.",
                        L"Ошибка валидации", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                    return;
                }
            }

            // Защита от ввода строки типа "+( )---"
            if (digitCount < 7) {
                MessageBox::Show(L"Ошибка: Номер телефона должен содержать как минимум 7 цифр!",
                    L"Ошибка валидации", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            // Если все валидации пройдены успешно, передаем статус OK и закрываем форму
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }

        void btnCancel_Click(System::Object^ sender, System::EventArgs^ e) {
            this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->Close();
        }
    };
}