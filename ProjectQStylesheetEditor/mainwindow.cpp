#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"

#include <QDebug>
#include <QGroupBox>
#include <QKeyEvent>
#include <QSplitterHandle>
#include <QFileDialog>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_apply_changes_timer = new QTimer(this);
    m_apply_changes_timer->setInterval(1500);
    m_apply_changes_timer->setSingleShot(true);
    connect(m_apply_changes_timer, &QTimer::timeout, this, &MainWindow::ApplyChanges);

    ui->m_stylesheet_text_edit->installEventFilter(this);
    connect(ui->m_variables_text_edit, &QTextEdit::textChanged, this, [=] { m_apply_changes_timer->start(); });
    connect(ui->m_stylesheet_text_edit, &QTextEdit::textChanged, this, [=] { m_apply_changes_timer->start(); });

    ui->splitter->setSizes(QList<int>() << 500 << width() - 500);

    connect(ui->m_open_btn, &QPushButton::clicked, this, &MainWindow::Open);
    connect(ui->m_save_as_btn, &QPushButton::clicked, this, &MainWindow::SaveAs);
    connect(ui->m_export_btn, &QPushButton::clicked, this, &MainWindow::Export);

    connect(ui->m_about_btn, &QPushButton::clicked, [=]
    {
        auto* about_dialog = new AboutDialog(this);
        about_dialog->show();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if(watched == ui->m_stylesheet_text_edit && event->type() == QEvent::KeyPress)
    {
        auto* key_event = dynamic_cast<QKeyEvent*>(event);
        if(key_event->key() == Qt::Key_Tab)
        {
            ui->m_stylesheet_text_edit->textCursor().insertText("\t");
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void MainWindow::CollectVariables()
{
    m_variables.clear();

    const auto variables_text = ui->m_variables_text_edit->toPlainText();
    for(const auto& text_line : variables_text.split('\n'))
    {
        if(text_line.startsWith('$') && text_line.contains(':'))
        {
            const auto variable = text_line.split(':');
            const auto variable_name = variable[0].trimmed();
            const auto variable_value = variable[1].trimmed();

            m_variables.insert(variable_name, variable_value);
        }
    }
}

QString MainWindow::CreateStylesheetText()
{
    auto stylesheet_text = ui->m_stylesheet_text_edit->toPlainText();
    for(auto it = m_variables.begin(); it != m_variables.end(); ++it)
    {
        stylesheet_text = stylesheet_text.replace(it.key(), it.value());
    }

    return stylesheet_text;
}

void MainWindow::ApplyStylesheetOnPreview(const QString& stylesheet_text)
{
    const auto tab_count = ui->m_widgets_tab_widget->count();
    for(int i = 0; i < tab_count; ++i)
    {
        auto* tab = ui->m_widgets_tab_widget->widget(i);
        auto* tab_scroll_area = tab->findChild<QScrollArea*>(QString(), Qt::FindDirectChildrenOnly);
        auto* tab_scroll_area_content_widget = tab_scroll_area->widget();

        for(auto* container_group_box : tab_scroll_area_content_widget->findChildren<QGroupBox*>(QString(), Qt::FindDirectChildrenOnly))
        {
            for(auto* widget : container_group_box->findChildren<QWidget*>())
            {
                widget->setStyleSheet(stylesheet_text);
            }
        }
    }
}

QJsonDocument MainWindow::CreateQSEContent()
{
    const auto variables_text = ui->m_variables_text_edit->toPlainText();
    const auto stylesheet_text = ui->m_stylesheet_text_edit->toPlainText();

    QJsonObject root_object;
    root_object.insert("variables", variables_text);
    root_object.insert("stylesheet", stylesheet_text);

    return QJsonDocument(root_object);
}

void MainWindow::ParseQSEContent(const QJsonDocument& qse_json_document)
{
    const auto qse_json_object = qse_json_document.object();
    const auto variables_text = qse_json_object.value("variables").toString();
    const auto stylesheet_text = qse_json_object.value("stylesheet").toString();

    ui->m_variables_text_edit->setText(variables_text);
    ui->m_stylesheet_text_edit->setText(stylesheet_text);
}

void MainWindow::ApplyChanges()
{
    CollectVariables();
    const auto stylesheet_text = CreateStylesheetText();
    ApplyStylesheetOnPreview(stylesheet_text);
}

void MainWindow::Open()
{
    QFileDialog::getOpenFileContent("QStylesheet Editor File (*.qse)", [=] (const QString& filename, const QString& file_content)
    {
        if(!filename.isEmpty())
        {
            const auto qse_content_json_document = QJsonDocument::fromJson(file_content.toUtf8());
            ParseQSEContent(qse_content_json_document);
        }
    });
}

void MainWindow::SaveAs()
{
    const auto qse_content_json_document = CreateQSEContent();
    const auto qse_content_text = qse_content_json_document.toJson();
    QFileDialog::saveFileContent(qse_content_text, "stylesheet.qse");
}

void MainWindow::Export()
{
    const auto stylesheet_text = CreateStylesheetText().toUtf8();
    QFileDialog::saveFileContent(stylesheet_text, "stylesheet.qss");
}

