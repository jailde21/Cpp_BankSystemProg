#pragma once
#include "BankCore.h"

namespace CppCLRWinFormsProject {
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;

    public ref class StatsManager {
    private:
        // Хранилище посчитанных метрик для отрисовки графиков
        static int debitCount = 0;
        static int savingsCount = 0;
        static int creditCount = 0;
        static double totalCapital = 0;
        static int totalClients = 0;

        static Label^ lblTotalCapital;
        static Label^ lblTotalClients;
        static Panel^ chartPanel;

    public:
        // Метод динамического построения интерфейса статистики
        static Panel^ CreateStatsPanel(int width, int height) {
            Panel^ panel = gcnew Panel();
            panel->Size = System::Drawing::Size(width, height);
            panel->Dock = DockStyle::Fill;
            panel->BackColor = Color::FromArgb(226, 232, 240);
            panel->Visible = false;

            // Заголовок раздела
            Label^ lblTitle = gcnew Label();
            lblTitle->Text = L"Аналитика и статистика банка";
            lblTitle->Font = gcnew System::Drawing::Font("Arial", 16, FontStyle::Bold);
            lblTitle->Location = System::Drawing::Point(25, 20);
            lblTitle->Size = System::Drawing::Size(400, 35);
            panel->Controls->Add(lblTitle);

            // Карточка 1: Общий капитал банка
            Panel^ card1 = gcnew Panel();
            card1->Location = System::Drawing::Point(25, 75);
            card1->Size = System::Drawing::Size(420, 100);
            card1->BackColor = Color::White;

            Label^ l1 = gcnew Label();
            l1->Text = L"Общий капитал (баланс всех счетов):";
            l1->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Regular);
            l1->Location = System::Drawing::Point(15, 15);
            l1->Size = System::Drawing::Size(300, 20);
            card1->Controls->Add(l1);

            lblTotalCapital = gcnew Label();
            lblTotalCapital->Text = L"0.00 BYN";
            lblTotalCapital->Font = gcnew System::Drawing::Font("Arial", 14, FontStyle::Bold);
            lblTotalCapital->ForeColor = Color::FromArgb(11, 0, 163); 
            lblTotalCapital->Location = System::Drawing::Point(15, 45);
            lblTotalCapital->Size = System::Drawing::Size(390, 30);
            card1->Controls->Add(lblTotalCapital);
            panel->Controls->Add(card1);

            // Карточка 2: Всего клиентов в базе
            Panel^ card2 = gcnew Panel();
            card2->Location = System::Drawing::Point(470, 75);
            card2->Size = System::Drawing::Size(420, 100);
            card2->BackColor = Color::White;

            Label^ l2 = gcnew Label();
            l2->Text = L"Всего активных клиентов:";
            l2->Font = gcnew System::Drawing::Font("Arial", 10, FontStyle::Regular);
            l2->Location = System::Drawing::Point(15, 15);
            l2->Size = System::Drawing::Size(300, 20);
            card2->Controls->Add(l2);

            lblTotalClients = gcnew Label();
            lblTotalClients->Text = L"0";
            lblTotalClients->Font = gcnew System::Drawing::Font("Arial", 14, FontStyle::Bold);
            lblTotalClients->ForeColor = Color::FromArgb(11, 0, 163);
            lblTotalClients->Location = System::Drawing::Point(15, 45);
            lblTotalClients->Size = System::Drawing::Size(390, 30);
            card2->Controls->Add(lblTotalClients);
            panel->Controls->Add(card2);

            // Большая карточка для холста графика
            Panel^ graphCard = gcnew Panel();
            graphCard->Location = System::Drawing::Point(25, 200);
            graphCard->Size = System::Drawing::Size(865, 330);
            graphCard->BackColor = Color::White;

            Label^ lChartTitle = gcnew Label();
            lChartTitle->Text = L"Соотношение типов открытых счетов в системе";
            lChartTitle->Font = gcnew System::Drawing::Font("Arial", 12, FontStyle::Bold);
            lChartTitle->Location = System::Drawing::Point(15, 15);
            lChartTitle->Size = System::Drawing::Size(400, 25);
            graphCard->Controls->Add(lChartTitle);

            // Сам холст для рисования графика
            chartPanel = gcnew Panel();
            chartPanel->Location = System::Drawing::Point(15, 50);
            chartPanel->Size = System::Drawing::Size(835, 260);
            chartPanel->BackColor = Color::FromArgb(250, 251, 252);
            chartPanel->Paint += gcnew PaintEventHandler(&StatsManager::OnChartPaint);
            graphCard->Controls->Add(chartPanel);
            panel->Controls->Add(graphCard);

            // Применяем скругления углов
            RoundControl(card1, 15);
            RoundControl(card2, 15);
            RoundControl(graphCard, 18);

            return panel;
        }

        // Пересчет метрик на основе актуального списка клиентов
        static void RefreshStats(List<Client^>^ clients) {
            debitCount = 0;
            savingsCount = 0;
            creditCount = 0;
            totalCapital = 0;
            totalClients = clients->Count;

            for each (Client ^ cl in clients) {
                for each (BankAccount ^ acc in cl->Accounts) {
                    totalCapital += acc->Balance;
                    String^ type = acc->GetAccountType();
                    if (type == L"Дебетовый") debitCount++;
                    else if (type == L"Сберегательный") savingsCount++;
                    else if (type == L"Кредитный") creditCount++;
                }
            }

            if (lblTotalCapital != nullptr) lblTotalCapital->Text = totalCapital.ToString("N2") + " BYN";
            if (lblTotalClients != nullptr) lblTotalClients->Text = totalClients.ToString();
            if (chartPanel != nullptr) chartPanel->Invalidate(); // Заставляем график перерисоваться
        }

    private:
        static void RoundControl(Control^ control, int radius) {
            System::Drawing::Drawing2D::GraphicsPath^ path = gcnew System::Drawing::Drawing2D::GraphicsPath();
            path->StartFigure();
            path->AddArc(0, 0, radius, radius, 180, 90);
            path->AddArc(control->Width - radius, 0, radius, radius, 270, 90);
            path->AddArc(control->Width - radius, control->Height - radius, radius, radius, 0, 90);
            path->AddArc(0, control->Height - radius, radius, radius, 90, 90);
            path->CloseFigure();
            control->Region = gcnew System::Drawing::Region(path);
        }

        // РИСОВАНИЕ СТИЛЬНОГО ГРАФИКА ЧЕРЕЗ GDI+
        static void OnChartPaint(Object^ sender, PaintEventArgs^ e) {
            Graphics^ g = e->Graphics;
            g->SmoothingMode = System::Drawing::Drawing2D::SmoothingMode::AntiAlias;

            int maxCount = Math::Max(debitCount, Math::Max(savingsCount, creditCount));
            if (maxCount == 0) maxCount = 1; // Защита от деления на ноль

            int chartHeight = chartPanel->Height - 60;
            int startY = chartPanel->Height - 40;
            int startX = 120;
            int barWidth = 90;
            int gap = 160;

            array<int>^ counts = { debitCount, savingsCount, creditCount };
            array<String^>^ labels = { L"Дебетовые", L"Сберегательные", L"Кредитные" };

            // Цветовая палитра под стать дизайну
            array<Color>^ colors = {
                Color::FromArgb(0, 102, 255),   // Яркий синий
                Color::FromArgb(255, 153, 0),  // Оранжевый
                Color::FromArgb(220, 20, 60)    // Малиново-красный
            };

            // Рисуем пунктирную сетку
            Pen^ gridPen = gcnew Pen(Color::FromArgb(220, 225, 230), 1.0f);
            gridPen->DashStyle = System::Drawing::Drawing2D::DashStyle::Dash;
            for (int i = 1; i <= 4; i++) {
                int y = startY - (chartHeight / 4) * i;
                g->DrawLine(gridPen, startX - 30, y, startX + gap * 3, y);
            }

            // Ось X
            Pen^ axisPen = gcnew Pen(Color::DarkGray, 1.5f);
            g->DrawLine(axisPen, startX - 30, startY, startX + gap * 2 + barWidth + 30, startY);

            // Отрисовка столбцов диаграммы
            for (int i = 0; i < 3; i++) {
                int x = startX + i * gap;
                int currentBarHeight = (int)(((double)counts[i] / maxCount) * chartHeight);
                int y = startY - currentBarHeight;

                // столбец с заливкой
                Brush^ barBrush = gcnew SolidBrush(colors[i]);
                g->FillRectangle(barBrush, x, y, barWidth, currentBarHeight);

                // Количество над столбцом
                System::Drawing::Font^ fontValue = gcnew System::Drawing::Font("Arial", 10, FontStyle::Bold);
                Brush^ brushText = gcnew SolidBrush(Color::FromArgb(30, 30, 30));
                g->DrawString(counts[i].ToString(), fontValue, brushText, x + (barWidth / 2) - 10, y - 22);

                // Подпись под столбцом
                System::Drawing::Font^ fontLabel = gcnew System::Drawing::Font("Arial", 9, FontStyle::Bold);
                g->DrawString(labels[i], fontLabel, brushText, x, startY + 12);
            }
        }
    };
}