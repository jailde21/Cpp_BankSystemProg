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
		// Элементы интерфейса
		Label^ lblHeader;
		Label^ lblFIO;
		Label^ lblPhone;

	public:
		// Свойства для того, чтобы главная форма могла забрать введенные данные
		TextBox^ txtFIO;
		TextBox^ txtPhone;
		Button^ btnSave;
		Button^ btnCancel;

		AddClientForm(void)
		{
			InitializeComponent();
			// Привязываем скругление кнопок на загрузку формы
			this->Load += gcnew System::EventHandler(this, &AddClientForm::AddClientForm_Load);
		}

	protected:
		~AddClientForm()
		{
			if (components) delete components;
		}

	private:
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			Color brandBlue = Color::FromArgb(11, 0, 163);
			Color bgGray = Color::FromArgb(226, 232, 240);

			this->lblHeader = (gcnew Label());
			this->lblFIO = (gcnew Label());
			this->txtFIO = (gcnew TextBox());
			this->lblPhone = (gcnew Label());
			this->txtPhone = (gcnew TextBox());
			this->btnSave = (gcnew Button());
			this->btnCancel = (gcnew Button());

			this->SuspendLayout();

			// Настройки самого окна
			this->Size = System::Drawing::Size(420, 320);
			this->Text = L"Новый клиент";
			this->StartPosition = FormStartPosition::CenterParent; 
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->BackColor = bgGray;

			// Заголовок
			this->lblHeader->Text = L"РЕГИСТРАЦИЯ КЛИЕНТА";
			this->lblHeader->Font = gcnew System::Drawing::Font("Arial", 12, FontStyle::Bold);
			this->lblHeader->ForeColor = brandBlue;
			this->lblHeader->Location = System::Drawing::Point(20, 20);
			this->lblHeader->Size = System::Drawing::Size(360, 30);
			this->lblHeader->TextAlign = ContentAlignment::MiddleCenter;

			// Поле ФИО
			this->lblFIO->Text = L"ФИО Клиента:";
			this->lblFIO->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
			this->lblFIO->Location = System::Drawing::Point(30, 70);
			this->lblFIO->Size = System::Drawing::Size(340, 20);

			this->txtFIO->Font = gcnew System::Drawing::Font("Arial", 11);
			this->txtFIO->Location = System::Drawing::Point(30, 95);
			this->txtFIO->Size = System::Drawing::Size(340, 26);

			// Поле Телефон
			this->lblPhone->Text = L"Номер телефона:";
			this->lblPhone->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
			this->lblPhone->Location = System::Drawing::Point(30, 140);
			this->lblPhone->Size = System::Drawing::Size(340, 20);

			this->txtPhone->Font = gcnew System::Drawing::Font("Arial", 11);
			this->txtPhone->Location = System::Drawing::Point(30, 165);
			this->txtPhone->Size = System::Drawing::Size(340, 26);

			// Кнопка Сохранить
			this->btnSave->Text = L"Создать";
			this->btnSave->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
			this->btnSave->ForeColor = Color::White;
			this->btnSave->BackColor = brandBlue;
			this->btnSave->FlatStyle = FlatStyle::Flat;
			this->btnSave->Location = System::Drawing::Point(30, 220);
			this->btnSave->Size = System::Drawing::Size(160, 40);
			this->btnSave->DialogResult = System::Windows::Forms::DialogResult::OK; // Авто-закрытие окна с результатом OK

			// Кнопка Отмена
			this->btnCancel->Text = L"Отмена";
			this->btnCancel->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
			this->btnCancel->ForeColor = brandBlue;
			this->btnCancel->BackColor = Color::Transparent;
			this->btnCancel->FlatStyle = FlatStyle::Flat;
			this->btnCancel->FlatAppearance->BorderColor = brandBlue;
			this->btnCancel->Location = System::Drawing::Point(210, 220);
			this->btnCancel->Size = System::Drawing::Size(160, 40);
			this->btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel; // Авто-закрытие окна с результатом Cancel

			// Добавляем элементы на форму
			this->Controls->Add(this->lblHeader);
			this->Controls->Add(this->lblFIO);
			this->Controls->Add(this->txtFIO);
			this->Controls->Add(this->lblPhone);
			this->Controls->Add(this->txtPhone);
			this->Controls->Add(this->btnSave);
			this->Controls->Add(this->btnCancel);

			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion

	private:
		// Скругление углов для кнопок нового окна
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
	};
}