#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include "application.h"
#include "tcpclient.h"

Application::Application(QWidget *parent)
    : QWidget{parent}
    , m_pClient (nullptr)
    , m_strHost {}
    , m_nPort (0)
    , m_quantityBytes (0)
{
    // создаем объект для вывода информации
    m_ptxtInfo = new QTextEdit(this);
    m_ptxtInfo->setReadOnly(true); // Режим только для чтения

    // Создание строк ввода
    QLabel *lbl_1 = new QLabel ("Host: ");
    m_plnInputHost = new QLineEdit;
    lbl_1->setBuddy(m_plnInputHost);

    QLabel *lbl_2 = new QLabel ("Port: ");
    m_plnInputPort = new QLineEdit;
    lbl_2->setBuddy(m_plnInputPort);

    QLabel *lbl_3 = new QLabel ("Send bytes: ");
    m_plnInputQuantityBytes = new QLineEdit;
    lbl_3->setBuddy(m_plnInputQuantityBytes);

    // Создание окна приложения
    QGridLayout *gBoxlayout = new QGridLayout;

    gBoxlayout->addWidget(lbl_1, 0, 0);
    gBoxlayout->addWidget(m_plnInputHost, 0, 1);
    gBoxlayout->addWidget(lbl_2, 1, 0);
    gBoxlayout->addWidget(m_plnInputPort, 1, 1);
    gBoxlayout->addWidget(lbl_3, 2, 0);
    gBoxlayout->addWidget(m_plnInputQuantityBytes, 2, 1);

    // Создание нового клиента по кнопке
    QPushButton *pbtnNewClient = new QPushButton ("Create Client", this);
    pbtnNewClient->setStyleSheet("background-color: blue;");
    connect (pbtnNewClient, SIGNAL(clicked()), this, SLOT(slotCreateNewClient()));

    // Добавление виджетов в окно приложения
    QVBoxLayout *pvbxLayout = new QVBoxLayout(this);
    pvbxLayout->addWidget(new QLabel("<H1>Application</H1>"));
    pvbxLayout->addWidget(m_ptxtInfo);
    pvbxLayout->addLayout(gBoxlayout);
    pvbxLayout->addWidget(pbtnNewClient);
    setLayout(pvbxLayout);
}

Application::~Application() {

}

// Создание нового клиента
void Application::slotCreateNewClient() {
    m_strHost = m_plnInputHost->text();
    m_nPort = (quint16)(m_plnInputPort->text()).toUInt();
    m_quantityBytes = (quint16)(m_plnInputQuantityBytes->text()).toUInt();

    if (!m_strHost.isEmpty() && m_nPort > 0 && m_quantityBytes > 0) {
        m_pClient = new TCPClient(m_strHost, m_nPort, m_quantityBytes);
        if (m_pClient == nullptr) {
            m_ptxtInfo->append("This client couldn't created");
        } else {
            connect(m_pClient, SIGNAL(connectClient(const QString)), this, SLOT(slotConnectClient(const QString)));
            connect(m_pClient, SIGNAL(disconnectClient(const QString)), this, SLOT(slotDisconnectClient(const QString)));
            connect(m_pClient, SIGNAL(errorConnectClient(const QString)), this, SLOT(slotErrorConnectClient(const QString)));
            connect(m_pClient, SIGNAL(errorConnectClient(const QString)), this, SLOT(slotDeleteClient()));
            connect(m_pClient, SIGNAL(closeWindow()), this, SLOT(slotDeleteClient()));
            m_pClient->show();
        }
    } else {
        m_ptxtInfo->append("Enter number: host, port or quantity bytes");
    }
    m_strHost.clear();
    m_nPort = 0;
    m_quantityBytes = 0;
}

void Application::slotConnectClient(const QString information) {
    m_ptxtInfo->append((QTime::currentTime()).toString() + " The client connected - " + information);
}

void Application::slotDisconnectClient(const QString information) {
    m_ptxtInfo->append((QTime::currentTime()).toString() + " The client disconnected - " + information);
}

void Application::slotErrorConnectClient(const QString error) {
    m_ptxtInfo->append((QTime::currentTime()).toString() + " This client couldn't connected: " + error);
}

void Application::slotDeleteClient() {
    if (m_pClient != nullptr) {
        delete m_pClient;
    }
}
