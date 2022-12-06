#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainWindow *ui;

    QMap<QString, QString> m_variables;
    QTimer* m_apply_changes_timer = nullptr;

    void CollectVariables();
    QString CreateStylesheetText();
    void ApplyStylesheetOnPreview(const QString& stylesheet_text);

    QJsonDocument CreateQSEContent();
    void ParseQSEContent(const QJsonDocument& qse_json_document);

private slots:
    void ApplyChanges();

    void Open();
    void SaveAs();
    void Export();
};
#endif // MAINWINDOW_H
