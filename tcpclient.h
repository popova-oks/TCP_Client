#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QString>
#include <QTime>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpSocket>

class TCPClient : public QWidget {
    Q_OBJECT
private:
    QString m_strHost;
    quint16 m_nPort;
    quint16 m_quantityBytes;
    qint16 m_nNextBlockSize;
    QTcpSocket *m_pTcpSocket;
    QTextEdit *m_ptxtInfo;
    QLineEdit *m_ptxtInput;
protected:
    void closeEvent(QCloseEvent *event) override;
public:
    explicit TCPClient(const QString &strHost, const quint16 nPort, const quint16 quantityBytes, QWidget *parent = 0);
    virtual ~TCPClient();
public slots:
    void slotConnectedClient();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError err);
    void slotSendtoServer();
    void slotPBConnected();
    void slotPBDisconnected();
signals:    
    void connectClient(const QString information);
    void disconnectClient(const QString information);
    void errorConnectClient(const QString error);
    void closeWindow();
};

#endif // TCPCLIENT_H
