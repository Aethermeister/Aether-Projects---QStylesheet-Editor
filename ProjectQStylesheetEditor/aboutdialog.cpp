#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_StyledBackground);

    parent->installEventFilter(this);

    connect(ui->m_ok_btn, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->m_info_lbl, &QLabel::linkActivated, [] (const QString& link) { QDesktopServices::openUrl(QUrl(link)); });
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

bool AboutDialog::eventFilter(QObject* watched, QEvent* event)
{
    if(watched == parentWidget() && event->type() == QEvent::Resize)
    {
        move(parentWidget()->rect().center() - rect().center());
    }

    return QWidget::eventFilter(watched, event);
}

void AboutDialog::showEvent(QShowEvent* event)
{
    move(parentWidget()->rect().center() - rect().center());
    QWidget::showEvent(event);
}
