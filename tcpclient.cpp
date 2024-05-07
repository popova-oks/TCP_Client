#include "tcpclient.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

TCPClient::TCPClient (const QString &strHost, const quint16 nPort, const quint16 quantityBytes, QWidget *parent)
    : QWidget (parent)
    , m_strHost(strHost)
    , m_nPort (nPort)
    , m_quantityBytes (quantityBytes)
    , m_nNextBlockSize (0)
{
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(m_strHost, m_nPort);

    // создаем объекты для вывода и ввода информации
    m_ptxtInfo = new QTextEdit(this);
    m_ptxtInfo->setReadOnly(true); // Режим только для чтения
    m_ptxtInput = new QLineEdit(this);

    // соединяем сигналы от сокета и слоты
    connect(m_pTcpSocket, SIGNAL(connected()), this, SLOT(slotConnectedClient()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(m_ptxtInput, SIGNAL(returnPressed()), this, SLOT(slotSendtoServer()));

    // Устанавливаем заголовок  и размеры окна
    setWindowTitle("TCP Client: host: " + m_strHost + ", port: " + QString::number(m_nPort));
    resize(600, 800);

    QPushButton *pbtnConnect = new QPushButton ("Connect", this);
    pbtnConnect->setStyleSheet("background-color: blue;");
    connect (pbtnConnect, SIGNAL(clicked()), this, SLOT(slotPBConnected()));

    QPushButton *pbtnDisconnect = new QPushButton ("Disconnect", this);
    pbtnDisconnect->setStyleSheet("background-color: red;");
    connect (pbtnDisconnect, SIGNAL(clicked()), this, SLOT(slotPBDisconnected()));

    QVBoxLayout *pvbxLayout = new QVBoxLayout(this);
    //pvbxLayout->addWidget(new QLabel("<H1>Client</H1>"));
    pvbxLayout->addWidget(m_ptxtInfo);
    pvbxLayout->addWidget(m_ptxtInput);
    pvbxLayout->addWidget(pbtnConnect);
    pvbxLayout->addWidget(pbtnDisconnect);
    setLayout(pvbxLayout);
}

TCPClient::~TCPClient() {
    if (m_pTcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_pTcpSocket->disconnectFromHost();
        QString information = "host: " + m_strHost + " , port: " + QString::number(m_nPort);
        emit disconnectClient(information);
    }
}

void TCPClient::slotConnectedClient() {
    if (m_pTcpSocket->state() == QAbstractSocket::ConnectedState) {
        QString information = "host: " + m_strHost + " , port: " + QString::number(m_nPort);
        m_ptxtInfo->append((QTime::currentTime()).toString() + " Socket connected");
        emit connectClient(information);
    } else {
        QString strError = "Connection error: " + m_pTcpSocket->errorString();
        emit errorConnectClient(strError);;
    }
}

// слот для чтения данных из потока
void TCPClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_3);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < (int)sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> time >> str;

        m_ptxtInfo->append(time.toString() + " " + str);
        m_nNextBlockSize = 0;
    }
}

void TCPClient::slotError(QAbstractSocket::SocketError err) {
    QString strError = "Error = " +
            ( err == QAbstractSocket::HostNotFoundError ? "The host not found."
            : err == QAbstractSocket::RemoteHostClosedError ? "Remote host is closed."
            : err == QAbstractSocket::ConnectionRefusedError ? "Connection was refused."
            : QString(m_pTcpSocket->errorString())
            );
    emit errorConnectClient(strError);
}

void TCPClient::slotSendtoServer()
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << QTime::currentTime() << m_ptxtInput->text();

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
    m_ptxtInput->clear();
}

void TCPClient::slotPBConnected(){
    m_pTcpSocket->connectToHost(m_strHost, m_nPort);
}

void TCPClient::slotPBDisconnected(){
    if (m_pTcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_pTcpSocket->disconnectFromHost();
        QString information = "host: " + m_strHost + " , port: " + QString::number(m_nPort);
        emit disconnectClient(information);
        m_ptxtInfo->append((QTime::currentTime()).toString() + " Socket disconnected");
    } else {
        m_ptxtInfo->append((QTime::currentTime()).toString() + " Remote host is already closed");
    }
}

void TCPClient::closeEvent(QCloseEvent *event) {
    emit closeWindow();
    QWidget::closeEvent(event);
}
