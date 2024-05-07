#ifndef APPLICATION_H
#define APPLICATION_H

#include <QWidget>
#include "tcpclient.h"

class Application : public QWidget
{
    Q_OBJECT
private:
    TCPClient *m_pClient;
    QString m_strHost;
    quint16 m_nPort;
    quint16 m_quantityBytes;
    QTextEdit *m_ptxtInfo;
    QLineEdit *m_plnInputHost;
    QLineEdit *m_plnInputPort;
    QLineEdit *m_plnInputQuantityBytes;
public:
    explicit Application(QWidget *parent = nullptr);
    virtual ~Application();
public slots:
    void slotCreateNewClient();
    void slotConnectClient(const QString information);
    void slotDisconnectClient(const QString information);
    void slotErrorConnectClient(const QString error);
    void slotDeleteClient();
};

#endif // APPLICATION_H
