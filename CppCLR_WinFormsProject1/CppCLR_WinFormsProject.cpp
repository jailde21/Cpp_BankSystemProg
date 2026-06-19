#include "pch.h"

// 1. СНАЧАЛА ВСЕ ИНКЛЮДЫ (Нативные файлы и файлы форм)
#include "DatabaseManager.h"
#include "Form1.h"

// 2. И ТОЛЬКО ПОТОМ СТРОГО ПОД НИМИ — USING NAMESPACE!
using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<System::String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew CppCLRWinFormsProject::Form1());
    return 0;
}